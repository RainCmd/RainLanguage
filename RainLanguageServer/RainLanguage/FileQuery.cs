namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public IEnumerable<FileDeclaration> SelfDeclarations
        {
            get
            {
                foreach (var file in variables) yield return file;
                foreach (var file in functions) yield return file;
                foreach (var file in enums) yield return file;
                foreach (var file in structs) yield return file;
                foreach (var file in interfaces) yield return file;
                foreach (var file in classes) yield return file;
                foreach (var file in delegates) yield return file;
                foreach (var file in tasks) yield return file;
                foreach (var file in natives) yield return file;
            }
        }
        public IEnumerable<FileDeclaration> Declarations
        {
            get
            {
                foreach (var file in SelfDeclarations) yield return file;
                foreach (var child in children)
                    foreach (var file in child.Declarations)
                        yield return file;
            }
        }
        public IEnumerable<FileSpace> Spaces
        {
            get
            {
                yield return this;
                foreach (var child in children)
                    foreach (var space in child.Spaces)
                        yield return space;
            }
        }

        public IEnumerable<CompileMessage> Messages
        {
            get
            {
                foreach (var msg in collector) yield return msg;
                foreach (var child in children)
                    foreach (var msg in child.Messages)
                        yield return msg;
            }
        }

        public bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            foreach (var child in children)
                if (child.range.Contain(position))
                    return child.TryGetDeclaration(manager, position, out result);
            foreach (var declaration in SelfDeclarations)
                if (declaration.range != null && declaration.range.Contain(position))
                    return declaration.TryGetDeclaration(manager, position, out result);
            result = null;
            return false;
        }

        public void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach (var child in children)
                child.CollectSemanticToken(collector);
            if (name != null)
                foreach (var range in name)
                    collector.AddRange(SemanticTokenType.Namespace, range);
            foreach (var import in imports)
                foreach (var range in import)
                    collector.AddRange(SemanticTokenType.Namespace, range);
            foreach (var declaration in SelfDeclarations)
                declaration.CollectSemanticToken(collector);
        }

        public FileSpace GetFileSpace(TextPosition position)
        {
            foreach (var child in children)
                if (child.range.Contain(position))
                    return child.GetFileSpace(position);
            return this;
        }
        public FileDeclaration? GetFileDeclaration(TextPosition position)
        {
            var space = GetFileSpace(position);
            foreach (var file in space.SelfDeclarations)
                if (file.range.Contain(position))
                    return file;
            return null;
        }
    }
}
