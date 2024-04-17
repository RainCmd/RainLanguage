using LanguageServer.Infrastructure.JsonDotNet;

namespace LanguageServer.Json
{
    public abstract class Serializer
    {
        public abstract object Deserialize(Type objectType, string json);

        public abstract string Serialize(Type objectType, object value);

        public static readonly Serializer Instance = new JsonDotNetSerializer();
    }
}
