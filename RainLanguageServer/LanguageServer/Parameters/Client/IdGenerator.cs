using LanguageServer.Json;

namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// 用于从服务器到客户机的请求消息的id生成器类。
    /// </summary>
    public class IdGenerator
    {
        private static long id;

        /// <summary>
        /// 返回id并增加内部值。
        /// </summary>
        /// <returns></returns>
        public static NumberOrString Next()
        {
            var ns = new NumberOrString(id++);
            if (id < 0) id = 0;
            return ns;
        }
    }
}
