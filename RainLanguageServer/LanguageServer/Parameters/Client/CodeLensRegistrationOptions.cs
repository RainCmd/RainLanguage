namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class CodeLensRegistrationOptions : TextDocumentRegistrationOptions
    {
        public bool? resolveProvider;
    }
}
