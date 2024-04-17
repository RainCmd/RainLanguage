namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class DocumentOnTypeFormattingRegistrationOptions : TextDocumentRegistrationOptions
    {
        public string? firstTriggerCharacter;

        public string[]? moreTriggerCharacter;
    }
}
