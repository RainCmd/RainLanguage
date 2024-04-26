using System.Diagnostics.CodeAnalysis;

namespace RainLanguageServer
{
    internal class Program
    {
        [RequiresDynamicCode("Calls RainLanguageServer.Server.Server(Stream, Stream)")]
        static void Main(string[] args)
        {
            //var parser = new ArgsParser(args);
            var parser = new ArgsParser("", "D:\\Projects\\CPP\\RainLanguage\\RainLanguagePlugin\\kernel.rain", "D:\\Projects\\CPP\\RainLanguage\\RainLanguagePlugin\\test", "TestLib", "");
            var recorder = string.IsNullOrEmpty(parser.logPath) ? null : File.CreateText(parser.logPath);
            var server = new Server(parser, new RecorderStream(Console.OpenStandardInput(), recorder), new RecorderStream(Console.OpenStandardOutput(), recorder));
            server.Listen().Wait();
            recorder?.Close();
        }
    }
}
