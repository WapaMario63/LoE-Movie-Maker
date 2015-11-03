#ifndef SERVERCOMMANDS
#define SERVERCOMMANDS

#include <form.h>

#include <QTextStream>

/*
 * The Console class is an independent class that process all the server's commands.
 * All of this used to be inside widget.h then moved to form.h for LoE MM.
 *
 * This also now holds the debug logging from widget.h and also holds the set
 * player that is ready to be used to input the commands to.
 *
 */

class Console : public QObject
{
    Q_OBJECT
public:
    Console(QObject* parent = 0);
    ~Console();

public:
    void logInfoMsg(QString msg);
    void logErrMsg(QString msg); // stdcerr

    // GET
    Player *getPlayer();

    // SET
    void setPlayer(Player* peer); // All you need now is a player, no wonky id parsing that crashes everything!

    // Helpers
    void clearPlayer();
    void inputCmdHandler(QString str);

    // Event Loop
    void run();

public slots:
    // Slots because it could be usefull for QObject connections and modularity.

    // Normal Commands
    void cmdStopServer();
    void cmdStartServer();
    void cmdShowHelp();
    void cmdShowDebugHelp();
    void cmdListTcpPlayers();
    void cmdSetPlayer();
    void cmdSetPlayer(quint16 id);
    void cmdSetPlayer(QString IP, quint16 port);
    void cmdListPlayers();
    void cmdListPlayers(QString scenename);
    void cmdListVortexes();
    void cmdSync();
    void cmdDebugStressLoad();
    void cmdTpPlayerToPlayer(quint16 sourceId, quint16 destId);
    void cmdServerSay(QString msg);
    void cmdAnnouncement(QString msg, float duration);
    void cmdListNpcs();

    // Commands that require a set player
    void cmdKickPlayer(Player* player, QString reason);
    //void cmdBanPlayer(Player* player); // Incomplete function from old [LoEWCT], disabling alpha function entirely.
    void cmdLoadScene(Player* player, QString scenename);
    void cmdGetPosition(Player* player);
    UVector cmdGetPositionU(Player *player); // For Those who prefer it in its original form. (Not used in cmd)
    void cmdGetRotation(Player* player);
    UQuaternion cmdGetRotationU(Player *player);
    void cmdGetPonyData(Player* player);
    QByteArray cmdGetPonyDataD(Player *player); // For those who prefer getting the raw data. (Not used in cmd)
    void cmdSendPonies(Player* player);
    void cmdSendUtils3(Player* player);
    void cmdSetPlayerId(Player* player, unsigned id);
    void cmdReloadNpcs(Player* player, QString npcName);
    void cmdSendRemoveKill(Player* player);
    void cmdSendRemove(Player* player);
    void cmdSendPonyData(Player* player, QString ponyData);
    void cmdSendPonyData(Player *player, QByteArray ponyData); // Because, there are those times you need to send raw data, right?
    void cmdSetStat(Player* player, quint8 statId, float statValue); // 0 = health, 1 = energy
    void cmdSetMaxStat(Player* player, quint8 statId, float statValue);
    void cmdInstantiate(Player* player);
    void cmdInstantiate(Player* player, unsigned viewId, unsigned ownerId, float posx=0, float posy=0, float posz=0, float rotx=0,float roty=0,float rotz=0, float rotw=0);
    void cmdBeginDialog(Player* player);
    void cmdEndDialog(Player* player);
    void cmdSetDialogMsg(Player* player, QString msg);
    void cmdSetDialogOptions(Player* player, QString options);
    void cmdMove(Player* player, float x, float y, float z);
    void cmdErrorMessage(Player* player, QString msg);
    void cmdListQuests(Player* player);
    void cmdListMobs();
    void cmdListInventory(Player* player);
    void cmdListWornItems(Player* player);
    void cmdGiveItem(Player* player, int itemId, int amount);
    void cmdAnnouncePlayer(Player* player, QString msg, float duration);
private:
    Player* cmdPlayer;
    QString cmdString;
};

extern Console cmd;
extern QTextStream cout;
extern QTextStream cin;
extern QTextStream cerr;

#endif // SERVERCOMMANDS

