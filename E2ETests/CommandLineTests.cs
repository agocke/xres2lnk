using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
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
                var result = await RunXres2Lnk(xresFile.FilePath, lnkFile.FilePath);
                Assert.Equal(0, result.ExitCode);
                var content = File.ReadAllBytes(lnkFile.FilePath);
                var sha = BitConverter.ToString(SHA256.Create().ComputeHash(content));
                Assert.Equal("CE6D400FDEFB95455FA91902A4C524543F2ABA1DB85B78FAE2105888686EF8B2",
                    sha.Replace("-", ""));
            }
        }

        [Fact]
        public async Task GoodXresErrorMessage()
        {
            // Random paths that won't exist
            using (var lnkFile = TempFile.Create(TestResources.SampleXresourcesFile))
            {
                var xresPath = @"C:\fsdjknbgdf";
                var result = await RunXres2Lnk(xresPath, lnkFile.FilePath);
                Assert.NotEqual(0, result.ExitCode);
                // Error message should contain xres path
                Assert.Contains(xresPath, result.ErrOutput);
            }
        }

        private static Task<(int ExitCode, string Output, string ErrOutput)>
            RunXres2Lnk(string xresPath, string lnkPath)
        {
            s_xresInfo.Arguments = $"{xresPath} {lnkPath}";
            var tcs = new TaskCompletionSource<(int, string, string)>();
            var proc = new Process();
            var output = new StringBuilder();
            var errOutput = new StringBuilder();

            proc.EnableRaisingEvents = true;
            proc.OutputDataReceived += (s, e) =>
            {
                if (e.Data != null)
                {
                    output.AppendLine(e.Data);
                }
            };

            proc.ErrorDataReceived += (s, e) =>
            {
                if (e.Data != null)
                {
                    errOutput.AppendLine(e.Data);
                }
            };

            proc.Exited += delegate
            {
                Task.Run(() =>
                {
                    proc.WaitForExit();
                    var result = (proc.ExitCode,
                         output.ToString(),
                         errOutput.ToString());
                    tcs.TrySetResult(result);
                });
            };

            proc.StartInfo = s_xresInfo;
            proc.Start();
            proc.BeginOutputReadLine();
            proc.BeginErrorReadLine();
            return tcs.Task;
        }

        private static ProcessStartInfo s_xresInfo = new ProcessStartInfo
        {
            FileName = GetExePath(),
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false
        };

        private static string GetExePath()
        {
            var asmPath = new Uri(Assembly.GetExecutingAssembly().CodeBase).LocalPath;
            return Path.Combine(Path.GetDirectoryName(asmPath), "xres2lnk.exe");
        }
    }
}
