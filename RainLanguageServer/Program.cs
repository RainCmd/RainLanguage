using StreamJsonRpc;
using Newtonsoft.Json;

namespace RainLanguageServer
{
    internal class Program
    {
        private class RainStream(Stream stream, StreamWriter? recorder) : Stream
        {
            public override bool CanRead => stream.CanRead;
            public override bool CanSeek => stream.CanSeek;
            public override bool CanWrite => stream.CanWrite;
            public override long Length => stream.Length;
            public override long Position { get => stream.Position; set => stream.Position = value; }
            private void Recorder(bool rw, byte[] buffer, int offset, int count)
            {
                if (recorder == null) return;
                var message = recorder.Encoding.GetString(buffer, offset, count);
                recorder.WriteLine($"{DateTime.Now.ToString("[yy MM dd HH:mm:ss:fff]")}{(rw ? ">>" : " << ")} {message}");
            }
            public override void Flush()
            {
                stream.Flush();
                recorder?.Flush();
            }

            public override int Read(byte[] buffer, int offset, int count)
            {
                var result = stream.Read(buffer, offset, count);
                Recorder(true, buffer, offset, result);
                return result;
            }

            public override long Seek(long offset, SeekOrigin origin)
            {
                return stream.Seek(offset, origin);
            }

            public override void SetLength(long value)
            {
                stream.SetLength(value);
            }

            public override void Write(byte[] buffer, int offset, int count)
            {
                stream.Write(buffer, offset, count);
                Recorder(false, buffer, offset, count);
            }
        }
        private static readonly CancellationTokenSource cancellationSource = new();
        static void Main(string[] args)
        {
            var recorder = args?.Length > 0 ? File.CreateText(args[0] + "server.log") : null;
            var formatter = new JsonMessageFormatter();
            formatter.JsonSerializer.NullValueHandling = NullValueHandling.Ignore;
            formatter.JsonSerializer.ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor;
            formatter.JsonSerializer.Converters.Add(new UriConverter());
            var server = new Server();
            var cin = new RainStream(Console.OpenStandardInput(), recorder);
            var cout = new RainStream(Console.OpenStandardOutput(), recorder);
            using var rpc = new JsonRpc(new HeaderDelimitedMessageHandler(cout, cin, formatter), server);
            server.SetRPC(rpc);
            rpc.StartListening();
            cancellationSource.Token.WaitHandle.WaitOne();
            recorder?.Close();
        }
    }
}
