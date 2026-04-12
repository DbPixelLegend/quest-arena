# Quest Arena

A multiplayer VR arena game built with Unreal Engine 4, designed for VR headsets with dedicated server management tools.

## 📋 Overview

Quest Arena is a virtual reality multiplayer game featuring:

- **VR Combat Arena** - Immersive multiplayer battles in virtual reality
- **Player Progression System** - Cloud-based player accounts with levels and experience tracking
- **Dedicated Server Architecture** - Standalone server builds for hosting matches
- **Server Manager Application** - Windows desktop tool for server administration and player management

## 🎯 Project Structure

This repository contains three main components:

### 1. Unreal Engine Game (`unreal-questarena-app/`)

The main VR game client and server built in Unreal Engine 4.

**Key Features:**

- VR-optimized gameplay with hand tracking and motion controls
- Network multiplayer support
- Session-based matchmaking
- VR Toolkit integration

**Requirements:**

- Unreal Engine 4.x
- VR headset (Oculus Quest/Meta Quest compatible)

### 2. .NET Manager Application (`dotnet-questarena-managerapp/`)

Windows desktop application for managing game servers and player sessions.

**Components:**

#### QuestArenaManager

Windows Forms application providing:

- Server process management (start/stop/monitor)
- Player session management
- Real-time player registration
- Headset ID assignment
- Player property editing

#### QuestArenaAPI

Class library containing:

- **Online System**: Player, Room, and Session management
- **Server Components**: Server controller, connection handler, and process manager
- Network communication layer

**Requirements:**

- .NET Framework
- Windows OS

### 3. Game Assets (`unreal-questarena-assets/`)

Shared assets and resources for the Unreal Engine project.

## 🚀 Getting Started

### Prerequisites

- **Unreal Engine 4** (version specified in .uproject)
- **Visual Studio 2019+** with .NET desktop development workload
- **VR Development Kit** (for testing)
- **Git** for version control

### Building the Game

1. **Clone the repository:**

   ```bash
   git clone <repository-url>
   cd Quest Arena
   ```

2. **Open the Unreal project:**
   - Navigate to `unreal-questarena-app/`
   - Right-click `QuestArena.uproject` → "Generate Visual Studio project files"
   - Open `QuestArena.uproject` in Unreal Engine

3. **Build the game:**
   - For client: Build for your target VR platform
   - For server: Package as Windows Server build

### Building the Manager Application

1. **Open the solution:**

   ```bash
   cd dotnet-questarena-managerapp
   ```

   - Open `QuestArenaManager.sln` in Visual Studio

2. **Restore NuGet packages:**
   - Visual Studio will automatically restore dependencies

3. **Build the solution:**
   - Build → Build Solution (Ctrl+Shift+B)

4. **Run the Manager:**
   - Set `QuestArenaManager` as startup project
   - Press F5 to run

## 🎮 Usage

### Running a Game Server

1. **Launch Quest Arena Manager**
2. **Configure server path** (default: `D:/QuestBuildCemetery/mark_ready/WindowsNoEditor/QuestArena/Binaries/Win64/QuestArenaServer.exe`)
3. **Click "Start Server"** to launch the dedicated server
4. **Server runs on port 6969** (configurable)

### Managing Players

1. **Add players** using the "Add Player" button
2. **Assign Headset IDs** to match physical VR headsets
3. **View player properties:**
   - Cloud registration status
   - Player ID (cloud-based)
   - Player level and experience
   - Session information
4. **Players can join** once server is running and their headset ID is registered

### Player Progression System

- **Cloud Integration**: Players can log in to track progress across sessions
- **Experience System**: Matches award experience points
- **Level Progression**: Players level up based on total experience
- **Persistent Profiles**: Stats are saved when players are registered with the cloud

## 🔧 Configuration

### Server Configuration

- **Default Port**: 6969
- **Manager Communication Ports**: 6970-6971
- **Host**: localhost (configurable)

### Server Path

Update `ServerManager.cs` to match your server build location:

```csharp
string serverPathOnDisk = "YOUR_PATH/QuestArenaServer.exe";
```

## 🏗️ Architecture

### Network Architecture

```
VR Client (Headset)
    ↓
Dedicated Server (Unreal)
    ↓
Manager Application (.NET)
    ↓
Cloud System (Player Data)
```

### Key Components

- **ServerManager**: Launches and monitors the Unreal server process
- **ServerController**: Manages server commands and state
- **ServerConnection**: TCP/IP communication layer
- **Player System**: Handles player registration and progression
- **Session System**: Manages match sessions and rooms

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 Development Notes

- The manager application expects the server executable at a specific path - update this in `ServerManager.cs`
- VR headset IDs must match the assigned local player IDs in the manager
- The server communicates with the manager on separate ports for bidirectional communication
- Player data is only persisted when users are registered with the cloud system

## 📄 License

[Specify your license here]

## 🙋 Support

For issues, questions, or contributions, please open an issue in the GitHub repository.

---

**Built with:**

- Unreal Engine 4
- C# / .NET Framework
- VR Toolkit
- TCP/IP Networking
