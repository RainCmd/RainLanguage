namespace LanguageServer
{
    internal delegate void NotificationHandlerDelegate(object notification, Connection connection);
    internal class NotificationHandler(string rpcMethod, Type notificationType, NotificationHandlerDelegate handler)
    {
        internal string RpcMethod => rpcMethod;
        internal Type NotificationType => notificationType;
        internal void Handle(object notification, Connection connection) => handler(notification, connection);
    }
    public class NotificationHandlerCollection
    {
        private readonly Dictionary<string, NotificationHandler> dictionary = [];
        
        public void Clear() => dictionary.Clear();

        public int Count => dictionary.Count;

        public bool ContainsKey(string method) => dictionary.ContainsKey(method);

        public bool Remove(string method) => dictionary.Remove(method);

        public ICollection<string> Keys => dictionary.Keys;

        internal void AddNotificationHandler(NotificationHandler notificationHandler)
        {
            dictionary[notificationHandler.RpcMethod] = notificationHandler;
        }

        internal bool TryGetNotificationHandler(string rpcMethod, out NotificationHandler? notificationHandler)
        {
            return dictionary.TryGetValue(rpcMethod, out notificationHandler);
        }
    }
}