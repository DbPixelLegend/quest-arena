using System;
using System.Collections.Generic;
using System.Text;

namespace QuestArenaAPI.Server
{
    public class ServerController
    {
        private ServerConnection serverConnection;

        bool bServerCanReceiveOrders = false;

        public ServerController(ServerConnection inConnection)
        {
            serverConnection = inConnection;
            serverConnection.StartTcpServer(RequestProcessor);
        }

        internal void Shutdown()
        {
            serverConnection.StopTcpServer();
            serverConnection = null;
        }

        internal bool RequestProcessor(string ReceivedInMessage, out string CalculatedOutMessage)
        {
            throw new NotImplementedException();
        }
    }
}
