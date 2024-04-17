namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class DocumentLinkRegistrationOptions : TextDocumentRegistrationOptions
    {
        public bool? resolveProvider;
    }
}
