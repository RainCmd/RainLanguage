using LanguageServer.Client;
using LanguageServer.Parameters.General;

namespace LanguageServer.Server
{
    public class GeneralServiceTemplate : Service
    {
        private Proxy? proxy;

        public override Connection Connection
        {
            get => base.Connection;
            set
            {
                base.Connection = value;
                proxy = new Proxy(value);
            }
        }

        public Proxy Proxy => proxy ??= new Proxy(Connection);

        [JsonRpcMethod("initialize")]
        protected virtual Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams @params)
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("initialized")]
        protected virtual void Initialized()
        {
        }

        [JsonRpcMethod("shutdown")]
        protected virtual VoidResult<ResponseError> Shutdown()
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("exit")]
        protected virtual void Exit()
        {
        }
    }
}
