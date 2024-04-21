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
                            if (!space!.TryGet(import[i].ToString(), out space))
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
        private List<Declaration> GetDeclarations(ASTManager manager, TextRange name, bool isFunction)
        {
            if (compiling.declarations.TryGetValue(name.ToString(), out var declarations))
            {
                if (isFunction)
                {
                    var related = new List<TextRange>();
                    foreach (var declaration in declarations)
                        if (manager.GetDeclaration(declaration) is CompilingDeclaration compilingDeclaration)
                            if (compilingDeclaration.declaration.category != DeclarationCategory.Function && compilingDeclaration.declaration.category != DeclarationCategory.Native)
                                related.Add(compilingDeclaration.name);
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
                        if (manager.GetDeclaration(declaration) is CompilingDeclaration compilingDeclaration)
                            message.related.Add(compilingDeclaration.name);
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
            foreach (var enumeration in enums)
            {
            }
        }
    }
}
