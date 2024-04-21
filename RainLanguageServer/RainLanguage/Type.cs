namespace RainLanguageServer.RainLanguage
{
    internal enum TypeCode
    {
        Invalid,

        Struct,
        Enum,
        Handle,
        Interface,
        Delegate,
        Task,
    }
    internal readonly struct Type(int library, TypeCode code, string[] name, int dimension) : IEquatable<Type>
    {
        public readonly int library = library;
        public readonly TypeCode code = code;
        public readonly string[] name = name;//不包含程序集名
        public readonly int dimension = dimension;
        public bool Vaild => name != null;
        public bool Equals(Type type)
        {
            if (name.Length != type.name.Length) return false;
            for (int i = 0; i < name.Length; i++)
                if (name[i] != type.name[i])
                    return false;
            return library == type.library &&
             code == type.code &&
             dimension == type.dimension;
        }
        public override bool Equals(object? obj)
        {
            return obj is Type type && Equals(type);
        }
        public override int GetHashCode()
        {
            var name = 0;
            for (int i = 0; i < this.name.Length; i++)
                name = HashCode.Combine(name, this.name[i]);
            return HashCode.Combine(library, code, name, dimension);
        }
        public static bool operator ==(Type lhs, Type rhs) => lhs.Equals(rhs);
        public static bool operator !=(Type lhs, Type rhs) => !lhs.Equals(rhs);
        public const int LIBRARY_KERNEL = -2;
        public const int LIBRARY_SELF = -3;
    }
    internal readonly struct Tuple(List<Type> types) : IEquatable<Tuple>
    {
        private readonly List<Type> types = types;
        public readonly int Count => types.Count;
        public readonly Type this[int index] => types[index];

        public bool Equals(Tuple tuple)
        {
            if (Count != tuple.Count) return false;
            for (int i = 0; i < Count; i++)
                if (this[i] != tuple[i])
                    return false;
            return true;
        }
        public override bool Equals(object? obj)
        {
            return obj is Tuple tuple && Equals(tuple);
        }
        public override int GetHashCode()
        {
            return HashCode.Combine(types, Count);
        }
        public static bool operator ==(Tuple lhs, Tuple rhs) => lhs.Equals(rhs);
        public static bool operator !=(Tuple lhs, Tuple rhs) => !lhs.Equals(rhs);
    }
}
