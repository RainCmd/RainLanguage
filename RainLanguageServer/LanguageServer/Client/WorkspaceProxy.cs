using LanguageServer.Parameters.Client;
using LanguageServer.Parameters.Workspace;

namespace LanguageServer.Client
{
    /// <summary>
    /// 用于发送与工作空间相关的消息的代理类。
    /// </summary>
    public class WorkspaceProxy(Connection connection)
    {
        /// <summary>
        /// workspace/applyEdit
        /// 请求从服务器发送到客户端，以修改客户端的资源。
        /// </summary>
        /// <param name="params"></param>
        /// <returns></returns>
        public Task<Result<ApplyWorkspaceEditResponse, ResponseError>> ApplyEdit(ApplyWorkspaceEditParams @params)
        {
            var tcs = new TaskCompletionSource<Result<ApplyWorkspaceEditResponse, ResponseError>>();
            connection.SendRequest(
                new RequestMessage<ApplyWorkspaceEditParams>
                {
                    id = IdGenerator.Next(),
                    method = "workspace/applyEdit",
                    @params = @params
                },
                (ResponseMessage<ApplyWorkspaceEditResponse, ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }

        /// <summary>
        /// workspace/workspaceFolders
        /// 请求从服务器发送到客户端，以获取当前打开的工作空间文件夹列表。
        /// </summary>
        /// <returns>
        /// 如果在工具中只打开了一个文件，则返回 null。
        /// 如果工作空间已打开，但未配置任何文件夹，则返回空数组。
        /// </returns>
        /// <seealso>Spec 3.6.0</seealso>
        public Task<Result<WorkspaceFolder[], ResponseError>> WorkspaceFolders()
        {
            var tcs = new TaskCompletionSource<Result<WorkspaceFolder[], ResponseError>>();
            connection.SendRequest(
                new VoidRequestMessage
                {
                    id = IdGenerator.Next(),
                    method = "workspace/workspaceFolders"
                },
                (ResponseMessage<WorkspaceFolder[], ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }

        /// <summary>
        /// workspace/configuration
        /// 请求从服务器发送到客户端，以从客户端获取配置设置。
        /// 请求可以在一次往返中获取n个配置设置。
        /// </summary>
        /// <param name="params">要求的配置部分</param>
        /// <returns>配置设置</returns>
        /// <seealso>Spec 3.6.0</seealso>
        public Task<Result<dynamic[], ResponseError>> Configuration(ConfigurationParams @params)
        {
            var tcs = new TaskCompletionSource<Result<dynamic[], ResponseError>>();
            connection.SendRequest(
                new RequestMessage<ConfigurationParams>
                {
                    id = IdGenerator.Next(),
                    method = "workspace/configuration",
                    @params = @params
                },
                (ResponseMessage<dynamic[], ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }
    }
}
