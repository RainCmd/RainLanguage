using LanguageServer.Infrastructure.JsonDotNet;
using Newtonsoft.Json;

namespace LanguageServer.Json
{
    public class Serializer
    {
        private static readonly JsonSerializerSettings settings = new()
        {
            NullValueHandling = NullValueHandling.Ignore,
            Converters = [new EitherConverter()]
        };
        public static object? Deserialize(Type? objectType, string json)
        {
            return JsonConvert.DeserializeObject(json, objectType, settings);
        }
        public static T? Deserialize<T>(string json)
        {
            return JsonConvert.DeserializeObject<T>(json);
        }
        public static string Serialize(object? value)
        {
            return JsonConvert.SerializeObject(value, settings);

        }
    }
}
