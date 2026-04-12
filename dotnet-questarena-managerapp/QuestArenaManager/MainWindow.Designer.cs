namespace QuestArenaManager
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnStartServer = new System.Windows.Forms.Button();
            this.pgPlayer = new System.Windows.Forms.PropertyGrid();
            this.listPlayers = new System.Windows.Forms.ListBox();
            this.btnAddPlayer = new System.Windows.Forms.Button();
            this.btnRemovePlayer = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnStartServer
            // 
            this.btnStartServer.Location = new System.Drawing.Point(12, 240);
            this.btnStartServer.Name = "btnStartServer";
            this.btnStartServer.Size = new System.Drawing.Size(355, 44);
            this.btnStartServer.TabIndex = 0;
            this.btnStartServer.Text = "Start server";
            this.btnStartServer.UseVisualStyleBackColor = true;
            this.btnStartServer.Click += new System.EventHandler(this.btnStartServer_Click);
            // 
            // pgPlayer
            // 
            this.pgPlayer.Location = new System.Drawing.Point(138, 12);
            this.pgPlayer.Name = "pgPlayer";
            this.pgPlayer.Size = new System.Drawing.Size(229, 222);
            this.pgPlayer.TabIndex = 1;
            // 
            // listPlayers
            // 
            this.listPlayers.FormattingEnabled = true;
            this.listPlayers.Location = new System.Drawing.Point(12, 12);
            this.listPlayers.Name = "listPlayers";
            this.listPlayers.Size = new System.Drawing.Size(120, 186);
            this.listPlayers.TabIndex = 2;
            this.listPlayers.SelectedIndexChanged += new System.EventHandler(this.listPlayers_SelectedIndexChanged);
            // 
            // btnAddPlayer
            // 
            this.btnAddPlayer.Location = new System.Drawing.Point(12, 211);
            this.btnAddPlayer.Name = "btnAddPlayer";
            this.btnAddPlayer.Size = new System.Drawing.Size(44, 23);
            this.btnAddPlayer.TabIndex = 3;
            this.btnAddPlayer.Text = "+";
            this.btnAddPlayer.UseVisualStyleBackColor = true;
            this.btnAddPlayer.Click += new System.EventHandler(this.btnAddPlayer_Click);
            // 
            // btnRemovePlayer
            // 
            this.btnRemovePlayer.Location = new System.Drawing.Point(62, 211);
            this.btnRemovePlayer.Name = "btnRemovePlayer";
            this.btnRemovePlayer.Size = new System.Drawing.Size(44, 23);
            this.btnRemovePlayer.TabIndex = 4;
            this.btnRemovePlayer.Text = "-";
            this.btnRemovePlayer.UseVisualStyleBackColor = true;
            this.btnRemovePlayer.Click += new System.EventHandler(this.btnRemovePlayer_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(383, 300);
            this.Controls.Add(this.btnRemovePlayer);
            this.Controls.Add(this.btnAddPlayer);
            this.Controls.Add(this.listPlayers);
            this.Controls.Add(this.pgPlayer);
            this.Controls.Add(this.btnStartServer);
            this.Name = "MainWindow";
            this.Text = "QuestArena Manager";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnStartServer;
        private System.Windows.Forms.PropertyGrid pgPlayer;
        private System.Windows.Forms.ListBox listPlayers;
        private System.Windows.Forms.Button btnAddPlayer;
        private System.Windows.Forms.Button btnRemovePlayer;
    }
}

