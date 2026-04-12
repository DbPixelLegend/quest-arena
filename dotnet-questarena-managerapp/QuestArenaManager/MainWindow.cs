using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using QuestArenaAPI.Online;
using QuestArenaAPI.Server;

namespace QuestArenaManager
{
    public partial class MainWindow : Form
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnAddPlayer_Click(object sender, EventArgs e)
        {
            Session session = Globals.Get().currentSession;
            Player newPlayer = new Player();

            session.players.Add(newPlayer);
            UpdatePlayerListbox();
            listPlayers.SelectedIndex = listPlayers.Items.Count - 1; //select newest player
        }

        private void btnRemovePlayer_Click(object sender, EventArgs e)
        {
            if (listPlayers.SelectedIndex < 0) return;
            int playerIndex = listPlayers.SelectedIndex;

            Session session = Globals.Get().currentSession;
            session.players.RemoveAt(playerIndex);
            UpdatePlayerListbox();
        }

        private void UpdatePlayerListbox()
        {
            Session session = Globals.Get().currentSession;

            listPlayers.Items.Clear();
            foreach (Player p in session.players)
            {
                listPlayers.Items.Add(p.ToString());
            }
        }

        private void listPlayers_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listPlayers.SelectedIndex < 0) return;
            Session session = Globals.Get().currentSession;
            Player p = session.players[listPlayers.SelectedIndex];

            pgPlayer.SelectedObject = p;
        }

        private void btnStartServer_Click(object sender, EventArgs e)
        {
            ServerManager sm = Globals.Get().serverManager;

            if (sm.IsRunning)
            {
                sm.KillServerProcess();
                btnStartServer.Text = "Start server";
            }
            else
            {
                sm.OnServerClosed += Handle_ServerClosed;
                sm.StartServerProcess();
                btnStartServer.Text = "Kill server";
            }
        }

        private void Handle_ServerClosed(int ResultCode)
        {
            this.Invoke(new Action(() =>
            {
                btnStartServer.Text = "Start server";
            }));
        }
    }
}
