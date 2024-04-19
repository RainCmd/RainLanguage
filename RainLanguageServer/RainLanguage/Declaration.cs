namespace RainLanguageServer.RainLanguage
{
    enum DeclarationCategory
    {
        //                    library			index               definition
        Invalid,              //INVALID			INVALID             INVALID
        Variable,             //库				变量索引            NULL
        Function,             //库				方法列表索引        NULL
        Enum,                 //库				枚举索引	        NULL
        EnumElement,          //库				枚举元素索引        枚举索引
        Struct,               //库				方法列表索引        NULL
        StructVariable,       //库				成员变量索引        结构体索引
        StructFunction,       //库				成员方法索引        结构体索引
        Class,                //库				定义索引            NULL
        Constructor,          //库				构造方法列表索引    所属托管类索引
        ClassVariable,        //库				成员变量索引        所属托管类索引
        ClassFunction,        //库				成员方法索引        所属托管类索引
        Interface,            //库				接口索引            NULL
        InterfaceFunction,    //库				接口方法索引        所属接口索引     
        Delegate,             //库				委托类型索引        NULL
        Task,                 //库				任务类型索引        NULL
        Native,               //库				内部方法索引        NULL
        Lambda,               //LIBRARY_SELF	方法列表索引        匿名函数索引
        LambdaClosureValue,   //LIBRARY_SELF	成员变量索引        所属托管类索引
        LocalVariable,        //LIBRARY_SELF	局部变量id          NULL
    }
    internal readonly struct Declaration : IEquatable<Declaration>
    {
        public readonly int library;
        public readonly Visibility visibility;
        public readonly DeclarationCategory category;
        public readonly int index;
        public readonly int definition;

        public bool Equals(Declaration declaration)
        {
            return library == declaration.library &&
                   visibility == declaration.visibility &&
                   category == declaration.category &&
                   index == declaration.index &&
                   definition == declaration.definition;
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
