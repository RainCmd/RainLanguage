namespace LanguageServer
{
    internal class SyncDictionary<TKey, TValue> where TKey : notnull
    {
        private readonly Dictionary<TKey, TValue> dict = [];

        public void Set(TKey key, TValue value)
        {
            lock(dict) dict[key] = value;
        }

        public bool TryRemove(TKey key, out TValue? value)
        {
            lock(dict)
            {
                if (dict.TryGetValue(key, out value))
                {
                    dict.Remove(key);
                    return true;
                }
            }
            return false;
        }

        public bool Remove(TKey key)
        {
            lock (dict) return dict.Remove(key);
        }
    }
}
