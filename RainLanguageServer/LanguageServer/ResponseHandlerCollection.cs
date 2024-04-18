using LanguageServer.Json;

namespace LanguageServer
{
    internal delegate void ResponseHandlerDelegate(object response);
    internal class ResponseHandler(NumberOrString id, Type responseType, ResponseHandlerDelegate handler)
    {
        internal NumberOrString Id => id;
        internal Type ResponseType => responseType;

        internal void Handle(object response) => handler(response);
    }
    internal class ResponseHandlerCollection
    {
        private readonly SyncDictionary<NumberOrString, ResponseHandler> responseHandlers = new();
        
        internal void AddResponseHandler(ResponseHandler responseHandler)
        {
            responseHandlers.Set(responseHandler.Id, responseHandler);
        }

        internal bool TryRemoveResponseHandler(NumberOrString id, out ResponseHandler? responseHandler)
        {
            return responseHandlers.TryRemove(id, out responseHandler);
        }
    }
}