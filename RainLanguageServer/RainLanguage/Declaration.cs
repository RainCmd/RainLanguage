namespace RainLanguageServer.RainLanguage
{
    enum DeclarationCategory
    {
        //                    library		        name            signature
        Invalid,              //INVALID		                        -
        Variable,             //程序集编号                          -
        Function,             //程序集编号                          参数类型列表
        Enum,                 //程序集编号                          -
        EnumElement,          //程序集编号                          -
        Struct,               //程序集编号                          -
        StructVariable,       //程序集编号                          -
        StructFunction,       //程序集编号                          参数类型列表
        Class,                //程序集编号                          -
        Constructor,          //程序集编号                          参数类型列表
        ClassVariable,        //程序集编号                          -
        ClassFunction,        //程序集编号                          参数类型列表
        Interface,            //程序集编号                          -
        InterfaceFunction,    //程序集编号                          参数类型列表
        Delegate,             //程序集编号                          参数类型列表
        Task,                 //程序集编号                          -
        Native,               //程序集编号                          参数类型列表
        Lambda,               //LIBRARY_SELF                        -
        LambdaClosureValue,   //LIBRARY_SELF                        -
        LocalVariable,        //LIBRARY_SELF                        -
    }
    internal readonly struct Declaration(int library, Visibility visibility, DeclarationCategory category, string[] name, Tuple signature) : IEquatable<Declaration>
    {
        public readonly int library = library;
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
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
                    break;
            }
            return default;
        }
        public bool Equals(Declaration declaration)
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
        public override readonly bool Equals(object? obj)
        {
            return obj is Declaration declaration && Equals(declaration);
        }
        public override readonly int GetHashCode()
        {
            return HashCode.Combine(library, visibility, category, index, definition);
        }
        public static bool operator ==(Declaration lhs, Declaration rhs) => lhs.Equals(rhs);
        public static bool operator !=(Declaration lhs, Declaration rhs) => !lhs.Equals(rhs);
    }
}
