using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal class Context(CompilingSpace space, HashSet<ISpace> relies, CompilingDeclaration? declaration)
    {
        public readonly CompilingSpace space = space;
        public readonly HashSet<ISpace> relies = relies;
        public readonly CompilingDeclaration? declaration = declaration;

        private bool IsVisiable(ASTManager manager, Declaration declaration, bool isMember)
        {
            if (isMember)
            {
                if (IsVisiable(manager, manager.GetDefineDeclaration(declaration)!.Declaration, false))
                    if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return false;
            }
            else
            {
                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return manager.GetDeclaration(declaration)!.Space.Contain(space);
            }
        }
        public bool IsVisiable(ASTManager manager, Declaration declaration)
        {
            if (declaration.library == Type.LIBRARY_SELF)
            {
                switch (declaration.category)
                {
                    case DeclarationCategory.Invalid: break;
                    case DeclarationCategory.Variable:
                    case DeclarationCategory.Function:
                    case DeclarationCategory.Enum:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.EnumElement:
                        return IsVisiable(manager, declaration, true);
                    case DeclarationCategory.Struct:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.StructVariable:
                        return IsVisiable(manager, declaration, true);
                    case DeclarationCategory.StructFunction:
                        {
                            var define = manager.GetDefineDeclaration(declaration)!;
                            if (define == this.declaration) return true;
                            if (IsVisiable(manager, define.Declaration, false))
                                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                        }
                        return false;
                    case DeclarationCategory.Class:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.Constructor:
                    case DeclarationCategory.ClassVariable:
                    case DeclarationCategory.ClassFunction:
                        {
                            var define = manager.GetDefineDeclaration(declaration)!;
                            if (define == this.declaration) return true;
                            if (IsVisiable(manager, define.Declaration, false))
                                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                                else if (this.declaration?.declaration.category == DeclarationCategory.Class)
                                    if (declaration.visibility.ContainAny(Visibility.Protected))
                                    {
                                        var defineType = define.Declaration.GetDefineType();
                                        for (var index = this.declaration.declaration.GetDefineType(); index.Vaild; index = manager.GetParent(index))
                                            if (index == defineType) return true;
                                    }
                        }
                        return false;
                    case DeclarationCategory.Interface:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.InterfaceFunction:
                        return IsVisiable(manager, declaration, true);
                    case DeclarationCategory.Delegate:
                    case DeclarationCategory.Task:
                    case DeclarationCategory.Native:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.Lambda:
                        break;
                    case DeclarationCategory.LambdaClosureValue:
                        break;
                    case DeclarationCategory.LocalVariable:
                        break;
                }
            }
            return false;
        }
        public bool TryFindSpace(ASTManager manager, TextRange name, out ISpace? space, MessageCollector collector)
        {
            var targetName = name.ToString();
            for (var index = this.space; index != null; index = index.parent)
                if (index.TryGetChild(targetName, out space)) return true;
            var results = new HashSet<ISpace>();
            foreach (var rely in relies)
                if (rely.TryGetChild(targetName, out space)) results.Add(space!);
            if (manager.relies.TryGetValue(targetName, out var library))
                results.Add(library);
            if (results.Count > 0)
            {
                if (results.Count > 1)
                {
                    var message = new StringBuilder();
                    message.AppendLine("依赖空间不明确");
                    foreach (var result in results)
                        message.AppendLine(result.GetFullName());
                    collector.Add(name, CErrorLevel.Error, message.ToString());
                }
                space = results.First();
                return true;
            }
            else if (targetName == manager.kernel.Name)
            {
                space = manager.kernel;
                return true;
            }
            space = default;
            return false;
        }
        public bool TryFindDeclaration(ASTManager manager, TextRange name, out List<IDeclaration> results, MessageCollector collector)
        {
            results = [];
            var targetName = name.ToString();
            if (declaration?.Declaration.category == DeclarationCategory.Struct)
            {
                var compilingStruct = (CompilingStruct)declaration;
                foreach (var variable in compilingStruct.variables)
                    if (variable.Name == targetName)
                    {
                        results.Add(variable);
                        return true;
                    }
                foreach (var function in compilingStruct.functions)
                    if (function.Name == targetName)
                        results.Add(function);
                if (results.Count > 0) return true;
            }
            else if (declaration?.Declaration.category == DeclarationCategory.Class)
            {
                for (var index = declaration.declaration.GetDefineType(); index.Vaild; index = manager.GetParent(index))
                {
                    var @class = (IClass)manager.GetDefineDeclaration(index)!;
                    foreach (var variable in @class.Variables)
                        if (variable.Name == targetName && IsVisiable(manager, variable.Declaration))
                        {
                            results.Add(variable);
                            return true;
                        }
                    foreach (var function in @class.Functions)
                        if (function.Name == targetName && IsVisiable(manager, function.Declaration))
                            results.Add(function);
                }
                if (results.Count > 0) return true;
            }
            else
            {
                for (var index = space; index != null; index = index.parent)
                    if (index.TryGetDeclarations(targetName, out var declarations))
                    {
                        results.AddRange(declarations);
                        var declaration = declarations[0].Declaration;
                        if (declaration.category != DeclarationCategory.Function && declaration.category != DeclarationCategory.Native) break;
                    }
                if (results.Count == 1) return true;
                else if (results.Count > 1)
                {
                    foreach (var declaration in results)
                        if (declaration.Declaration.category != DeclarationCategory.Function && declaration.Declaration.category != DeclarationCategory.Native)
                        {
                            var builder = new StringBuilder();
                            builder.AppendLine("查找申明不明确");
                            foreach (var item in results)
                                builder.AppendLine(item.GetFullName());
                            collector.Add(name, CErrorLevel.Error, builder.ToString());
                            break;
                        }
                    return true;
                }
                else
                {
                    foreach (var rely in relies)
                        if (rely.TryGetDeclarations(targetName, out var declarations))
                            foreach (var declaration in declarations)
                                if (IsVisiable(manager, declaration.Declaration))
                                    results.Add(declaration);
                    if (results.Count == 1) return true;
                    else if (results.Count > 1)
                    {
                        foreach (var declaration in results)
                            if (declaration.Declaration.category != DeclarationCategory.Function && declaration.Declaration.category != DeclarationCategory.Native)
                            {
                                var builder = new StringBuilder();
                                builder.AppendLine("查找申明不明确");
                                foreach (var item in results)
                                    builder.AppendLine(item.GetFullName());
                                collector.Add(name, CErrorLevel.Error, builder.ToString());
                                break;
                            }
                        return true;
                    }
                }
            }
            return false;
        }
        private static void CollectOverideFunctions(HashSet<IDeclaration> filter, ASTManager manager, IClass @class, IFunction function)
        {
            while (@class.Parent.Vaild)
            {
                @class = (IClass)manager.GetDefineDeclaration(@class.Parent)!;
                foreach (var item in @class.Functions)
                    if (item.Name == function.Name && item.Parameters == function.Parameters)
                        filter.Add(item);
            }
        }
        private static void CollectFunctions(ASTManager manager, string name, IInterface @interface, List<IDeclaration> results)
        {
            foreach (var item in @interface.Callables)
                if (item.Name == name)
                    results.Add(item);
            foreach (var item in @interface.Inherits)
                CollectFunctions(manager, name, (IInterface)manager.GetDefineDeclaration(item)!, results);
        }
        public bool TryFindMember(ASTManager manager, TextRange name, Type type, out List<IDeclaration> results)
        {
            results = [];
            if (type.dimension > 0) type = Type.ARRAY;
            else if (type.code == TypeCode.Task) type = Type.TASK;
            var targetName = name.ToString();
            var declaration = manager.GetDefineDeclaration(type)!;
            if (type.code == TypeCode.Struct)
            {
                var abstractStruct = (IStruct)declaration;
                foreach (var item in abstractStruct.Variables)
                    if (item.Name == targetName)
                    {
                        results.Add(item);
                        return true;
                    }
                foreach (var item in abstractStruct.Functions)
                    if (item.Name == targetName && IsVisiable(manager, item.Declaration))
                        results.Add(item);
            }
            else if (type.code == TypeCode.Handle)
            {
                var filter = new HashSet<IDeclaration>();
                for (var index = (IClass)declaration; index != null; index = manager.GetDefineDeclaration(index.Parent) as IClass)
                {
                    foreach (var item in index.Variables)
                        if (item.Name == targetName && IsVisiable(manager, item.Declaration))
                        {
                            results.Add(item);
                            return true;
                        }
                    foreach (var item in index.Functions)
                        if (item.Name == targetName && IsVisiable(manager, item.Declaration) && !filter.Contains(item))
                        {
                            results.Add(item);
                            CollectOverideFunctions(filter, manager, index, item);
                        }
                }
            }
            else if (type.code == TypeCode.Interface) CollectFunctions(manager, targetName, (IInterface)declaration, results);
            return results.Count > 0;
        }
        public bool TryFindDeclaration(ASTManager manager, List<TextRange> name, out List<IDeclaration> results, MessageCollector collector)
        {
            results = [];
            if (name.Count == 1)
            {
                var type = LexicalTypeExtend.Parse(name[0].ToString());
                switch (type)
                {
                    case LexicalType.KeyWord_bool: results.Add(manager.GetDefineDeclaration(Type.BOOL)!); break;
                    case LexicalType.KeyWord_byte: results.Add(manager.GetDefineDeclaration(Type.BYTE)!); break;
                    case LexicalType.KeyWord_char: results.Add(manager.GetDefineDeclaration(Type.CHAR)!); break;
                    case LexicalType.KeyWord_integer: results.Add(manager.GetDefineDeclaration(Type.INT)!); break;
                    case LexicalType.KeyWord_real: results.Add(manager.GetDefineDeclaration(Type.REAL)!); break;
                    case LexicalType.KeyWord_real2: results.Add(manager.GetDefineDeclaration(Type.REAL2)!); break;
                    case LexicalType.KeyWord_real3: results.Add(manager.GetDefineDeclaration(Type.REAL3)!); break;
                    case LexicalType.KeyWord_real4: results.Add(manager.GetDefineDeclaration(Type.REAL4)!); break;
                    case LexicalType.KeyWord_type: results.Add(manager.GetDefineDeclaration(Type.TYPE)!); break;
                    case LexicalType.KeyWord_string: results.Add(manager.GetDefineDeclaration(Type.STRING)!); break;
                    case LexicalType.KeyWord_entity: results.Add(manager.GetDefineDeclaration(Type.ENTITY)!); break;
                    case LexicalType.KeyWord_handle: results.Add(manager.GetDefineDeclaration(Type.HANDLE)!); break;
                    case LexicalType.KeyWord_interface: results.Add(manager.GetDefineDeclaration(Type.INTERFACE)!); break;
                    case LexicalType.KeyWord_delegate: results.Add(manager.GetDefineDeclaration(Type.DELEGATE)!); break;
                    case LexicalType.KeyWord_task: results.Add(manager.GetDefineDeclaration(Type.TASK)!); break;
                    case LexicalType.KeyWord_array: results.Add(manager.GetDefineDeclaration(Type.ARRAY)!); break;
                    default:
                        if (declaration != null)
                        {
                            var targetName = name[0].ToString();
                            if (declaration.declaration.category == DeclarationCategory.Struct)
                            {
                                foreach (var item in ((IStruct)declaration).Variables)
                                    if (item.Name == targetName)
                                        results.Add(item);
                                foreach (var item in ((IStruct)declaration).Functions)
                                    if (item.Name == targetName)
                                        results.Add(item);
                            }
                            else if (declaration.declaration.category == DeclarationCategory.Class)
                            {
                                for (var index = (IClass)declaration; index != null; index = manager.GetDefineDeclaration(index.Parent) as IClass)
                                {
                                    foreach (var item in index.Variables)
                                        if (item.Name == targetName && IsVisiable(manager, item.Declaration))
                                            results.Add(item);
                                    foreach (var item in index.Functions)
                                        if (item.Name == targetName && IsVisiable(manager, item.Declaration))
                                            results.Add(item);
                                }
                            }
                            if (results.Count > 0) return true;
                            for (var index = space; index != null; index = index.parent)
                                if (index.TryGetDeclarations(targetName, out var declarations))
                                {
                                    results.AddRange(declarations);
                                    return true;
                                }
                            foreach (var rely in relies)
                                if (rely.TryGetDeclarations(targetName, out var declarations))
                                    results.AddRange(declarations);
                        }
                        break;
                }
            }
            else if (name.Count > 1)
            {
                if (TryFindSpace(manager, name[0], out var space, collector))
                {
                    for (var i = 1; i < name.Count - 1; i++)
                        if (!space!.TryGetChild(name[i].ToString(), out space))
                            return false;
                    if (space!.TryGetDeclarations(name[^1].ToString(), out results)) return true;
                }
            }
            return results.Count > 0;
        }
    }
}
