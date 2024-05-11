namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public readonly HashSet<CompilingSpace> relies = [];
        private void InitRelies(ASTManager manager, CompilingLibrary library)
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
                        if (space != null && !relies.Add(space)) collector.Add(import, CErrorLevel.Info, "重复导入的命名空间");
                        goto lable_next_import;
                    }
                }
                if (import[0].ToString() == library.name) collector.Add(import[0], CErrorLevel.Error, "不能导入自己");
                else
                {
                    CompilingSpace? space = manager.LoadLibrary(import[0].ToString());
                    if (space == null) collector.Add(import[0], CErrorLevel.Error, "未找到命名空间或程序集");
                    else
                    {
                        for (var i = 1; i < import.Count; i++)
                            if (!space!.children.TryGetValue(import[i].ToString(), out space))
                            {
                                collector.Add(import[i], CErrorLevel.Error, "导入的命名空间未找到");
                                break;
                            }
                        if (space != null && !relies.Add(space)) collector.Add(import, CErrorLevel.Info, "重复导入的命名空间");
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
        public void Tidy(ASTManager manager, CompilingLibrary library, bool cite)
        {
            InitRelies(manager, library);
            foreach (var child in children)
            {
                foreach (var rely in relies) child.relies.Add(rely);
                child.Tidy(manager, library, cite);
            }
            foreach (var file in enums)
            {
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Enum, compiling.GetChildName(file.name.ToString()), default);
                var compilingEnum = new CompilingEnum(file.name, declaration, file.attributes, compiling, relies, cite ? file : null);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingEnum);
                library.enums.Add(compilingEnum);
                file.compiling = compilingEnum;
            }
            foreach (var file in structs)
            {
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Struct, compiling.GetChildName(file.name.ToString()), default);
                var compilingStruct = new CompilingStruct(file.name, declaration, file.attributes, compiling, cite ? file : null);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingStruct);
                library.structs.Add(compilingStruct);
                file.compiling = compilingStruct;
            }
            foreach (var file in interfaces)
            {
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Interface, compiling.GetChildName(file.name.ToString()), default);
                var compilingInterface = new CompilingInterface(file.name, declaration, file.attributes, compiling, cite ? file : null);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingInterface);
                library.interfaces.Add(compilingInterface);
                file.compiling = compilingInterface;
            }
            foreach (var file in classes)
            {
                if (file.constructors.Count == 0) file.constructors.Add(new FileFunction(file.name, Visibility.Public, file.space, [], [], []));
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Class, compiling.GetChildName(file.name.ToString()), default);
                var compilingClass = new CompilingClass(file.name, declaration, file.attributes, compiling, cite ? file : null, default);
                compilingClass.destructor = new GrammaticalAnalysis.LogicBlock(compilingClass, file.destructor, relies, collector);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingClass);
                library.classes.Add(compilingClass);
                file.compiling = compilingClass;
            }
            foreach (var file in delegates)
            {
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Delegate, compiling.GetChildName(file.name.ToString()), default);
                var compilingDelegate = new CompilingDelegate(file.name, declaration, file.attributes, compiling, cite ? file : null, [], default);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingDelegate);
                library.delegates.Add(compilingDelegate);
                file.compiling = compilingDelegate;
            }
            foreach (var file in tasks)
            {
                var declaration = new Declaration(library.name, file.visibility, DeclarationCategory.Task, compiling.GetChildName(file.name.ToString()), default);
                var compilingTask = new CompilingTask(file.name, declaration, file.attributes, compiling, cite ? file : null, default);
                AddCompilingTypeDeclaration(file.name.ToString(), compilingTask);
                library.tasks.Add(compilingTask);
                file.compiling = compilingTask;
            }
        }
    }
}
