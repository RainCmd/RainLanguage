using LanguageServer;
using Microsoft.Win32.SafeHandles;
using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ArrayCreateExpression : Expression
    {
        public readonly TypeExpression? typeExpression;
        public readonly Expression length;
        public ArrayCreateExpression(TextRange range, Expression length, Type type, TypeExpression? typeExpression) : base(range, new Tuple([type]))
        {
            this.length = length;
            this.typeExpression = typeExpression;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (typeExpression != null && typeExpression.typeWordRange.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine(types[0].ToString());
                sb.AppendLine("```");
                info = new HoverInfo(range, sb.ToString(), true);
                return true;
            }
            else if (length.range.Contain(position)) return length.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (typeExpression != null && typeExpression.typeWordRange.Contain(position))
            {
                var source = manager.GetSourceDeclaration(types[0].Source);
                if (source != null) source.OnHighlight(manager, infos);
                else infos.Add(new HighlightInfo(typeExpression.typeWordRange, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            else if (length.range.Contain(position)) return length.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (typeExpression != null && typeExpression.typeWordRange.Contain(position))
            {
                result = manager.GetSourceDeclaration(types[0].Source);
                return result != null;
            }
            else if (length.range.Contain(position)) return length.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            typeExpression?.CollectSemanticToken(collector);
            length.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            typeExpression?.Read(parameter);
            length.Read(parameter);
        }
    }
    internal class ArrayInitExpression : Expression
    {
        public readonly TextRange? typeRange;
        public readonly Expression elements;
        public ArrayInitExpression(TextRange range, TextRange? typeRange, Expression elements, Type type) : base(range, new Tuple([type]))
        {
            this.typeRange = typeRange;
            this.elements = elements;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (typeRange != null && typeRange.Value.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine(types[0].ToString());
                sb.AppendLine("```");
                info = new HoverInfo(range, sb.ToString(), true);
                return true;
            }
            else if (elements.range.Contain(position)) return elements.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (typeRange != null && typeRange.Value.Contain(position))
            {
                var source = manager.GetSourceDeclaration(types[0].Source);
                if (source != null) source.OnHighlight(manager, infos);
                else infos.Add(new HighlightInfo(typeRange.Value, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            else if (elements.range.Contain(position)) return elements.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (typeRange != null && typeRange.Value.Contain(position))
            {
                result = manager.GetSourceDeclaration(types[0].Source);
                return result != null;
            }
            else if (elements.range.Contain(position)) return elements.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (typeRange != null) collector.AddRange(types[0], typeRange.Value);
            elements.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            if (typeRange != null) parameter.manager.GetSourceDeclaration(types[0])?.references.Add(typeRange.Value);
            elements.Read(parameter);
        }
    }
    internal class ArrayEvaluationExpression : Expression
    {
        public readonly Expression array;
        public readonly Expression index;
        public ArrayEvaluationExpression(TextRange range, Expression array, Expression index, Type elementType, bool assignable) : base(range, new Tuple([elementType]))
        {
            this.array = array;
            this.index = index;
            attribute = ExpressionAttribute.Value | elementType.GetAttribute();
            if (assignable) attribute |= ExpressionAttribute.Assignable;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (array.range.Contain(position)) return array.OnHover(manager, position, out info);
            else if (index.range.Contain(position)) return index.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (array.range.Contain(position)) return array.OnHighlight(manager, position, infos);
            else if (index.range.Contain(position)) return index.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (array.range.Contain(position)) return array.TryGetDeclaration(manager, position, out result);
            else if (index.range.Contain(position)) return index.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            array.CollectSemanticToken(collector);
            index.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            array.Read(parameter);
            index.Read(parameter);
        }
        public override void Write(ExpressionParameter parameter)
        {
            array.Read(parameter);
            index.Read(parameter);
        }
    }
    internal class StringEvaluationExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression index;
        public StringEvaluationExpression(TextRange range, Expression source, Expression index) : base(range, new Tuple([Type.CHAR]))
        {
            this.source = source;
            this.index = index;
            attribute = ExpressionAttribute.Value;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (index.range.Contain(position)) return index.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (index.range.Contain(position)) return index.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (index.range.Contain(position)) return index.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            index.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            index.Read(parameter);
        }
    }
    internal class ArraySubExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression subRange;
        public ArraySubExpression(TextRange range, Expression source, Expression subRange) : base(range, source.types)
        {
            this.source = source;
            this.subRange = subRange;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (subRange.range.Contain(position)) return subRange.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (subRange.range.Contain(position)) return subRange.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (subRange.range.Contain(position)) return subRange.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            subRange.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            subRange.Read(parameter);
        }
    }
}
