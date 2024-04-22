﻿namespace RainLanguageServer.RainLanguage
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
            if (Vaild && type.Vaild)
            {
                if (name.Length != type.name.Length) return false;
                for (int i = 0; i < name.Length; i++)
                    if (name[i] != type.name[i])
                        return false;
                return library == type.library &&
                       code == type.code &&
                       dimension == type.dimension;
            }
            else return !Vaild && !type.Vaild;
        }
        public override bool Equals(object? obj)
        {
            return obj is Type type && Equals(type);
        }
        public override int GetHashCode()
        {
            if (!Vaild) return 0;
            var result = HashCode.Combine(library, code, dimension);
            foreach (var item in name) result = HashCode.Combine(item, result);
            return result;
        }
        public static bool operator ==(Type lhs, Type rhs) => lhs.Equals(rhs);
        public static bool operator !=(Type lhs, Type rhs) => !lhs.Equals(rhs);
        public const int LIBRARY_KERNEL = -2;
        public const int LIBRARY_SELF = -3;
        public static readonly Type BOOL = new(LIBRARY_KERNEL, TypeCode.Struct, ["bool"], 0);
        public static readonly Type BYTE = new(LIBRARY_KERNEL, TypeCode.Struct, ["byte"], 0);
        public static readonly Type CHAR = new(LIBRARY_KERNEL, TypeCode.Struct, ["char"], 0);
        public static readonly Type INT = new(LIBRARY_KERNEL, TypeCode.Struct, ["integer"], 0);
        public static readonly Type REAL = new(LIBRARY_KERNEL, TypeCode.Struct, ["real"], 0);
        public static readonly Type REAL2 = new(LIBRARY_KERNEL, TypeCode.Struct, ["real2"], 0);
        public static readonly Type REAL3 = new(LIBRARY_KERNEL, TypeCode.Struct, ["real3"], 0);
        public static readonly Type REAL4 = new(LIBRARY_KERNEL, TypeCode.Struct, ["real4"], 0);
        public static readonly Type ENUM = new(LIBRARY_KERNEL, TypeCode.Struct, ["enum"], 0);
        public static readonly Type TYPE = new(LIBRARY_KERNEL, TypeCode.Struct, ["type"], 0);
        public static readonly Type STRING = new(LIBRARY_KERNEL, TypeCode.Struct, ["string"], 0);
        public static readonly Type ENTITY = new(LIBRARY_KERNEL, TypeCode.Struct, ["entity"], 0);
        public static readonly Type HANDLE = new(LIBRARY_KERNEL, TypeCode.Handle, ["handle"], 0);
        public static readonly Type INTERFACE = new(LIBRARY_KERNEL, TypeCode.Handle, ["interface"], 0);
        public static readonly Type DELEGATE = new(LIBRARY_KERNEL, TypeCode.Handle, ["delegate"], 0);
        public static readonly Type TASK = new(LIBRARY_KERNEL, TypeCode.Handle, ["task"], 0);
        public static readonly Type ARRAY = new(LIBRARY_KERNEL, TypeCode.Handle, ["array"], 0);
    }
    internal readonly struct Tuple(List<Type> types) : IEquatable<Tuple>
    {
        private readonly List<Type> types = types;
        public readonly int Count => types.Count;
        public readonly Type this[int index] => types[index];
        public bool Valid => types != null;
        public bool Equals(Tuple tuple)
        {
            if (Valid && tuple.Valid)
            {
                if (Count != tuple.Count) return false;
                for (int i = 0; i < Count; i++)
                    if (this[i] != tuple[i])
                        return false;
                return true;
            }
            else return !Valid && !tuple.Valid;
        }
        public override bool Equals(object? obj)
        {
            return obj is Tuple tuple && Equals(tuple);
        }
        public override int GetHashCode()
        {
            var result = 0;
            foreach (var type in types) result = HashCode.Combine(result, type.GetHashCode());
            return result;
        }
        public static bool operator ==(Tuple lhs, Tuple rhs) => lhs.Equals(rhs);
        public static bool operator !=(Tuple lhs, Tuple rhs) => !lhs.Equals(rhs);
    }
}
