using System.Collections;
using System.Text.RegularExpressions;

namespace RainLanguageServer.RainLanguage
{
    internal interface IGroupMember<T> where T : class, IGroupMember<T>
    {
        Groups<T> Groups { get; }
    }
    internal class Group<T> : IEnumerable<T> where T : class, IGroupMember<T>
    {
        private readonly HashSet<T> members = [];
        private Group() { }
        internal void Add(T? member)
        {
            if (member != null && members.Add(member)) member.Groups.JoinGroup(this);
        }
        internal void Remove(T member, bool delay = false)
        {
            if (members.Remove(member))
            {
                if (!delay) member.Groups.LeaveGroup(this);
                if (members.Count == 1) members.First().Groups.LeaveGroup(this);
            }
        }
        public IEnumerator<T> GetEnumerator()
        {
            return members.GetEnumerator();
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
        public static void CreateGroup(IEnumerable<T?>? members)
        {
            if (members != null && members.Count() > 1)
            {
                var group = new Group<T>();
                foreach (var member in members)
                    group.Add(member);
            }
        }
    }
    internal class Groups<T> : IEnumerable<Group<T>> where T : class, IGroupMember<T>
    {
        private HashSet<Group<T>> groups = [];
        internal void JoinGroup(Group<T> group) => groups.Add(group);
        internal void LeaveGroup(Group<T> group) => groups.Remove(group);
        internal void Clear() => groups.Clear();
        public IEnumerator<Group<T>> GetEnumerator()
        {
            return groups.GetEnumerator();
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
    internal static class GroupExtend
    {
        public static void JoinGroup<T>(this IGroupMember<T> member, Group<T> group) where T : class, IGroupMember<T>
        {
            group.Add((T)member);
        }
        public static void LeaveGroup<T>(this IGroupMember<T> member, Group<T> group) where T : class, IGroupMember<T>
        {
            group.Remove((T)member);
        }
        public static void ClearGroup<T>(this IGroupMember<T> member) where T : class, IGroupMember<T>
        {
            foreach (var group in member.Groups)
                group.Remove((T)member, true);
            member.Groups.Clear();
        }
    }
}
