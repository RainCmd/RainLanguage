namespace RainLanguageServer.RainLanguage
{
    internal interface IDeclaration
    {
        string Name { get; }
        Declaration Declaration { get; }
        int AttributeCount { get; }
        string GetAttribute(int index);
    }
    internal interface IVariable : IDeclaration
    {
        bool IsReadonly { get; }
        Type Type { get; }
    }
    internal interface ICallable : IDeclaration
    {
        Tuple Parameters { get; }
        Tuple Returns { get; }
    }
    internal interface IFunction : ICallable { }
    internal interface IEnum : IDeclaration
    {
        int ElementCount { get; }
        string GetElement(int index);
    }
    internal interface IStruct : IDeclaration
    {
        int VariableCount { get; }
        IVariable GetVariable(int index);
        int FunctionCount { get; }
        IFunction GetFunction(int index);
    }
    internal interface IInterface : IDeclaration
    {
        int InheritCount { get; }
        Type GetInherit(int index);
        int FunctionCount { get; }
        IFunction GetFunction(int index);
    }
    internal interface IClass : IInterface
    {
        Type Parent { get; }
        int ConstructorCount { get; }
        IFunction GetConstructor(int index);
        int VariableCount { get; }
        IVariable GetVariable(int index);
    }
    internal interface IDelegate : ICallable { }
    internal interface ITask : IDeclaration
    {
        Tuple Returns { get; }
    }
    internal interface INative : ICallable { }
    internal interface ISpace
    {
        int Index { get; }
        ISpace Parent { get; }
        string Name { get; }
        int AttributeCount { get; }
        string GetAttribute(int index);
        bool TryGet(string name, out ISpace? child);
        bool TryGet(string name, out List<Declaration>? declarations);
    }
    internal interface ILibrary : ISpace
    {
        int Library { get; }
        int VariableCount { get; }
        IVariable GetVariable(int index);
        int FunctionCount { get; }
        IFunction GetFunction(int index);
        int EnumCount { get; }
        IEnum GetEnum(int index);
        int StructCount { get; }
        IStruct GetStruct(int index);
        int InterfaceCount { get; }
        IInterface GetInterface(int index);
        int ClassCount { get; }
        IClass GetClass(int index);
        int DelegateCount { get; }
        IDelegate GetDelegate(int index);
        int TaskCount { get; }
        ITask GetTask(int index);
        int NativeCount { get; }
        INative GetNative(int index);
    }
}
