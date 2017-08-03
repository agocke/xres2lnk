using System;
using System.Diagnostics;
using System.IO;
using System.Security.Cryptography;
using System.Threading.Tasks;
using Xunit;

namespace E2ETests
{
    public class CommandLineTests
    {
        [Fact]
        public async Task SimpleTest()
        {
            using (var xresFile = TempFile.Create(TestResources.SampleXresourcesFile))
            using (var lnkFile = TempFile.Create(TestResources.BlankLnkFile))
            {
                var exit = await RunXres2Lnk($"{xresFile.FilePath} {lnkFile.FilePath}");
                Assert.Equal(0, exit);
                var content = File.ReadAllBytes(lnkFile.FilePath);
                var sha = BitConverter.ToString(SHA256.Create().ComputeHash(content));
                Assert.Equal("CE6D400FDEFB95455FA91902A4C524543F2ABA1DB85B78FAE2105888686EF8B2",
                    sha.Replace("-", ""));
            }
        }

        private static Task<int> RunXres2Lnk(string args)
        {
            s_xresInfo.Arguments = args;
            var tcs = new TaskCompletionSource<int>();
            var proc = new Process();
            var task = tcs.Task;
            proc.EnableRaisingEvents = true;
            proc.Exited += delegate { tcs.TrySetResult(proc.ExitCode); };
            proc.StartInfo = s_xresInfo;
            proc.Start();
            return task;
        }

        private static ProcessStartInfo s_xresInfo = new ProcessStartInfo
        {
            FileName = Path.Combine(
                Path.GetDirectoryName(typeof(CommandLineTests).Assembly.CodeBase),
                "xres2lnk.exe")
        };
    }
}
