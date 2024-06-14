namespace RainLanguageServer.RainLanguage
{
    internal partial class CompilingSpace
    {
        protected void DuplicationNameCheck(ASTManager manager)
        {
            foreach (var child in children)
            {
                child.Value.DuplicationNameCheck(manager);
                if (this.declarations.TryGetValue(child.Key, out var declarations))
                    foreach (var declaration in declarations)
                        declaration.file?.space.collector.Add(declaration.name, CErrorLevel.Error, "当前命名空间下有同名子命名空间");
            }
            var filter = new HashSet<Declaration>();
            var duplications = new List<CompilingDeclaration>();
            foreach (var declarations in declarations.Values)
                if (declarations.Count > 1)
                {
                    if (IsFunctions(declarations))
                    {
                        for (var x = 0; x < declarations.Count; x++)
                        {
                            var declarationX = declarations[x];
                            if (filter.Add(declarationX.declaration))
                            {
                                for (var y = x + 1; y < declarations.Count; y++)
                                {
                                    var delcarationY = declarations[y];
                                    if (declarationX.declaration.signature == delcarationY.declaration.signature)
                                    {
                                        duplications.Add(delcarationY);
                                        filter.Add(delcarationY.declaration);
                                    }
                                }
                                if (duplications.Count > 0)
                                {
                                    duplications.Add(declarationX);
                                    foreach (var declaration in duplications)
                                    {
                                        var msg = new CompileMessage(declaration.name, CErrorLevel.Error, "无效的重载");
                                        foreach (var item in duplications)
                                            if (item != declaration)
                                                msg.related.Add(new(item.name, "重载的函数"));
                                        declaration.file?.space.collector.Add(msg);
                                    }
                                    duplications.Clear();
                                }
                            }
                        }
                        filter.Clear();
                    }
                    else
                    {
                        foreach (var declaration in declarations)
                        {
                            var msg = new CompileMessage(declaration.name, CErrorLevel.Error, "名称重复");
                            foreach (var item in declarations)
                                if (item != declaration)
                                    msg.related.Add(new(item.name, "名称重复的定义"));
                            declaration.file?.space.collector.Add(msg);
                        }
                    }
                }
        }
        private static bool IsFunctions(List<CompilingDeclaration> declarations)
        {
            foreach (var declaration in declarations)
                if (declaration.declaration.category != DeclarationCategory.Function && declaration.declaration.category != DeclarationCategory.Native)
                    return false;
            return true;
        }
    }
    internal partial class CompilingLibrary
    {
        private static bool FindDeclaration(ASTManager manager, HashSet<CompilingStruct> structSet, Type define, Type type)
        {
            if (type.dimension > 0 || type.code != TypeCode.Struct) return false;
            else if (type.library == manager.library.name)
            {
                if (type == define) return true;
                else
                {
                    if (manager.GetSourceDeclaration(type) is CompilingStruct compiling && structSet.Add(compiling))
                        foreach (var variable in compiling.variables)
                            if (FindDeclaration(manager, structSet, define, variable.type))
                                return true;
                }
            }
            return false;
        }
        public void DeclarationValidityCheck(ASTManager manager)
        {
            DuplicationNameCheck(manager);
            var filter = new HashSet<Declaration>();
            var duplicationNames = new List<TextRange>();
            foreach (var compilingElement in enums)
            {
                for (var x = 0; x < compilingElement.elements.Count; x++)
                    if (filter.Add(compilingElement.elements[x].declaration))
                    {
                        for (var y = x + 1; y < compilingElement.elements.Count; y++)
                            if (compilingElement.elements[y].name.ToString() == compilingElement.elements[x].name.ToString())
                            {
                                duplicationNames.Add(compilingElement.elements[y].name);
                                filter.Add(compilingElement.elements[y].declaration);
                            }
                        if (duplicationNames.Count > 0)
                        {
                            duplicationNames.Add(compilingElement.elements[x].name);
                            foreach (var element in duplicationNames)
                            {
                                var msg = new CompileMessage(element, CErrorLevel.Error, "重复的枚举名");
                                foreach (var index in duplicationNames)
                                    if (index != element)
                                        msg.related.Add(new(index, "名称重复的枚举"));
                                compilingElement.file?.space.collector.Add(msg);
                            }
                            duplicationNames.Clear();
                        }
                    }
                filter.Clear();
            }
            var duplications = new List<CompilingDeclaration>();
            var structSet = new HashSet<CompilingStruct>();
            foreach (var compilingStruct in structs)
            {
                for (var x = 0; x < compilingStruct.variables.Count; x++)
                {
                    var variableX = compilingStruct.variables[x];
                    if (filter.Add(variableX.declaration))
                    {
                        for (var y = x + 1; y < compilingStruct.variables.Count; y++)
                        {
                            var variableY = compilingStruct.variables[y];
                            if (variableX.name.ToString() == variableY.name.ToString())
                            {
                                duplications.Add(variableY);
                                filter.Add(variableY.declaration);
                            }
                        }
                        foreach (var function in compilingStruct.functions)
                            if (variableX.name.ToString() == function.name.ToString())
                            {
                                duplications.Add(function);
                                filter.Add(function.declaration);
                            }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(variableX);
                            foreach (var index in duplications)
                            {
                                var msg = new CompileMessage(index.name, CErrorLevel.Error, "名称重复");
                                foreach (var item in duplications)
                                    if (item != index)
                                        msg.related.Add(new(item.name, "名称重复的成员"));
                                index.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                        if (variableX.file is FileVariable file && FindDeclaration(manager, structSet, compilingStruct.declaration.GetDefineType(), variableX.type))
                            compilingStruct.file?.space.collector.Add(file.type.GetNameRange(), CErrorLevel.Error, "结构体循环包含");
                    }
                }
                filter.Clear();
                for (var x = 0; x < compilingStruct.functions.Count; x++)
                {
                    var functionX = compilingStruct.functions[x];
                    if (functionX.name.ToString() == compilingStruct.name.ToString())
                        functionX.file?.space.collector.Add(functionX.name, CErrorLevel.Error, "结构体不允许有构造函数");
                    else if (filter.Add(functionX.declaration))
                    {
                        for (var y = x + 1; y < compilingStruct.functions.Count; y++)
                        {
                            var functionY = compilingStruct.functions[y];
                            if (functionX.name.ToString() == functionY.name.ToString() && functionX.declaration.signature == functionY.declaration.signature)
                            {
                                duplications.Add(functionX);
                                filter.Add(functionY.declaration);
                            }
                        }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(functionX);
                            foreach (var function in duplications)
                            {
                                var msg = new CompileMessage(function.name, CErrorLevel.Error, "重复的定义");
                                foreach (var index in duplications)
                                    if (index != function)
                                        msg.related.Add(new(index.name, "重复定义的函数"));
                                function.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                    }
                }
                filter.Clear();
            }
            var typeStack = new List<Type>();
            foreach (var compilingInterface in interfaces)
            {
                typeStack.Add(compilingInterface.declaration.GetDefineType());
                if (CheckCircularInheritance(manager, compilingInterface.inherits, typeStack))
                    compilingInterface.file?.space.collector.Add(compilingInterface.name, CErrorLevel.Error, "存在循环继承");
                typeStack.Clear();
                for (var x = 0; x < compilingInterface.callables.Count; x++)
                {
                    var callableX = compilingInterface.callables[x];
                    if (filter.Add(callableX.declaration))
                    {
                        for (var y = x + 1; y < compilingInterface.callables.Count; y++)
                        {
                            var callableY = compilingInterface.callables[y];
                            if (callableX.name.ToString() == callableY.name.ToString() && callableX.declaration.signature == callableY.declaration.signature)
                            {
                                duplications.Add(callableY);
                                filter.Add(callableY.declaration);
                            }
                        }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(callableX);
                            foreach (var callable in duplications)
                            {
                                var msg = new CompileMessage(callable.name, CErrorLevel.Error, "重复的定义");
                                foreach (var index in duplications)
                                    if (index != callable)
                                        msg.related.Add(new(index.name, "重复定义的接口函数"));
                                callable.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                    }
                }
                filter.Clear();
            }
            foreach (var compilingClass in classes)
            {
                typeStack.Add(compilingClass.declaration.GetDefineType());
                foreach (var index in manager.GetInheritIterator(compilingClass.parent))
                    if (typeStack.Contains(index))
                    {
                        compilingClass.file?.space.collector.Add(compilingClass.name, CErrorLevel.Error, "存在循环继承");
                        break;
                    }
                    else typeStack.Add(index);
                typeStack.Clear();
                for (var x = 0; x < compilingClass.constructors.Count; x++)
                {
                    var ctorX = compilingClass.constructors[x];
                    if (ctorX.returns.Count > 0) ctorX.file?.space.collector.Add(ctorX.name, CErrorLevel.Error, "构造函数不能有返回值");
                    if (filter.Add(ctorX.declaration))
                    {
                        for (var y = x + 1; y < compilingClass.constructors.Count; y++)
                        {
                            var ctorY = compilingClass.constructors[y];
                            if (ctorX.declaration.signature == ctorY.declaration.signature)
                            {
                                duplications.Add(ctorY);
                                filter.Add(ctorY.declaration);
                            }
                        }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(ctorX);
                            foreach (var index in duplications)
                            {
                                var msg = new CompileMessage(index.name, CErrorLevel.Error, "重复的定义");
                                foreach (var item in duplications)
                                    if (item != index)
                                        msg.related.Add(new(item.name, "重复定义的构造函数"));
                                index.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                    }
                }
                filter.Clear();
                for (var x = 0; x < compilingClass.variables.Count; x++)
                {
                    var variableX = compilingClass.variables[x];
                    if (filter.Add(variableX.declaration))
                    {
                        for (var y = x + 1; y < compilingClass.variables.Count; y++)
                        {
                            var variableY = compilingClass.variables[y];
                            if (variableX.name.ToString() == variableY.name.ToString())
                            {
                                duplications.Add(variableY);
                                filter.Add(variableY.declaration);
                            }
                        }
                        foreach (var function in compilingClass.functions)
                            if (variableX.name.ToString() == function.name.ToString())
                            {
                                duplications.Add(function);
                                filter.Add(function.declaration);
                            }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(variableX);
                            foreach (var index in duplications)
                            {
                                var msg = new CompileMessage(index.name, CErrorLevel.Error, "命名冲突");
                                foreach (var item in duplications)
                                    if (index != item)
                                        msg.related.Add(new(item.name, "命名冲突的成员"));
                                index.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                    }
                }
                filter.Clear();
                for (var x = 0; x < compilingClass.functions.Count; x++)
                {
                    var functionX = compilingClass.functions[x];
                    if (filter.Add(functionX.declaration))
                    {
                        for (var y = x + 1; y < compilingClass.functions.Count; y++)
                        {
                            var functionY = compilingClass.functions[y];
                            if (functionX.name.ToString() == functionY.name.ToString() && functionX.declaration.signature == functionY.declaration.signature)
                            {
                                duplications.Add(functionY);
                                filter.Add(functionY.declaration);
                            }
                        }
                        if (duplications.Count > 0)
                        {
                            duplications.Add(functionX);
                            foreach (var index in duplications)
                            {
                                var msg = new CompileMessage(index.name, CErrorLevel.Error, "重复的定义");
                                foreach (var item in duplications)
                                    if (index != item)
                                        msg.related.Add(new(item.name, "重复定义的函数"));
                                index.file?.space.collector.Add(msg);
                            }
                            duplications.Clear();
                        }
                    }
                }
                filter.Clear();
            }
        }
        private static bool CheckCircularInheritance(ASTManager manager, List<Type> inherits, List<Type> stack)
        {
            foreach (var type in inherits)
            {
                if (stack.Contains(type)) return true;
                stack.Add(type);
                if (manager.GetSourceDeclaration(type) is CompilingInterface compiling && CheckCircularInheritance(manager, compiling.inherits, stack))
                    return true;
                stack.RemoveAt(stack.Count - 1);
            }
            return false;
        }
    }
}
