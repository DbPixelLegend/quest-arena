using System;
using System.Collections.Generic;
using System.Text;
using QuestArenaAPI.Online;

namespace QuestArenaAPI.Server
{
    public class Globals
    {
        //-------------------SINGLETON IMPLEMENTATION

        private static Globals instance;
        private Globals() { instance = this; }

        public static Globals Get() { return (instance == null ? new Globals() : instance); }

        //-------------------PROPERTIES

        public Session currentSession = new Session();
        public ServerManager serverManager = new ServerManager();
    }
}
