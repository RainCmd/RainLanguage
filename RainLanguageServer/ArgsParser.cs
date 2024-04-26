using System.Reflection;

namespace RainLanguageServer
{
    [AttributeUsage(AttributeTargets.Field)]
    internal class NecessaryAttribute : Attribute { }
    internal class ArgsParser
    {
        public string? logPath;
        public ArgsParser(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i].ToLower())
                {
                    case "-lp":
                    case "-logpath":
                        if (++i < args.Length) logPath = args[i]; break;
                }
            }
            foreach (var field in typeof(ArgsParser).GetFields())
                if (field.GetCustomAttribute<NecessaryAttribute>() != null && field.GetValue(this) == null)
                    throw new ArgumentNullException($"缺少 {field.Name} 参数");
        }
    }
}
