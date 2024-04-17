using LanguageServer.Parameters.Client;
using LanguageServer.Parameters.Window;

namespace LanguageServer.Client
{
    /// <summary>
    /// 用于发送与窗口相关的消息的代理类。
    /// </summary>
    public sealed class WindowProxy(Connection connection)
    {
        /// <summary>
        /// window/showMessage
        /// 通知从服务器发送到客户端，要求客户端在用户界面中显示特定的消息。
        /// </summary>
        /// <param name="params"></param>
        public void ShowMessage(ShowMessageParams @params)
        {
            connection.SendNotification(new NotificationMessage<ShowMessageParams>
            {
                method = "window/showMessage",
                @params = @params
            });
        }

        /// <summary>
        /// window/showMessageRequest
        /// 请求从服务器发送到客户端，请求客户端在用户界面中显示特定的消息。
        /// </summary>
        /// <param name="params"></param>
        /// <returns></returns>
        public Task<Result<MessageActionItem, ResponseError>> ShowMessageRequest(ShowMessageRequestParams @params)
        {
            var tcs = new TaskCompletionSource<Result<MessageActionItem, ResponseError>>();
            connection.SendRequest(
                new RequestMessage<ShowMessageRequestParams>
                {
                    id = IdGenerator.Next(),
                    method = "window/showMessageRequest",
                    @params = @params
                },
                (ResponseMessage<MessageActionItem, ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }

        /// <summary>
        /// window/logMessage
        /// 通知从服务器发送到客户端，要求客户端记录特定的消息。
        /// </summary>
        /// <param name="params"></param>
        public void LogMessage(LogMessageParams @params)
        {
            connection.SendNotification(new NotificationMessage<LogMessageParams>
            {
                method = "window/logMessage",
                @params = @params
            });
        }

        /// <summary>
        /// telemetry/event
        /// 通知从服务器发送到客户端，要求客户端记录遥测事件。
        /// </summary>
        /// <param name="params"></param>
        public void Event(dynamic @params)
        {
            connection.SendNotification(new NotificationMessage<dynamic>
            {
                method = "telemetry/event",
                @params = @params
            });
        }
    }
}
