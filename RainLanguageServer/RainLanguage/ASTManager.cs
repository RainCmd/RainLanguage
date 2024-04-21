namespace RainLanguageServer.RainLanguage
{
    internal class ASTManager(string name)
    {
        public readonly CompilingLibrary library = new(name);
        public readonly Dictionary<string, FileSpace> fileSpaces = [];
        public ILibrary? LoadLibrary(string name)
        {
            //todo 加载程序集
            return null;
        }
        public IDeclaration GetDeclaration(Declaration declaration)
        {
            return null;
        }
    }
}
