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
                            if (!relies.Add(space)) collector.Add(import, CErrorLevel.Info, "重复导入的命名空间");
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
                        if (space != null&&!relies.Add(space)) collector.Add(import, CErrorLevel.Info, "重复导入的命名空间");
                    }
                }
            lable_next_import:;
            }
        }
        private void AddCompilingTypeDeclaration(string name, CompilingDeclaration compilingDeclaration)
        {
            if (!compiling.declarations.TryGetValue(name.ToString(), out var declarations))
            {
                declarations = [];
                compiling.declarations.Add(name.ToString(), declarations);
            }
            declarations.Add(compilingDeclaration);
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
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Enum, compiling.GetChildName(file.name.ToString()), default);
                var compilingEnum = new CompilingEnum(file.name, declaration, compiling, file);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingEnum);
                manager.library.enums.Add(compilingEnum);
            }
            foreach (var file in structs)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Struct, compiling.GetChildName(file.name.ToString()), default);
                var compilingStruct = new CompilingStruct(file.name, declaration, compiling, file);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingStruct);
                manager.library.structs.Add(compilingStruct);
            }
            foreach (var file in interfaces)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Interface, compiling.GetChildName(file.name.ToString()), default);
                var compilingInterface = new CompilingInterface(file.name, declaration, compiling, file);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingInterface);
                manager.library.interfaces.Add(compilingInterface);
            }
            foreach (var file in classes)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Class, compiling.GetChildName(file.name.ToString()), default);
                var compilingClass = new CompilingClass(file.name, declaration, compiling, file, default, file.destructor, relies);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingClass);
                manager.library.classes.Add(compilingClass);
            }
            foreach (var file in delegates)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Delegate, compiling.GetChildName(file.name.ToString()), default);
                var compilingDelegate = new CompilingDelegate(file.name, declaration, compiling, file, [], default);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingDelegate);
                manager.library.delegates.Add(compilingDelegate);
            }
            foreach (var file in tasks)
            {
                var declaration = new Declaration(Type.LIBRARY_SELF, file.visibility, DeclarationCategory.Task, compiling.GetChildName(file.name.ToString()), default);
                var compilingTask = new CompilingTask(file.name, declaration, compiling, file, default);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingTask);
                manager.library.tasks.Add(compilingTask);
            }
        }
    }
}
