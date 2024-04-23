using System.Reflection;

namespace RainLanguageServer
{
    [AttributeUsage(AttributeTargets.Field)]
    internal class NecessaryAttribute : Attribute { }
    internal class ArgsParser
    {
        public readonly string? logPath;
        [Necessary]
        public readonly string? kernelDefinePath;
        public readonly string? projectRoot;
        public readonly string? projectName;
        public readonly string? filePath;
        public ArgsParser(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i].ToLower())
                {
                    case "-lp":
                    case "-logpath":
                        if (++i < args.Length) logPath = args[i]; break;
                    case "-kdp":
                    case "-kerneldefinepath":
                        if (++i < args.Length) kernelDefinePath = args[i]; break;
                    case "-pr":
                    case "-projectroot":
                        if (++i < args.Length) projectRoot = args[i]; break;
                    case "-pn":
                    case "-projectname":
                        if (++i < args.Length) projectName = args[i]; break;
                    case "-fp":
                    case "-filepath":
                        if (++i < args.Length) filePath = args[i]; break;
                }
            }
            foreach (var field in fields)
                if (field.GetValue(this) == null)
                    throw new ArgumentNullException($"缺少 {field.Name} 参数");
        }
        private readonly static List<FieldInfo> fields = [];
        static ArgsParser()
        {
            foreach (var field in typeof(ArgsParser).GetFields())
                if (field.GetCustomAttribute<NecessaryAttribute>() != null)
                    fields.Add(field);
        }
    }
}
