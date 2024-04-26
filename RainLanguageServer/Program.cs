using System.Diagnostics.CodeAnalysis;
using System.Net;
using System.Net.Sockets;

namespace RainLanguageServer
{
    internal class Program
    {
        [RequiresDynamicCode("Calls RainLanguageServer.Server.Server(Stream, Stream)")]
        static void Main(string[] args)
        {
            var parser = new ArgsParser(args);
#if DEBUG
            parser.logPath = "D:\\Projects\\CPP\\RainLanguage\\RainLanguagePlugin\\bin\\server.log";
            var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            socket.Bind(new IPEndPoint(IPAddress.Loopback, 14567));
            socket.Listen(1);
            var stream = new NetworkStream(socket.Accept());
            var input = stream;
            var output = stream;
#else
            var input = Console.OpenStandardInput();
            var output = Console.OpenStandardOutput();
#endif
            var recorder = string.IsNullOrEmpty(parser.logPath) ? null : File.CreateText(parser.logPath);
            var server = new Server(new RecorderStream(input, recorder), new RecorderStream(output, recorder));
            server.Listen().Wait();
            recorder?.Close();
        }
    }
}
