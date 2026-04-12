using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace QuestArenaAPI.Server
{
    public delegate bool dProcessRequest(string ReceivedInMessage, out string CalculatedOutMessage);

    public class ServerConnection
    {
        //-----------------CONST VALUES
        public const string internalErrorMessage = "__INTERNALERROR__";
        public const int messageBufferLen = 256;

        //-----------------INTERNAL VARIABLES
        private Task connectionTask;
        private bool bEndTaskRequested = false;
        private dProcessRequest ProcessRequest;

        //-----------------CONFIG PROPERTIES
        Int16 managerListeningPort = 0;
        Int16 serverListeningPort = 0;
        string serverHostName = "";

        //OBJECT LIFECYCLE
        public ServerConnection(Int16 managerListeningPort, Int16 serverListeningPort, string serverHostName)
        {
            this.managerListeningPort = managerListeningPort;
            this.serverListeningPort = serverListeningPort;
            this.serverHostName = serverHostName;
        }

        //-----------------SERVER METHODS
        internal void StartTcpServer(dProcessRequest RequestProcessor)
        {
            if (managerListeningPort == 0) throw new ArgumentException("Invalid managerListeningPort");
            if (serverListeningPort == 0) throw new ArgumentException("Invalid serverListeningPort");
            if (string.IsNullOrEmpty(serverHostName)) throw new ArgumentException("Invalid serverHostName");
            if (connectionTask != null) throw new ServerAlreadyExistsException();

            ProcessRequest = RequestProcessor;
            bEndTaskRequested = false;
            connectionTask = new Task(AsyncRunTcpServer);
            connectionTask.ContinueWith(OnTcpServerStopped, TaskScheduler.FromCurrentSynchronizationContext());
        }

        private void AsyncRunTcpServer()
        {
            TcpListener server = null;
            try
            {
                IPAddress localAddr = IPAddress.Parse("127.0.0.1");
                server = new TcpListener(localAddr, managerListeningPort);
                server.Start();

                byte[] inMessageAsBytes = new byte[0];
                byte[] inMessageBuffer = new byte[messageBufferLen];
                string inMessage;

                while (!bEndTaskRequested)
                {
                    try
                    {
                        using (TcpClient client = server.AcceptTcpClient())
                        {

                            NetworkStream stream = client.GetStream();

                            //while receiving data, continue resizing out message array by number 
                            //of received bytes and copy buffer contents to the freshly allocated space.
                            int bytes;
                            while ((bytes = stream.Read(inMessageBuffer, 0, messageBufferLen)) > 0)
                            {
                                int currentLength = inMessageAsBytes.Length;
                                Array.Resize(ref inMessageAsBytes, currentLength + bytes);
                                Array.Copy(inMessageBuffer, 0, inMessageAsBytes, currentLength, bytes);
                            }

                            inMessage = Encoding.UTF8.GetString(inMessageAsBytes);
                            string outMessage = "";
                            bool bSuccess = false;
                            try
                            {
                                bSuccess = ProcessRequest(inMessage, out outMessage);
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine(e);
                            }

                            byte[] outMessageAsBytes = Encoding.UTF8.GetBytes(bSuccess ? outMessage : internalErrorMessage);
                            stream.Write(outMessageAsBytes, 0, outMessageAsBytes.Length);
                            client.Close();
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            finally
            {
                server.Stop();
            }
        }
        
        internal void StopTcpServer()
        {
            if (connectionTask == null) throw new ServerDoesntExistException();
            bEndTaskRequested = true;
            connectionTask.Wait();
        }
        
        private void OnTcpServerStopped(Task arg1, object arg2)
        {
            connectionTask = null;
        }

        //-----------------CLIENT METHODS
        public bool SendMessage(string SentOutMessage, out string ReturnedInMessage)
        {
            try
            {
                using (TcpClient client = new TcpClient(serverHostName, serverListeningPort))
                {
                    NetworkStream stream = client.GetStream();

                    byte[] inMessageAsBytes = Encoding.UTF8.GetBytes(SentOutMessage);
                    stream.Write(inMessageAsBytes, 0, inMessageAsBytes.Length);

                    byte[] outMessageAsBytes = new byte[0];
                    byte[] outMessageBuffer = new Byte[messageBufferLen];

                    //while receiving data, continue resizing out message array by number 
                    //of received bytes and copy buffer contents to the freshly allocated space.
                    int bytes;
                    while ((bytes = stream.Read(outMessageBuffer, 0, messageBufferLen)) > 0)
                    {
                        int currentLength = outMessageAsBytes.Length;
                        Array.Resize(ref outMessageAsBytes, currentLength + bytes);
                        Array.Copy(outMessageBuffer, 0, outMessageAsBytes, currentLength, bytes);
                    }

                    ReturnedInMessage = Encoding.UTF8.GetString(outMessageAsBytes, 0, outMessageAsBytes.Length);
                    stream.Close();
                    client.Close();
                }
                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                ReturnedInMessage = "";
                return false;
            }
        }
    }
}
