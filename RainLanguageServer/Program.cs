using System.Diagnostics.CodeAnalysis;

namespace RainLanguageServer
{
    internal class Program
    {
        [RequiresDynamicCode("Calls RainLanguageServer.Server.Server(Stream, Stream)")]
        static void Main(string[] args)
        {
            var parser = new ArgsParser(args);
            var recorder = parser.logPath == null ? null : File.CreateText(parser.logPath);
            var server = new Server(parser, new RecorderStream(Console.OpenStandardInput(), recorder), new RecorderStream(Console.OpenStandardOutput(), recorder));
            server.Listen().Wait();
            recorder?.Close();
        }
    }
}
