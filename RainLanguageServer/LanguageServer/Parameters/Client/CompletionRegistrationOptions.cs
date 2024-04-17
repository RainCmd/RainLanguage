namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class CompletionRegistrationOptions : TextDocumentRegistrationOptions
    {
        public string[]? triggerCharacters;

        public bool? resolveProvider;
    }
}
