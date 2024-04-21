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
                if (IsVisiable(manager, manager.GetDeclaration(declaration.library, declaration.name.AsSpan()[..(declaration.name.Length - 1)]).Declaration, false))
                    if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return false;
            }
            else
            {
                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return manager.GetDeclaration(declaration).Space.Contain(space);
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
                            var define = manager.GetDeclaration(declaration.library, declaration.name.AsSpan()[..(declaration.name.Length - 1)]);
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
                            var define = manager.GetDeclaration(declaration.library, declaration.name.AsSpan()[..(declaration.name.Length - 1)]);
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
                results.Add(library!);
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
                    var @class = (IClass)manager.GetDeclaration(index.library, index.name);
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
                    if(results.Count == 1) return true;
                    else if(results.Count > 1)
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
        public bool TryFindMember(ASTManager manager, TextRange name, out List<CompilingDeclaration>? declarations)
        {
            declarations = default;
            return false;
        }
    }
}
