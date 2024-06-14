using System.Collections;

namespace RainLanguageServer.RainLanguage
{
    internal struct InheritIterator(ASTManager manager, Type type) : IEnumerable<Type>
    {
        private readonly ASTManager manager = manager;
        private Type type = type;

        public IEnumerator<Type> GetEnumerator()
        {
            if (!type.Vaild) yield break;
            var index = type;
            yield return index;
            for (index = manager.GetParent(index); index.Vaild; index = manager.GetParent(index))
                if (index == type) break;
                else yield return index;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
