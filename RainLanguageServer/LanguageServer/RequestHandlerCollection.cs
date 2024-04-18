namespace LanguageServer
{
    internal delegate object RequestHandlerDelegate(object request, Connection connection, CancellationToken token);
    internal class RequestHandler(string rpcMethod, Type requestType, Type responseType, RequestHandlerDelegate handler)
    {
        internal string RpcMethod => rpcMethod;
        internal Type RequestType => requestType;
        internal Type ResponseType => responseType;

        internal object Handle(object request, Connection connection, CancellationToken token) => handler(request, connection, token);
    }

    public class RequestHandlerCollection
    {
        private readonly Dictionary<string, RequestHandler> dictionary = [];

        public void Clear() => dictionary.Clear();

        public int Count => dictionary.Count;

        public bool ContainsKey(string method) => dictionary.ContainsKey(method);

        public bool Remove(string method) => dictionary.Remove(method);

        public ICollection<string> Keys => dictionary.Keys;

        internal void AddRequestHandler(RequestHandler requestHandler)
        {
            dictionary[requestHandler.RpcMethod] = requestHandler;
        }

        internal bool TryGetRequestHandler(string rpcMethod, out RequestHandler? requestHandler)
        {
            return dictionary.TryGetValue(rpcMethod, out requestHandler);
        }
    }
}