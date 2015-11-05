#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

/*
 ** We getting some PoneNgine stuff in here! **

 * Removed mobile devices and WinRT, because there is no point for a server on those. *
*/
/// BEGIN ponesettings.h from PoneNgine
// Defined all possible platforms
#define PLAT_WIN32 0
#define PLAT_WIN64 1
#define PLAT_MACOSX 2
#define PLAT_UNIX 3

// Yes we are calling almost everything with Pony at the beginning
// The engine is named after pony, so, why not?

#if defined(WIN32) || defined(_WIN32)
#define PONY_PLATFORM PLAT_WIN32
#elif defined(_WIN64)
#define PONY_PLATFORM PLAT_WIN64
#elif defined(__APPLE__)
#define PONY_PLATFORM PLAT_MACOSX
#else
#define PONY_PLATFORM PLAT_UNIX
#endif

/// END ponesettings.h from PoneNgine

#define DEFAULT_SYNC_INTERVAL 250
#define IF_AUGUST_BUILD false
#define IF_BABSCON_BUILD true
#define IF_BABS15_BUILD false
#define IF_AUG15_BUILD false
#define LOEWCT_VERSION "v0.6.2a"
#define LOE_MM_VERSION "v1.2a"
#define MAX_PACKET_DUPS 1000

//#define BABSCON15

/* LoE Server Config */

namespace Settings
{

extern int loginPort; // Port for the login server
extern int gamePort; // Port for the game server
extern QString remoteLoginIP; // IP of the remote login server
extern int remoteLoginPort; // Port of the remote login server
extern int remoteLoginTimeout; // Time before we give up connecting to the remote login server
extern bool useRemoteLogin; // Whever or not to use the remote login server
extern int maxRegistered; // Max number of registered players in database, can deny registration
extern int pingTimeout; // Max time between recdption of pings, can disconnect player
extern int pingCheckInterval; // Time between ping timeout checks
extern bool logInfos; // Can disable logMessage, but doesn't affect logStatusMessage
extern QString saltPassword; // Used to check passwords between login and game servers, must be the same on all the servers involved
extern bool enableSessKeyValidation; // Enable Session Key Validation
extern bool enableLoginServer; // Starts a login server
extern bool enableMultiplayer; // Sync players' positions
extern bool enableGetlog; // Enable GET /log requests
extern bool enableGameServer; // Starts a game server
extern int maxConnected; // Max number of players connected at the same time, can deny login

}

/* LoEWCT config */

namespace LoEWCTsettings
{

extern QString motdMessage;
extern QString rulesMessage;
extern QString helpMessage;
extern QString serverPrefix;
extern bool mapTpEnable;
extern int rollCoolDown;
extern bool pvp;
extern bool playerJoinedMsg;
extern int playerListRefreshInterval;
extern bool playerListRefresh;
}

namespace PlayerSettings
{

extern bool isBanned; // check if the player is banned
extern bool isModerator; // check if the player is a server moderator
extern QString banReason; // reason of why the player is banned
extern QString ip; // player's IP

}

#endif // SETTINGS_H
