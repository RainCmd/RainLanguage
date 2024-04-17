using LanguageServer.Parameters.General;

namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class TextDocumentChangeRegistrationOptions : TextDocumentRegistrationOptions
    {
        public TextDocumentSyncKind? syncKind;
    }
}
