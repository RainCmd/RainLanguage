using Newtonsoft.Json;

namespace RainLanguageServer
{
    internal class UriConverter : JsonConverter
    {
        public override bool CanConvert(Type objectType) => false;//objectType == typeof(Uri);

        public override object? ReadJson(JsonReader reader, Type objectType, object? existingValue, JsonSerializer serializer)
        {
            if (reader.Value == null) return null;
            if (reader.TokenType == JsonToken.String)
                return new Uri(((string)reader.Value).Replace("%3A", ":"));
            if (reader.TokenType == JsonToken.Null)
                return null;
            throw new InvalidOperationException($"UriConterter: 不支持的转换类型：{reader.TokenType}");
        }

        public override void WriteJson(JsonWriter writer, object? value, JsonSerializer serializer)
        {
            if (value == null) writer.WriteNull();
            else if (value is Uri uri)
            {
                var scheme = uri.Scheme;
                var str = uri.ToString();
                if (str.Contains("://"))
                    str = scheme + "://" + str.Substring(scheme.Length + 3).Replace(":", "%3A").Replace('\\', '/');
                writer.WriteValue(str);
            }
            else throw new InvalidOperationException($"UriConterter: 不支持的转换类型：{value.GetType()}");
        }
    }
}
