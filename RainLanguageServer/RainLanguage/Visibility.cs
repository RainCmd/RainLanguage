namespace RainLanguageServer.RainLanguage
{
    internal enum Visibility
    {
        None,
        Public = 0x1,
        Internal = 0x2,
        Space = 0x4,
        Protected = 0x8,
        Private = 0x10,
    }
    internal static class VisibilityExtend
    {
        public static bool ContainAny(this Visibility visibility, Visibility target)
        {
            return (visibility & target) != 0;
        }

        public static bool IsClash(this Visibility visibility, Visibility target)
        {
            if (visibility.ContainAny(Visibility.Public))
                return target.ContainAny((Visibility)0b11111);
            else if (visibility.ContainAny(Visibility.Internal))
                return target.ContainAny((Visibility)0b10111);
            else if (visibility.ContainAny(Visibility.Space))
                return target.ContainAny((Visibility)0b10011);
            else if (visibility.ContainAny(Visibility.Protected))
                return target.ContainAny((Visibility)0b11001);
            else if (visibility.ContainAny(Visibility.Private))
                return target.ContainAny((Visibility)0b11111);
            return false;
        }

        public static bool CanAccess(Visibility visibility, bool space, bool child)
        {
            if (space)
            {
                if (child && visibility.ContainAny(Visibility.Protected)) return true;
                else return !visibility.ContainAny(Visibility.Protected | Visibility.Private);
            }
            else
            {
                if (visibility.ContainAny(Visibility.Space)) return false;
                else if (child && visibility.ContainAny(Visibility.Protected)) return true;
                else return !visibility.ContainAny(Visibility.Protected | Visibility.Private);
            }
        }
    }
}
