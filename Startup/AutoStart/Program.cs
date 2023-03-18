using System.Diagnostics;
using System.IO;

namespace AutoStart
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var processInfo = new ProcessStartInfo(Path.Combine("C:\\Program Files\\Microsoft.Tray", "Startup.exe"))
            {
                CreateNoWindow = true,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
            };
            Process.Start(processInfo);
        }
    }
}
