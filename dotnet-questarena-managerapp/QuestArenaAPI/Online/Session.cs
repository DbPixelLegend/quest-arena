using System;
using System.Collections.Generic;
using System.Text;

namespace QuestArenaAPI.Online
{
    public enum SessionState
    {
        BeforeSession,
        InProgress,
        Ended
    }

    public class Session
    {
        public Guid sessionId;

        public int ownerUserId;

        public int roomId;

        public DateTime sessionStartTime; //expected session start time

        public SessionState sessionState;

        public List<Player> players = new List<Player>();

        public string sessionResultJson; //json-encoded session result, valid if sessionState == Ended
    }
}
