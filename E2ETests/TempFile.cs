using System;
using System.IO;

namespace E2ETests
{
    internal class TempFile : IDisposable
    {
        public readonly string FilePath;

        private TempFile(string filePath)
        {
            FilePath = filePath;
        }

        public void Dispose()
        {
            try
            {
                File.Delete(FilePath);
            }
            catch { }
        }

        public static TempFile Create(byte[] content)
        {
            var tempPath = Path.GetTempFileName();
            File.WriteAllBytes(tempPath, content);
            return new TempFile(tempPath);
        }
    }
}
