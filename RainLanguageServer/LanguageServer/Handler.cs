using LanguageServer.Json;

namespace LanguageServer
{
    internal class ResponseHandler(NumberOrString id, Type responseType, ResponseHandlerDelegate handler)
    {
        internal NumberOrString Id => id;
        internal Type ResponseType => responseType;

        internal void Handle(object response)
        {
            handler(response);
        }
    }

    internal delegate void ResponseHandlerDelegate(object response);

    internal class RequestHandler(string rpcMethod, Type requestType, Type responseType, RequestHandlerDelegate handler)
    {
        internal string RpcMethod => rpcMethod;
        internal Type RequestType => requestType;
        internal Type ResponseType => responseType;

        internal object Handle(object request, Connection connection, CancellationToken token)
        {
            return handler(request, connection, token);
        }
    }

    internal delegate object RequestHandlerDelegate(object request, Connection connection, CancellationToken token);

    internal class NotificationHandler(string rpcMethod, Type notificationType, NotificationHandlerDelegate handler)
    {
        internal string RpcMethod => rpcMethod;
        internal Type NotificationType => notificationType;

        internal void Handle(object notification, Connection connection)
        {
            handler(notification, connection);
        }
    }

    internal delegate void NotificationHandlerDelegate(object notification, Connection connection);
}
