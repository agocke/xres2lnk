using System;
using System.IO;
using System.Reflection;
using System.Text;

namespace E2ETests
{
    internal static class TestResources
    {
        private static Stream GetResourceStream(string name)
        {
            var assembly = typeof(TestResources).GetTypeInfo().Assembly;

            var stream = assembly.GetManifestResourceStream(name);
            if (stream == null)
            {
                throw new InvalidOperationException($"Resource '{name}' not found in {assembly.FullName}.");
            }

            return stream;
        }

        private static byte[] GetResourceBlob(string name)
        {
            using (var stream = GetResourceStream(name))
            {
                var bytes = new byte[stream.Length];
                using (var memoryStream = new MemoryStream(bytes))
                {
                    stream.CopyTo(memoryStream);
                }

                return bytes;
            }
        }

        private static byte[] GetOrCreateResource(ref byte[] resource, string name)
            => resource ?? (resource = GetResourceBlob(name));

        private static byte[] _sampleXresourceFile;
        public static byte[] SampleXresourcesFile => GetOrCreateResource(ref _sampleXresourceFile, "E2ETests.Resources.XResources");

        private static byte[] _blankLnkFile;
        public static byte[] BlankLnkFile => GetOrCreateResource(ref _blankLnkFile, "E2ETests.Resources.bash.exe.lnk");
    }
}
