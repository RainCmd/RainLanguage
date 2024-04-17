using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using StreamJsonRpc;

namespace RainLanguageServer
{
    internal class Server
    {
        private JsonRpc? rpc;
        public void SetRPC(JsonRpc? rpc)
        {
            this.rpc = rpc;
        }
    }
}
