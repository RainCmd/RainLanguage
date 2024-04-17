using LanguageServer.Json;
using LanguageServer.Parameters.General;
using Matarillo.IO;
using System.Text;

namespace LanguageServer
{
    public class Connection(Stream input, Stream output)
    {
        private readonly ProtocolReader input = new(input);
        private const byte CR = 13;
        private const byte LF = 10;
        private readonly byte[] separator = [CR, LF];
        private readonly object outputLock = new();

        public readonly RequestHandlerCollection RequestHandlers = new();
        public readonly NotificationHandlerCollection NotificationHandlers = new();
        private readonly ResponseHandlerCollection ResponseHandlers = new();
        private readonly CancellationHandlerCollection CancellationHandlers = new();

        public async Task Listen()
        {
            while (true)
            {
                try
                {
                    if (!await ReadAndHandle()) break;
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }
            }
        }

        public async Task<bool> ReadAndHandle()
        {
            var json = await Read();
            var messageTest = (MessageTest)Serializer.Instance.Deserialize(typeof(MessageTest), json);
            if (messageTest == null) return false;
            if (messageTest.IsRequest) HandleRequest(messageTest.method, messageTest.id, json);
            else if (messageTest.IsResponse) HandleResponse(messageTest.id, json);
            else if (messageTest.IsCancellation) HandleCancellation(json);
            else if (messageTest.IsNotification) HandleNotification(messageTest.method, json);
            return true;
        }

        private void HandleRequest(string method, NumberOrString id, string json)
        {
            if (RequestHandlers.TryGetRequestHandler(method, out var handler))
            {
                try
                {
                    var tokenSource = new CancellationTokenSource();
                    CancellationHandlers.AddCancellationTokenSource(id, tokenSource);
                    var request = Serializer.Instance.Deserialize(handler.RequestType, json);
                    var requestResponse = (ResponseMessageBase)handler.Handle(request, this, tokenSource.Token);
                    CancellationHandlers.RemoveCancellationTokenSource(id);
                    requestResponse.id = id;
                    SendMessage(requestResponse);
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                    var requestErrorResponse = Reflector.CreateErrorResponse(handler.ResponseType, ex.ToString());
                    SendMessage(requestErrorResponse);
                }
            }
            else
            {
                Console.Error.WriteLine($"WARN: server does not support a request '{method}'");
            }
        }

        private void HandleResponse(NumberOrString id, string json)
        {
            if (ResponseHandlers.TryRemoveResponseHandler(id, out var handler))
            {
                var response = Serializer.Instance.Deserialize(handler.ResponseType, json);
                handler.Handle(response);
            }
            else
            {
                var idString = id.IsLeft ? id.Left.ToString() : id.Right;
                Console.Error.WriteLine($"WARN: server does not expect a response to '{idString}'");
            }
        }

        private void HandleCancellation(string json)
        {
            var cancellation = (NotificationMessage<CancelParams>)Serializer.Instance.Deserialize(typeof(NotificationMessage<CancelParams>), json);
            var id = cancellation.@params.id;
            if (CancellationHandlers.TryRemoveCancellationTokenSource(id, out var tokenSource))
            {
                tokenSource.Cancel();
            }
            else
            {
                var idString = id.IsLeft ? id.Left.ToString() : id.Right;
                Console.Error.WriteLine($"WARN: server can not cancel a procedure '{idString}'");
            }
        }

        private void HandleNotification(string method, string json)
        {
            if (NotificationHandlers.TryGetNotificationHandler(method, out var handler))
            {
                var notification = Serializer.Instance.Deserialize(handler.NotificationType, json);
                handler.Handle(notification, this);
            }
            else
            {
                Console.Error.WriteLine($"WARN: server does not support a notification '{method}'");
            }
        }

        public void SendRequest<TRequest, TResponse>(TRequest request, Action<TResponse> responseHandler)
            where TRequest : RequestMessageBase
            where TResponse : ResponseMessageBase
        {
            var handler = new ResponseHandler(request.id, typeof(TResponse), o => responseHandler((TResponse)o));
            ResponseHandlers.AddResponseHandler(handler);
            SendMessage(request);
        }

        public void SendNotification<TNotification>(TNotification notification)
            where TNotification : NotificationMessageBase
        {
            SendMessage(notification);
        }

        public void SendCancellation(NumberOrString id)
        {
            var message = new NotificationMessage<CancelParams> { method = "$/cancelRequest", @params = new CancelParams { id = id } };
            SendMessage(message);
        }

        private void SendMessage(MessageBase message)
        {
            Write(Serializer.Instance.Serialize(typeof(MessageBase), message));
        }

        private void Write(string json)
        {
            var utf8 = Encoding.UTF8.GetBytes(json);
            lock (outputLock)
            {
                using (var writer = new StreamWriter(output, Encoding.ASCII, 1024, true))
                {
                    writer.Write($"Content-Length: {utf8.Length}\r\n");
                    writer.Write("\r\n");
                    writer.Flush();
                }
                output.Write(utf8, 0, utf8.Length);
                output.Flush();
            }
        }

        private async Task<string> Read()
        {
            var contentLength = 0;
            var headerBytes = await input.ReadToSeparatorAsync(separator);
            while (headerBytes.Length != 0)
            {
                var headerLine = Encoding.ASCII.GetString(headerBytes);
                var position = headerLine.IndexOf(": ", StringComparison.Ordinal);
                if (position >= 0)
                {
                    var name = headerLine[..position];
                    var value = headerLine[(position + 2)..];
                    if (string.Equals(name, "Content-Length", StringComparison.Ordinal))
                    {
                        _ = int.TryParse(value, out contentLength);
                    }
                }
                headerBytes = await input.ReadToSeparatorAsync(separator);
            }
            if (contentLength == 0)
            {
                return "";
            }
            var contentBytes = await input.ReadBytesAsync(contentLength);
            return Encoding.UTF8.GetString(contentBytes);
        }
    }
}
