#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include <message.h>

class QListWidgetItem;
class QFile;
class Player;

namespace Ui {
class Form;
}

namespace ServerVersion
{
    extern bool isBABSCon; // Use BABScon14 Configurations
    extern bool isAugust; // Use August14 Configurations
    extern bool isJanuray; // Use January15 Server System and Configurations (EARLY ALPHA).
}

class Form : public QWidget
{
    Q_OBJECT

public slots:
    void sendCmdLine();
    void refreshPlayerList();

public:
    explicit Form(QWidget *parent = 0);
    ~Form();

    //Global Vars
    QString clientName, clientExePath, clientDataPath, clientAssemblyPath; // BABSCon Build
    QString clientNameA, clientExePathA, clientDataPathA, clientAssemblyPathA; // August Build
    QString clientNameO, clientExePathO, clientDataPathO, clientAssemblyPathO; // Ohaycon Build
    QString clientNameJ, clientExePathJ, clientDataPathJ, clientAssemblyPathJ; // January Build
    int anHeroDelay;

    Ui::Form *ui;
    void externCmdLine(QString str);
    void logStatusMsg(QString str);
    void launchClient();
    void searchClientPath();

public slots:
    // Server Commands (check out the old serverCommands.cpp for the function definitions!)
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

    // Helpers for player sets
    Player getPlayer();
    Player refreshPlayerInfo(Player* peer);
    Player clearPlayer(Player* peer); // Only for clearing of the cmdPeer!

    // Commands that require a set player
    // All you need now is a player, no wonky id parsing that crashes everything!
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

private slots:
    void on_btnSettings_clicked();
    void on_btnOpenGame_clicked();
    void on_lineEdit_returnPressed();
    void on_btnMapTp_clicked();
    void on_btnPlayerTp_clicked();
    void on_btnPosTp_clicked();
    void on_btnGetPos_clicked();
    void on_btnGetRot_clicked();
    void on_btnGetPonyData_clicked();
    void on_btnSetPlayer_clicked();
    void on_listPlayers_itemClicked(QListWidgetItem *item);
    void on_pushButton_clicked();
    void on_sendInputCmd_clicked();

    void on_btnStartStopServer_clicked();

    void on_pushButton_2_clicked();


    void on_btnAbout_clicked();

    void on_btnAboutQt_clicked();

    void on_btnWebsiteLink_clicked();

    void on_btnNPCManager_clicked();

public:
    enum MsgBoxType
    {
        Critical,
        Warning,
        Info,
        Question
    };
    void MsgBox(MsgBoxType type, QString title, QString text);
};
extern Form lwin;

#endif // FORM_H
