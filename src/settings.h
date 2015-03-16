#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

#define DEFAULT_SYNC_INTERVAL 250
#define IF_AUGUST_BUILD false
#define IF_BABSCON_BUILD true
#define LOEWCT_VERSION "v0.6.0"
#define MAX_PACKET_DUPS 1000

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
