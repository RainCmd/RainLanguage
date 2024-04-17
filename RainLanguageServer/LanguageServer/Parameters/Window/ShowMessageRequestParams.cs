namespace LanguageServer.Parameters.Window
{
    public class ShowMessageRequestParams
    {
        public MessageType? type;
        public string? message;
        public MessageActionItem[]? actions;
    }
}
