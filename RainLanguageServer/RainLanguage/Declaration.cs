using System.Text;

namespace RainLanguageServer.RainLanguage
{
    enum DeclarationCategory
    {
        //                    library		        name                signature
        Invalid,              //程序集名            名称路径            -
        Variable,             //程序集名            名称路径            -
        Function,             //程序集名            名称路径            参数类型列表
        Enum,                 //程序集名            名称路径            -
        EnumElement,          //程序集名            名称路径            -
        Struct,               //程序集名            名称路径            -
        StructVariable,       //程序集名            名称路径            -
        StructFunction,       //程序集名            名称路径            参数类型列表
        Class,                //程序集名            名称路径            -
        Constructor,          //程序集名            名称路径            参数类型列表
        ClassVariable,        //程序集名            名称路径            -
        ClassFunction,        //程序集名            名称路径            参数类型列表
        Interface,            //程序集名            名称路径            -
        InterfaceFunction,    //程序集名            名称路径            参数类型列表
        Delegate,             //程序集名            名称路径            参数类型列表
        Task,                 //程序集名            名称路径            -
        Native,               //程序集名            名称路径            参数类型列表
    }
    internal readonly struct Declaration(string library, Visibility visibility, DeclarationCategory category, string[] name, Tuple signature) : IEquatable<Declaration>
    {
        public readonly string library = library;
        public readonly Visibility visibility = visibility;
        public readonly DeclarationCategory category = category;
        public readonly string[] name = name;//不包括程序集名
        public readonly Tuple signature = signature;
        public bool Vaild => name != null;
        public Type GetDefineType()
        {
            switch (category)
            {
                case DeclarationCategory.Invalid:
                case DeclarationCategory.Variable:
                case DeclarationCategory.Function:
                    break;
                case DeclarationCategory.Enum: return new Type(library, TypeCode.Enum, name, 0);
                case DeclarationCategory.EnumElement: return new Type(library, TypeCode.Enum, name[0..(name.Length - 1)], 0);
                case DeclarationCategory.Struct: return new Type(library, TypeCode.Struct, name, 0);
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction: return new Type(library, TypeCode.Struct, name[0..(name.Length - 1)], 0);
                case DeclarationCategory.Class: return new Type(library, TypeCode.Handle, name, 0);
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction: return new Type(library, TypeCode.Handle, name[0..(name.Length - 1)], 0);
                case DeclarationCategory.Interface: return new Type(library, TypeCode.Interface, name, 0);
                case DeclarationCategory.InterfaceFunction: return new Type(library, TypeCode.Interface, name[0..(name.Length - 1)], 0);
                case DeclarationCategory.Delegate: return new Type(library, TypeCode.Delegate, name, 0);
                case DeclarationCategory.Task: return new Type(library, TypeCode.Task, name, 0);
                case DeclarationCategory.Native:
                    break;
            }
            return default;
        }
        public bool Equals(Declaration declaration)
        {
            if (Vaild && declaration.Vaild)
            {
                if (name != null && declaration.name != null)
                {
                    if (declaration.name.Length != name.Length) return false;
                    for (int i = 0; i < name.Length; i++)
                        if (name[i] != declaration.name[i])
                            return false;
                }
                else if (name != declaration.name) return false;
                return library == declaration.library &&
                       visibility == declaration.visibility &&
                       category == declaration.category &&
                       signature == declaration.signature;
            }
            else return !Vaild && !declaration.Vaild;
        }
        public override readonly bool Equals(object? obj)
        {
            return obj is Declaration declaration && Equals(declaration);
        }
        public override readonly int GetHashCode()
        {
            if (!Vaild) return 0;
            var result = HashCode.Combine(library, visibility, category, signature);
            foreach (var item in name) result = HashCode.Combine(item, result);
            return result;
        }
        public override string ToString()
        {
            if (!Vaild) return "无效的定义";
            var sb = new StringBuilder(library);
            foreach(var item in name)
            {
                sb.Append('.');
                sb.Append(item);
            }
            return sb.ToString();
        }
        public static bool operator ==(Declaration lhs, Declaration rhs) => lhs.Equals(rhs);
        public static bool operator !=(Declaration lhs, Declaration rhs) => !lhs.Equals(rhs);
    }
}
