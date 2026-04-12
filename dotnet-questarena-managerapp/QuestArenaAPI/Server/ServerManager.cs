using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace QuestArenaAPI.Server
{
    public class ServerAlreadyExistsException : Exception { };
    public class ServerDoesntExistException : Exception { };

    public delegate void dOnServerClosed(int ResultCode);

    public class ServerManager
    {
        Process serverProcess;
        ServerConnection serverConnection;
        ServerController serverController;

        public dOnServerClosed OnServerClosed;

        string serverPathOnDisk = "D:/QuestBuildCemetery/mark_ready/WindowsNoEditor/QuestArena/Binaries/Win64/QuestArenaServer.exe";
        string serverArgs = "-log -port={0}";
        Int16 targetPort = 6969;

        public bool IsRunning => (serverProcess != null);

        public ServerController StartServerProcess()
        {
            if (serverProcess != null) throw new ServerAlreadyExistsException();

            serverConnection = new ServerConnection(6970, 6971, "localhost");
            serverController = new ServerController(serverConnection); //will launch serverConnection for two-side communication

            serverProcess = new Process();
            serverProcess.StartInfo = new ProcessStartInfo(serverPathOnDisk, string.Format(serverArgs, targetPort));
            serverProcess.EnableRaisingEvents = true;
            serverProcess.Exited += Handle_OnServerClosed;
            serverProcess.Start();

            return serverController;
        }

        public void KillServerProcess()
        {
            if (serverProcess == null) throw new ServerDoesntExistException();
            serverProcess.Kill();
        }

        private void Handle_OnServerClosed(object sender, EventArgs e)
        {
            serverController.Shutdown();
            serverController = null;
            OnServerClosed(serverProcess.ExitCode);

            //clear reference at the end of handler method
            serverProcess = null;
        }
    }
}
