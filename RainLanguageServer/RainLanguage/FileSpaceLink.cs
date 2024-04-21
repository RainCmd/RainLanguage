namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public void Link(ASTManager manager)
        {
            foreach(var child in children) child.Link(manager);

        }
    }
}
