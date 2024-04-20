namespace RainLanguageServer.RainLanguage
{
    internal class ASTManager(string name)
    {
        public readonly CompilingLibrary library = new(name);
        public readonly Dictionary<string, FileSpace> fileSpaces = [];
    }
}
