using LanguageServer.Parameters;
using LanguageServer;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BlockStatement : Statement
    {
        public int indent = -1;
        public readonly List<Statement> statements = [];
        public readonly List<TextRange>? group;
        public BlockStatement(TextRange anchor, List<TextRange>? group) : base(anchor)
        {
            this.group = group;
            if (anchor.start.document != null)
                group?.Add(anchor);
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var statement in statements)
                statement.Read(parameter);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            foreach (var statement in statements)
                if (statement.range.Contain(position))
                    return statement.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (group != null && anchor.Contain(position))
            {
                foreach (var anchor in group)
                    infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            foreach (var statement in statements)
                if (statement.range.Contain(position))
                    return statement.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            foreach (var statement in statements)
                if (statement.range.Contain(position))
                    return statement.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach (var statement in statements)
                statement.CollectSemanticToken(collector);
        }
    }
}
