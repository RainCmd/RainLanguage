using System.Collections;

namespace RainLanguageServer.RainLanguage
{
    internal interface ICitePort<TSelf, TTarget> where TSelf : ICitePort<TSelf, TTarget> where TTarget : ICitePort<TTarget, TSelf>
    {
        CitePort<TTarget> Cites { get; }
    }
    internal class CitePort<T> : IEnumerable<T>
    {
        private readonly HashSet<T> values = [];
        internal int Count => values.Count;
        internal void Add(T value) => values.Add(value);
        internal void Remove(T value) => values.Remove(value);
        internal void Clear() => values.Clear();
        public IEnumerator<T> GetEnumerator()
        {
            return values.GetEnumerator();
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return values.GetEnumerator();
        }
    }
    internal static class CitePortExtend
    {
        public static void AddCite<TSelf, TTarget>(this ICitePort<TSelf, TTarget> port, TTarget value) where TSelf : ICitePort<TSelf, TTarget> where TTarget : ICitePort<TTarget, TSelf>
        {
            port.Cites.Add(value);
            value.Cites.Add((TSelf)port);
        }
        public static void RemoveCite<TSelf, TTarget>(this ICitePort<TSelf, TTarget> port, TTarget value) where TSelf : ICitePort<TSelf, TTarget> where TTarget : ICitePort<TTarget, TSelf>
        {
            port.Cites.Remove(value);
            value.Cites.Remove((TSelf)port);
        }
        public static void ClearCite<TSelf, TTarget>(this ICitePort<TSelf, TTarget> port) where TSelf : ICitePort<TSelf, TTarget> where TTarget : ICitePort<TTarget, TSelf>
        {
            foreach (var cite in port.Cites) cite.RemoveCite((TSelf)port);
            port.Cites.Clear();
        }
    }
}
