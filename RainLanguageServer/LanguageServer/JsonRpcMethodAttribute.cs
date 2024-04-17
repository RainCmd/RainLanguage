namespace LanguageServer
{
    [AttributeUsage(AttributeTargets.Method)]
    public class JsonRpcMethodAttribute(string method) : Attribute
    {
        public string Method => method;
    }
}
