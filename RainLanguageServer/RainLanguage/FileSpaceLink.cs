namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public void Link(ASTManager manager)
        {
            foreach (var child in children) child.Link(manager);
            var context = new Context(compiling, relies, default);
            foreach (var variable in variables)
            {
                if (context.TryFindDeclaration(manager, variable.type.name, out var declarations, collector))
                {

                }
            }
        }
    }
}
