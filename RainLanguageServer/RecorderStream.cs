namespace RainLanguageServer
{
    internal class RecorderStream(Stream stream, StreamWriter? recorder) : Stream
    {
        public override bool CanRead => stream.CanRead;
        public override bool CanSeek => stream.CanSeek;
        public override bool CanWrite => stream.CanWrite;
        public override long Length => stream.Length;
        public override long Position { get => stream.Position; set => stream.Position = value; }
        private void Recorder(bool rw, byte[] buffer, int offset, int count)
        {
            if (recorder == null) return;
            var message = recorder.Encoding.GetString(buffer, offset, count);
            if (message.StartsWith("Content-Length:"))
            {
                message = message[(message.IndexOf("\r\n\r\n") + 4)..];
                recorder.Write($"\r\n{DateTime.Now:[yy MM dd HH:mm:ss:fff]}{(rw ? ">> " : "<< ")} ");
            }
            recorder.Write(message);
        }
        public override void Flush()
        {
            stream.Flush();
            recorder?.Flush();
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            var result = stream.Read(buffer, offset, count);
            Recorder(true, buffer, offset, result);
            return result;
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            return stream.Seek(offset, origin);
        }

        public override void SetLength(long value)
        {
            stream.SetLength(value);
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            stream.Write(buffer, offset, count);
            Recorder(false, buffer, offset, count);
        }
    }
}
