﻿using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal class Context(CompilingSpace space, HashSet<CompilingSpace> relies, CompilingDeclaration? declaration)
    {
        public readonly CompilingSpace space = space;
        public readonly HashSet<CompilingSpace> relies = relies;
        public readonly CompilingDeclaration? declaration = declaration;

        private bool IsVisiable(ASTManager manager, Declaration declaration, bool isMember)
        {
            if (isMember)
            {
                if (IsVisiable(manager, manager.GetDeclaringDeclaration(declaration)!.declaration, false))
                    if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return false;
            }
            else
            {
                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                return manager.GetDeclaration(declaration)!.space.Contain(space);
            }
        }
        public bool IsVisiable(ASTManager manager, Declaration declaration)
        {
            if (declaration.library == space.Library.name)
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
                            var define = manager.GetDeclaringDeclaration(declaration)!;
                            if (define == this.declaration) return true;
                            if (IsVisiable(manager, define.declaration, false))
                                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                        }
                        return false;
                    case DeclarationCategory.Class:
                        return IsVisiable(manager, declaration, false);
                    case DeclarationCategory.Constructor:
                    case DeclarationCategory.ClassVariable:
                    case DeclarationCategory.ClassFunction:
                        {
                            var define = manager.GetDeclaringDeclaration(declaration)!;
                            if (define == this.declaration) return true;
                            if (IsVisiable(manager, define.declaration, false))
                                if (declaration.visibility.ContainAny(Visibility.Public | Visibility.Internal)) return true;
                                else if (this.declaration?.declaration.category == DeclarationCategory.Class)
                                    if (declaration.visibility.ContainAny(Visibility.Protected))
                                    {
                                        var defineType = define.declaration.GetDefineType();
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
        public bool TryFindSpace(ASTManager manager, TextRange name, out CompilingSpace? space, MessageCollector collector)
        {
            var targetName = name.ToString();
            for (var index = this.space; index != null; index = index.parent)
                if (index.children.TryGetValue(targetName, out space)) return true;
            var results = new HashSet<CompilingSpace>();
            foreach (var rely in relies)
                if (rely.children.TryGetValue(targetName, out space)) results.Add(space!);
            if (manager.relies.TryGetValue(targetName, out var library))
                results.Add(library);
            if (results.Count > 0)
            {
                if (results.Count > 1)
                {
                    var message = new StringBuilder().AppendLine("依赖空间不明确");
                    foreach (var result in results)
                        message.AppendLine(result.GetFullName());
                    collector.Add(name, CErrorLevel.Error, message.ToString());
                }
                space = results.First();
                return true;
            }
            else if (targetName == manager.kernel.name)
            {
                space = manager.kernel;
                return true;
            }
            space = default;
            return false;
        }
        public bool TryFindDeclaration(ASTManager manager, TextRange name, out List<CompilingDeclaration> results, MessageCollector collector)
        {
            results = [];
            var targetName = name.ToString();
            if (declaration?.declaration.category == DeclarationCategory.Struct)
            {
                var compilingStruct = (CompilingStruct)declaration;
                foreach (var variable in compilingStruct.variables)
                    if (variable.name == targetName)
                    {
                        results.Add(variable);
                        return true;
                    }
                foreach (var function in compilingStruct.functions)
                    if (function.name == targetName)
                        results.Add(function);
                if (results.Count > 0) return true;
            }
            else if (declaration?.declaration.category == DeclarationCategory.Class)
            {
                for (var index = declaration.declaration.GetDefineType(); index.Vaild; index = manager.GetParent(index))
                {
                    var @class = (CompilingClass)manager.GetSourceDeclaration(index)!;
                    foreach (var variable in @class.variables)
                        if (variable.name == targetName && IsVisiable(manager, variable.declaration))
                        {
                            results.Add(variable);
                            return true;
                        }
                    foreach (var function in @class.functions)
                        if (function.name == targetName && IsVisiable(manager, function.declaration))
                            results.Add(function);
                }
                if (results.Count > 0) return true;
            }
            else
            {
                for (var index = space; index != null; index = index.parent)
                    if (index.declarations.TryGetValue(targetName, out var declarations))
                    {
                        results.AddRange(declarations);
                        var declaration = declarations[0].declaration;
                        if (declaration.category != DeclarationCategory.Function && declaration.category != DeclarationCategory.Native) break;
                    }
                if (results.Count == 1) return true;
                else if (results.Count > 1)
                {
                    foreach (var declaration in results)
                        if (declaration.declaration.category != DeclarationCategory.Function && declaration.declaration.category != DeclarationCategory.Native)
                        {
                            var builder = new StringBuilder().AppendLine("查找申明不明确");
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
                        if (rely.declarations.TryGetValue(targetName, out var declarations))
                            foreach (var declaration in declarations)
                                if (IsVisiable(manager, declaration.declaration))
                                    results.Add(declaration);
                    if (results.Count == 1) return true;
                    else if (results.Count > 1)
                    {
                        foreach (var declaration in results)
                            if (declaration.declaration.category != DeclarationCategory.Function && declaration.declaration.category != DeclarationCategory.Native)
                            {
                                var builder = new StringBuilder().AppendLine("查找申明不明确");
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
        private static void CollectOverideFunctions(HashSet<CompilingDeclaration> filter, ASTManager manager, CompilingClass @class, CompilingFunction function)
        {
            while (@class.parent.Vaild)
            {
                @class = (CompilingClass)manager.GetSourceDeclaration(@class.parent)!;
                foreach (var item in @class.functions)
                    if (item.name == function.name && item.parameters == function.parameters)
                        filter.Add(item);
            }
        }
        private static void CollectFunctions(ASTManager manager, string name, CompilingInterface @interface, List<CompilingDeclaration> results)
        {
            foreach (var item in @interface.callables)
                if (item.name == name)
                    results.Add(item);
            foreach (var item in @interface.inherits)
                CollectFunctions(manager, name, (CompilingInterface)manager.GetSourceDeclaration(item)!, results);
        }
        public bool TryFindMember(ASTManager manager, TextRange name, Type type, out List<CompilingDeclaration> results)
        {
            results = [];
            if (type.dimension > 0) type = Type.ARRAY;
            else if (type.code == TypeCode.Task) type = Type.TASK;
            var targetName = name.ToString();
            var declaration = manager.GetSourceDeclaration(type)!;
            if (type.code == TypeCode.Struct)
            {
                var compiling = (CompilingStruct)declaration;
                foreach (var item in compiling.variables)
                    if (item.name == targetName)
                    {
                        results.Add(item);
                        return true;
                    }
                foreach (var item in compiling.functions)
                    if (item.name == targetName && IsVisiable(manager, item.declaration))
                        results.Add(item);
            }
            else if (type.code == TypeCode.Handle)
            {
                var filter = new HashSet<CompilingDeclaration>();
                for (var index = (CompilingClass)declaration; index != null; index = manager.GetSourceDeclaration(index.parent) as CompilingClass)
                {
                    foreach (var item in index.variables)
                        if (item.name == targetName && IsVisiable(manager, item.declaration))
                        {
                            results.Add(item);
                            return true;
                        }
                    foreach (var item in index.functions)
                        if (item.name == targetName && IsVisiable(manager, item.declaration) && !filter.Contains(item))
                        {
                            results.Add(item);
                            CollectOverideFunctions(filter, manager, index, item);
                        }
                }
            }
            else if (type.code == TypeCode.Interface) CollectFunctions(manager, targetName, (CompilingInterface)declaration, results);
            return results.Count > 0;
        }
        public bool TryFindDeclaration(ASTManager manager, List<TextRange> name, out List<CompilingDeclaration> results, MessageCollector collector)
        {
            results = [];
            if (name.Count == 1)
            {
                var type = LexicalTypeExtend.Parse(name[0].ToString());
                switch (type)
                {
                    case LexicalType.KeyWord_bool: results.Add(manager.GetSourceDeclaration(Type.BOOL)!); break;
                    case LexicalType.KeyWord_byte: results.Add(manager.GetSourceDeclaration(Type.BYTE)!); break;
                    case LexicalType.KeyWord_char: results.Add(manager.GetSourceDeclaration(Type.CHAR)!); break;
                    case LexicalType.KeyWord_integer: results.Add(manager.GetSourceDeclaration(Type.INT)!); break;
                    case LexicalType.KeyWord_real: results.Add(manager.GetSourceDeclaration(Type.REAL)!); break;
                    case LexicalType.KeyWord_real2: results.Add(manager.GetSourceDeclaration(Type.REAL2)!); break;
                    case LexicalType.KeyWord_real3: results.Add(manager.GetSourceDeclaration(Type.REAL3)!); break;
                    case LexicalType.KeyWord_real4: results.Add(manager.GetSourceDeclaration(Type.REAL4)!); break;
                    case LexicalType.KeyWord_type: results.Add(manager.GetSourceDeclaration(Type.TYPE)!); break;
                    case LexicalType.KeyWord_string: results.Add(manager.GetSourceDeclaration(Type.STRING)!); break;
                    case LexicalType.KeyWord_entity: results.Add(manager.GetSourceDeclaration(Type.ENTITY)!); break;
                    case LexicalType.KeyWord_handle: results.Add(manager.GetSourceDeclaration(Type.HANDLE)!); break;
                    case LexicalType.KeyWord_interface: results.Add(manager.GetSourceDeclaration(Type.INTERFACE)!); break;
                    case LexicalType.KeyWord_delegate: results.Add(manager.GetSourceDeclaration(Type.DELEGATE)!); break;
                    case LexicalType.KeyWord_task: results.Add(manager.GetSourceDeclaration(Type.TASK)!); break;
                    case LexicalType.KeyWord_array: results.Add(manager.GetSourceDeclaration(Type.ARRAY)!); break;
                    default:
                        if (declaration != null)
                        {
                            var targetName = name[0].ToString();
                            if (declaration.declaration.category == DeclarationCategory.Struct)
                            {
                                var compiling = (CompilingStruct)declaration;
                                foreach (var item in compiling.variables)
                                    if (item.name == targetName)
                                        results.Add(item);
                                foreach (var item in compiling.functions)
                                    if (item.name == targetName)
                                        results.Add(item);
                            }
                            else if (declaration.declaration.category == DeclarationCategory.Class)
                            {
                                for (var index = (CompilingClass)declaration; index != null; index = manager.GetSourceDeclaration(index.parent) as CompilingClass)
                                {
                                    foreach (var item in index.variables)
                                        if (item.name == targetName && IsVisiable(manager, item.declaration))
                                            results.Add(item);
                                    foreach (var item in index.functions)
                                        if (item.name == targetName && IsVisiable(manager, item.declaration))
                                            results.Add(item);
                                }
                            }
                            if (results.Count > 0) return true;
                            for (var index = space; index != null; index = index.parent)
                                if (index.declarations.TryGetValue(targetName, out var declarations))
                                {
                                    results.AddRange(declarations);
                                    return true;
                                }
                            foreach (var rely in relies)
                                if (rely.declarations.TryGetValue(targetName, out var declarations))
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
                        if (!space!.children.TryGetValue(name[i].ToString(), out space))
                            return false;
                    if (space!.declarations.TryGetValue(name[^1].ToString(), out results!)) return true;
                    else results = [];
                }
            }
            return results.Count > 0;
        }
    }
}
