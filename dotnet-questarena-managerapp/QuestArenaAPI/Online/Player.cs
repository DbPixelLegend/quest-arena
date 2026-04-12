using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;

namespace QuestArenaAPI.Online
{
    public class Player
    {
        //------------------ACTUAL PROPERTY LIST
        public bool m_isPlayerRegistered;
        public int m_playerId; //on cloud, will be -1 if isPlayerRegistered == false
        public int m_localPlayerId; //corresponds to Headset ID
        public string m_playerName;
        public int m_playerLevel;
        public int m_playerTotalExperience;

        //------------------GET-SETTERS FOR PROPERTYGRID

        [Browsable(true)]
        [ReadOnly(true)]
        [Description("If true, player is logged in to the cloud system. This match will increase his level")]
        [Category("Cloud")]
        [DisplayName("Logged in")]
        public bool IsPlayerRegistered
        {
            get { return m_isPlayerRegistered; }
            set { m_isPlayerRegistered = value; }
        }

        [Browsable(true)]
        [ReadOnly(true)]
        [Description("Cloud system ID. Valid if user is logged in")]
        [Category("Cloud")]
        [DisplayName("Player ID")]
        public int PlayerId
        {
            get { return m_playerId; }
            set { m_playerId = value; }
        }

        [Browsable(true)]
        [ReadOnly(false)]
        [Description("Player ID during match. This must be equal to headset ID given to the person.")]
        [Category("Session")]
        [DisplayName("Headset ID")]
        public int LocalPlayerId
        {
            get { return m_localPlayerId; }
            set { m_localPlayerId = value; }
        }

        [Browsable(true)]
        [ReadOnly(false)]
        [Description("Player name visible in scoreboard.")]
        [Category("Session")]
        [DisplayName("Player name")]
        public string PlayerName
        {
            get { return m_playerName; }
            set { m_playerName = value; }
        }

        [Browsable(true)]
        [ReadOnly(true)]
        [Description("Player level")]
        [Category("Cloud")]
        [DisplayName("Player level")]
        public int PlayerLevel
        {
            get { return m_playerLevel; }
            set { m_playerLevel = value; }
        }

        [Browsable(true)]
        [ReadOnly(true)]
        [Description("Total experience points accumulated by this player")]
        [Category("Cloud")]
        [DisplayName("Player EXP")]
        public int PlayerTotalExperience
        {
            get { return m_playerTotalExperience; }
            set { m_playerTotalExperience = value; }
        }
    }
}
