using System.Collections;
using System.Text;

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
    internal readonly struct Type(string library, TypeCode code, string[] name, int dimension) : IEquatable<Type>
    {
        public readonly string library = library;
        public readonly TypeCode code = code;
        public readonly string[] name = name;//不包含程序集名
        public readonly int dimension = dimension;
        public bool Vaild => name != null && dimension >= 0;
        public bool Managed => Vaild && (dimension > 0 || code >= TypeCode.Handle);
        public Type Source => new(library, code, name, 0);
        public Type GetDimensionType(int dimension) => new(library, code, name, dimension);
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
        public override string ToString()
        {
            return ToString(true, null);
        }
        private static bool GetCommon(CompilingSpace space, string library, string[] names, out int index)
        {
            if (space.parent == null)
            {
                index = 0;
                return space.name == library;
            }
            else if (GetCommon(space.parent, library, names, out index))
            {
                if (index < names.Length - 1 && space.name == names[index])
                {
                    index++;
                    return true;
                }
            }
            return false;
        }
        public string ToString(bool addCode, CompilingSpace? space)
        {
            if (Vaild)
            {
                var sb = new StringBuilder();
                var index = 0;
                if (library != LIBRARY_KERNEL)
                {
                    if (space != null) GetCommon(space, library, name, out index);
                    else sb.Append(library);
                }
                for (var i = index; i < name.Length; ++i)
                {
                    if (sb.Length > 0)
                    {
                        if (i == name.Length - 1) sb.Append(':');
                        else sb.Append('.');
                    }
                    sb.Append(name[i]);
                }
                if (addCode)
                {
                    switch (code)
                    {
                        case TypeCode.Invalid: break;
                        case TypeCode.Struct:
                            return $"struct {sb}";
                        case TypeCode.Enum:
                            return $"enum {sb}";
                        case TypeCode.Handle:
                            return $"handle {sb}";
                        case TypeCode.Interface:
                            return $"interface {sb}";
                        case TypeCode.Delegate:
                            return $"delegate {sb}";
                        case TypeCode.Task:
                            return $"task {sb}";
                    }
                }
                else
                {
                    for (var i = 0; i < dimension; i++) sb.Append("[]");
                    return sb.ToString();
                }
            }
            return "无效的类型";
        }
        public static bool operator ==(Type lhs, Type rhs) => lhs.Equals(rhs);
        public static bool operator !=(Type lhs, Type rhs) => !lhs.Equals(rhs);
        public const string LIBRARY_KERNEL = "kernel";
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
    internal readonly struct Tuple(List<Type> types) : IEquatable<Tuple>, IEnumerable<Type>
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
            if (types == null) return 0;
            var result = 0;
            foreach (var type in types)
                result = HashCode.Combine(result, type.GetHashCode());
            return result;
        }
        public override string ToString()
        {
            var builder = new StringBuilder();
            foreach (var type in types)
            {
                if (builder.Length > 0) builder.Append(", ");
                builder.Append(type.ToString());
            }
            return builder.ToString();
        }

        public IEnumerator<Type> GetEnumerator()
        {
            return types.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public static bool operator ==(Tuple lhs, Tuple rhs) => lhs.Equals(rhs);
        public static bool operator !=(Tuple lhs, Tuple rhs) => !lhs.Equals(rhs);
        public static implicit operator List<Type>(Tuple tuple) => tuple.types;
    }
}
