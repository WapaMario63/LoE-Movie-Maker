#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QWidget>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>
#include <QTimer>
#include <QList>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QSocketNotifier>
#include <QtConcurrent/QtConcurrentMap>
#include <QCryptographicHash>
#include <QMutex>
#include <QMap>
#include <cstdint>
#include <iostream>

//#include <windows.h>
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#include "character.h"
#include "scene.h"
#include "sync.h"
#include "quest.h"
#include "settings.h"

#include "luascript.h"
#include "lib.h"

#define GAMEDATAPATH "data/data/"
#define PLAYERSPATH "data/players/"
#define NETDATAPATH "data/netData/"
#define CONFIGFILEPATH "data/server.ini"
#define SERVERSLISTFILEPATH "data/serversList.cfg"
#define LOEWCTCONFIGFILEPATH "LoEWCT_config.ini"
#define MOBSPATH "data/mobZones/"

class Mobzone;
class Mob;
namespace Ui {class Widget;}

class Widget : public QWidget
{
    Q_OBJECT

    /// Main functions
public slots:
    //void sendCmdLine();
    //void externCmdLine(QString str);
    void checkPingTimeouts();
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void logMessage(QString msg);
    void logStatusMessage(QString msg);
    void startServer();
    void stopServer(); // Calls stopServer(true)
    void stopServer(bool log);
    int getNewNetviewId();
    int getNewId();

    /// UDP/TCP
public slots:
    void udpProcessPendingDatagrams();
    void tcpConnectClient();
    void tcpDisconnectClient();
    void tcpProcessPendingDatagrams();
public:
    void tcpProcessData(QByteArray data, QTcpSocket *socket);
#if IS_LIB
    void useLibraryMode();
#endif

public:
    Ui::Widget* ui;
    float startTimestamp;
    QUdpSocket *udpSocket;
    QList<Player*> tcpPlayers; // Used by the TCP login server
    QList<Player*> udpPlayers; // Used by the UDP game server
    QList<Pony*> npcs; // List of npcs from the npcs DB
    QList<Quest> quests; // List of quests from the npcs DB
    QMap<uint32_t, uint32_t> wearablePositionsMap; // Maps item IDs to their wearable positions.
    int loginPort; // Port for the login server
    int gamePort; // Port for the game server
    QList<Scene> scenes; // List of scenes from the vortex DB
    int lastNetviewId;
    int lastId;
    QMutex lastIdMutex; // Protects lastId and lastNetviewId
    int syncInterval;
    QList<Mobzone*> mobzones;
    QList<Mob*> mobs;

    // Public config
    bool enableGameServer; // Starts a game server
    Player* cmdPeer; // Player selected for the server commands
    int maxConnected; // Max numbre of players connected at the same time, can deny login

    Sync sync;
private:
    QTcpServer* tcpServer;
    QList<QPair<QTcpSocket*, QByteArray*>> tcpClientsList;
    QTcpSocket remoteLoginSock; // Socket to the remote login server, if we use one
    QByteArray* tcpReceivedDatas;

    QTimer* pingTimer;

    bool* usedids;

    // Private Config
    QString remoteLoginIP; // IP of the remote login server
    int remoteLoginPort; // Port of the remote login server
    int remoteLoginTimeout; // Time before we give up connecting to the remote login server
    bool useRemoteLogin; // Whever or not to use the remote login server

    int maxRegistered; // Max number of registered players in database, can deny registration
    int pingTimeout; // Max time between recdption of pings, can disconnect player
    int pingCheckInterval; // Time between ping timeout checks
    bool logInfos; // Can disable logMessage, but doesn't affect logStatusMessage
    QString saltPassword; // Used to check passwords between login and game servers, must be the same on all the servers involved
    bool enableSessKeyValidation; // Enable Session Key Validation
    bool enableLoginServer; // Starts a login server

    bool enableMultiplayer; // Sync players' positions
    bool enableGetlog; // Enable GET /log requests*/

    // Console stuff
    QSocketNotifier *cin_notifier;
};

// Global import from main
extern Widget win;
extern QTextStream cout;
extern QTextStream cin;

#endif // WIDGET_H
