using LanguageServer.Parameters.TextDocument;

namespace RainLanguageServer
{
    internal readonly struct CompletionInfo(string lable, CompletionItemKind kind, string data)
    {
        public readonly string lable = lable;
        public readonly CompletionItemKind kind = kind;
        public readonly string data = data;
        public static void CollectAccessKeyword(List<CompletionInfo> infos)
        {
            infos.Add(new CompletionInfo("public", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("internal", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("space", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("protected", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("private", CompletionItemKind.Keyword, "关键字"));
        }
        public static void CollectStructKeyword(List<CompletionInfo> infos)
        {
            infos.Add(new CompletionInfo("namespace", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("import", CompletionItemKind.Keyword, "关键字"));
        }
        public static void CollectDefineKeyword(List<CompletionInfo> infos)
        {
            infos.Add(new CompletionInfo("native", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("enum", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("struct", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("class", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("interface", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("const", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("delegate", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("task", CompletionItemKind.Keyword, "关键字"));
        }
        public static void CollectStatementKeyword(List<CompletionInfo> infos)
        {
            infos.Add(new CompletionInfo("global", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("if", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("elseif", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("else", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("while", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("for", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("break", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("continue", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("return", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("wait", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("exit", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("try", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("catch", CompletionItemKind.Keyword, "关键字"));
            infos.Add(new CompletionInfo("finally", CompletionItemKind.Keyword, "关键字"));
        }
    }
}
