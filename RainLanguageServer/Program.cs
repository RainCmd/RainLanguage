using LanguageServer;

namespace RainLanguageServer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var recorder = args?.Length > 0 ? File.CreateText(args[0] + "server.log") : null;
            var server = new Server(new RecorderStream(Console.OpenStandardInput(), recorder), new RecorderStream(Console.OpenStandardOutput(), recorder));
            Service.Register(server, server.GetType());
            server.CancellationToken.WaitHandle.WaitOne();
            recorder?.Close();
        }
    }
}
