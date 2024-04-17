namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// For <c>textDocument/didChange</c>
    /// </summary>
    public class DidChangeTextDocumentParams
    {
        public VersionedTextDocumentIdentifier? textDocument;

        public TextDocumentContentChangeEvent[]? contentChanges;
    }
}
