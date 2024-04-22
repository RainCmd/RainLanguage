namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public readonly HashSet<ISpace> relies = [];
        private void InitRelies(ASTManager manager)
        {
            foreach (var import in imports)
            {
                for (var index = compiling; index != null; index = index.parent)
                {
                    if (index.children.TryGetValue(import[0].ToString(), out var space))
                    {
                        for (var i = 1; i < import.Count; i++)
                            if (!space!.children.TryGetValue(import[i].ToString(), out space))
                            {
                                collector.Add(import[i], CErrorLevel.Error, "导入的命名空间未找到");
                                break;
                            }
                        if (space != null)
                        {
                            space.AddCite(this);
                            relies.Add(space);
                        }
                        goto lable_next_import;
                    }
                }
                if (import[0].ToString() == manager.library.name) collector.Add(import[0], CErrorLevel.Error, "不能导入自己");
                else
                {
                    ISpace? space = manager.LoadLibrary(import[0].ToString());
                    if (space == null) collector.Add(import[0], CErrorLevel.Error, "未找到命名空间或程序集");
                    else
                    {
                        for (var i = 0; i < import.Count; i++)
                            if (!space!.TryGetChild(import[i].ToString(), out space))
                            {
                                collector.Add(import[i], CErrorLevel.Error, "导入的命名空间未找到");
                                break;
                            }
                        if (space != null) relies.Add(space);
                    }
                }
            lable_next_import:;
            }
        }
        private List<CompilingDeclaration> GetDeclarations(TextRange name, bool isFunction)
        {
            if (compiling.declarations.TryGetValue(name.ToString(), out var declarations))
            {
                if (isFunction)
                {
                    var related = new List<TextRange>();
                    foreach (var declaration in declarations)
                        if (declaration.declaration.category != DeclarationCategory.Function && declaration.declaration.category != DeclarationCategory.Native)
                            related.Add(declaration.name);
                    if (related.Count > 0)
                    {
                        var message = new CompileMessage(name, CErrorLevel.Error, "命名冲突");
                        message.related.AddRange(related);
                        collector.Add(message);
                    }
                }
                else
                {
                    var message = new CompileMessage(name, CErrorLevel.Error, "命名冲突");
                    foreach (var declaration in declarations)
                        message.related.Add(declaration.name);
                    collector.Add(message);
                }
            }
            else
            {
                declarations = [];
                compiling.declarations.Add(name.ToString(), declarations);
            }
            return declarations;
        }
        public void Tidy(ASTManager manager)
        {
            InitRelies(manager);
            foreach (var child in children)
            {
                foreach (var rely in relies) child.relies.Add(rely);
                child.Tidy(manager);
            }
            foreach (var file in enums)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Enum, compiling.GetChildName(file.name.ToString()), default);
                var compilingEnum = new CompilingEnum(file.name, declaration, compiling, file);
                declarations.Add(compilingEnum);
                manager.library.enums.Add(compilingEnum);
                //var related = new List<TextRange>();
                //for (var x = 0; x < file.elements.Count; x++)
                //{
                //    var element = file.elements[x];
                //    for (var y = 0; y < x; y++)
                //        if (element.name.ToString() == file.elements[y].name.ToString())
                //            related.Add(file.elements[y].name);
                //    if (related.Count > 0)
                //    {
                //        var message = new CompileMessage(element.name, CErrorLevel.Error, "重复的枚举名");
                //        message.related.AddRange(related);
                //        collector.Add(message);
                //        related.Clear();
                //    }
                //    var name = new string[compilingEnum.declaration.name.Length + 1];
                //    Array.Copy(compilingEnum.declaration.name, name, compilingEnum.declaration.name.Length);
                //    name[^1] = element.name.ToString();
                //    declaration = new Declaration(Type.LIBRARY_SELF, Visibility.Public, DeclarationCategory.EnumElement, name, default);
                //    var compilingEnumElement = new CompilingEnum.Element(element.name, declaration, element.expression);
                //    compilingEnumElement.AddCite(element);
                //    compilingEnum.elements.Add(compilingEnumElement);
                //}
            }
            foreach (var file in structs)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Struct, compiling.GetChildName(file.name.ToString()), default);
                var compilingStruct = new CompilingStruct(file.name, declaration, compiling, file);
                declarations.Add(compilingStruct);
                manager.library.structs.Add(compilingStruct);
            }
            foreach (var file in interfaces)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Interface, compiling.GetChildName(file.name.ToString()), default);
                var compilingInterface = new CompilingInterface(file.name, declaration, compiling, file);
                declarations.Add(compilingInterface);
                manager.library.interfaces.Add(compilingInterface);
            }
            foreach (var file in classes)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Class, compiling.GetChildName(file.name.ToString()), default);
                var compilingClass = new CompilingClass(file.name, declaration, compiling, file, default, default, []);
                declarations.Add(compilingClass);
                manager.library.classes.Add(compilingClass);
            }
            foreach (var file in delegates)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Delegate, compiling.GetChildName(file.name.ToString()), default);
                var compilingDelegate = new CompilingDelegate(file.name, declaration, compiling, file, [], default);
                declarations.Add(compilingDelegate);
                manager.library.delegates.Add(compilingDelegate);
            }
            foreach (var file in tasks)
            {
                var declarations = GetDeclarations(file.name, false);
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Task, compiling.GetChildName(file.name.ToString()), default);
                var compilingTask = new CompilingTask(file.name, declaration, compiling, file, default);
                declarations.Add(compilingTask);
                manager.library.tasks.Add(compilingTask);
            }
        }
    }
}
