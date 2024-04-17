namespace LanguageServer.Client
{
    /// <summary>
    /// 用于将消息从服务器发送到客户机的代理类。
    /// </summary>
    /// <param name="connection"></param>
    public sealed class Proxy(Connection connection)
    {
        private WindowProxy? _window;
        private ClientProxy? _client;
        private WorkspaceProxy? _workspace;
        private TextDocumentProxy? _textDocument;

        /// <summary>
        /// 获取用于发送与窗口相关的消息的代理对象。
        /// </summary>
        public WindowProxy Window => _window ??= new WindowProxy(connection);

        /// <summary>
        /// 获取用于发送与客户端相关的消息的代理对象。
        /// </summary>
        public ClientProxy Client => _client ??= new ClientProxy(connection);

        /// <summary>
        /// 获取用于发送与工作区相关的消息代理对象。
        /// </summary>
        public WorkspaceProxy Workspace => _workspace ??= new WorkspaceProxy(connection);

        /// <summary>
        /// 获取用于发送与文本文档相关的消息代理对象。
        /// </summary>
        public TextDocumentProxy TextDocument=>_textDocument ??= new TextDocumentProxy(connection);
    }
}
