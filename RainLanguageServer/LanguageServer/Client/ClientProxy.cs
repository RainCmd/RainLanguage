using LanguageServer.Parameters.Client;

namespace LanguageServer.Client
{
    /// <summary>
    /// 用于发送与客户端相关的消息的代理类。
    /// </summary>
    public sealed class ClientProxy(Connection connection)
    {
        /// <summary>
        /// The <c>client/registerCapability</c> request is sent from the server to the client
        /// to register for a new capability on the client side.
        /// client/registerCapability
        /// 请求从服务器发送到客户端，以在客户端注册新功能。
        /// </summary>
        /// <param name="params"></param>
        /// <returns></returns>
        public Task<VoidResult<ResponseError>> RegisterCapability(RegistrationParams @params)
        {
            var tcs = new TaskCompletionSource<VoidResult<ResponseError>>();
            connection.SendRequest(
                new RequestMessage<RegistrationParams>
                {
                    id = IdGenerator.Next(),
                    method = "client/registerCapability",
                    @params = @params
                },
                (VoidResponseMessage<ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }

        /// <summary>
        /// client/unregisterCapability
        /// 请求从服务器发送到客户端，取消先前注册的功能。
        /// </summary>
        /// <param name="params"></param>
        /// <returns></returns>
        public Task<VoidResult<ResponseError>> UnregisterCapability(UnregistrationParams @params)
        {
            var tcs = new TaskCompletionSource<VoidResult<ResponseError>>();
            connection.SendRequest(
                new RequestMessage<UnregistrationParams>
                {
                    id = IdGenerator.Next(),
                    method = "client/unregisterCapability",
                    @params = @params
                },
                (VoidResponseMessage<ResponseError> res) => tcs.TrySetResult(Message.ToResult(res)));
            return tcs.Task;
        }
    }
}
