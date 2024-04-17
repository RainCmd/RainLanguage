using LanguageServer.Json;

namespace LanguageServer.Parameters
{
    public class LocationSingleOrArray : Either
    {
        public static implicit operator LocationSingleOrArray(Location value) => new(value);

        public static implicit operator LocationSingleOrArray(Location[] value) => new(value);

        public LocationSingleOrArray(Location value) : base(value, typeof(Location)) { }

        public LocationSingleOrArray(Location[] value) : base(value, typeof(Location[])) { }

        public bool IsSingle => Type == typeof(Location);

        public bool IsArray => Type == typeof(Location[]);

        public Location Single => GetValue<Location>();

        public Location[] Array => GetValue<Location[]>();
    }
}
