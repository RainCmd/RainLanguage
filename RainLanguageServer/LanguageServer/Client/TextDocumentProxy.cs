using LanguageServer.Parameters.TextDocument;

namespace LanguageServer.Client
{
    /// <summary>
    /// 用于发送与文本文档相关的消息的代理类。
    /// </summary>
    public class TextDocumentProxy(Connection connection)
    {
        /// <summary>
        /// textDocument/publishDiagnostics
        /// 通知从服务器发送到客户端，以表示验证运行的结果。
        /// </summary>
        /// <param name="params"></param>
        public void PublishDiagnostics(PublishDiagnosticsParams @params)
        {
            connection.SendNotification(new NotificationMessage<PublishDiagnosticsParams>
            {
                method = "textDocument/publishDiagnostics",
                @params = @params
            });
        }
    }
}
