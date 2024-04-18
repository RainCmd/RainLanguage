using LanguageServer;
using System.Diagnostics.CodeAnalysis;

namespace RainLanguageServer
{
    internal class Program
    {
        [RequiresDynamicCode("Calls RainLanguageServer.Server.Server(Stream, Stream)")]
        static void Main(string[] args)
        {
            var recorder = args?.Length > 0 ? File.CreateText(args[0] + "server.log") : null;
            var server = new Server(new RecorderStream(Console.OpenStandardInput(), recorder), new RecorderStream(Console.OpenStandardOutput(), recorder));
            server.Listen().Wait();
            recorder?.Close();
        }
    }
}
