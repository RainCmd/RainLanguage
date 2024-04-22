namespace RainLanguageServer
{
    internal class ArgsParser
    {
        public string? logPath;
        public string? kernelDefinePath;
        public ArgsParser(string[] args)
        {
            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i].ToLower())
                {
                    case "-lp":
                    case "-logpath":
                        if (++i < args.Length) logPath = args[i];
                        break;
                    case "-kdp":
                    case "-kerneldefinepath":
                        if (++i < args.Length) kernelDefinePath = args[i];
                        break;
                }
            }
        }
    }
}
