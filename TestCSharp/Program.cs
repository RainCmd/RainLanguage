using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using RainLanguage;

namespace TestCSharp
{
    class CodeFile : BuildParameter.ICodeFile
    {
        public string Path { get; private set; }
        public string Content { get; private set; }
        public CodeFile(string path)
        {
            Path = path;
            using (var sr = File.OpenText(path))
            {
                Content = sr.ReadToEnd();
            }
        }
        public static IEnumerable<BuildParameter.ICodeFile> LoadFiles(string path)
        {
            if (File.Exists(path) && path.EndsWith(suffix))
                return new BuildParameter.ICodeFile[] { new CodeFile(path) };
            var result = new List<BuildParameter.ICodeFile>();
            if (Directory.Exists(path))
                LoadFiles(path, result);
            return result;
        }
        private static void LoadFiles(string dir, List<BuildParameter.ICodeFile> files)
        {
            foreach (var sub in Directory.GetDirectories(dir))
                LoadFiles(sub, files);
            foreach (var f in Directory.GetFiles(dir, "*" + suffix))
                files.Add(new CodeFile(f));
        }
        const string suffix = ".rain";
    }
    internal class Program
    {
        static void Main(string[] args)
        {
            var bp = new BuildParameter("test", true, CodeFile.LoadFiles(@"D:\Projects\Unity\RLDemo\Assets\Scripts\Logic\RainScripts"), OnLoadLibrary, RainErrorLevel.Error);
            var p = RainLanguageAdapter.BuildProduct(bp);
            for (RainErrorLevel el = 0; el < p.ErrorLevel; el++)
            {
                var ec = p.GetErrorCount(el);
                if (ec > 0)
                {
                    Console.WriteLine("ErrLvl:" + el);
                    for (int i = 0; i < ec; i++)
                    {
                        var emsg = p.GetErrorMessage(el, (uint)i);
                        var detail = emsg.Detail;
                        Console.WriteLine(detail.messageType);
                        Console.WriteLine($"{emsg.Path} line:{detail.line} [{detail.start}, {detail.start + detail.length}]\n");
                    }
                }
            }
            Console.ReadLine();
        }

        private static byte[] OnLoadLibrary(string name)
        {
            throw new NotImplementedException();
        }
    }
}

