#include "form.h"
#include "ui_form.h"
#include "widget.h"
#include "message.h"
#include "Utils/utils.h"
#include "Utils/serialize.h"
#include "mob.h"
#include "settings_widget.h"
#include "demo.h"
#include "character.h"
#include "console.h"

#include <QProcess>
#include <QMessageBox>
#include <QSound>
#include <QDesktopServices>
#include <QUrl>

bool ServerVersion::isBABSCon = true; // Use BABScon14 Configurations
bool ServerVersion::isAugust = false; // Use August14 Configurations
bool ServerVersion::isJanuray = false; // Use January15 Server System and Configurations (EARLY ALPHA).

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    clientName = "";
    clientExePath = "";
    clientDataPath = "";
    clientAssemblyPath = "";
    clientNameA = "";
    clientExePathA = "";
    clientDataPathA = "";
    clientAssemblyPathA = "";
    clientNameO = "";
    clientExePathO = "";
    clientDataPathO = "";
    clientAssemblyPathO = "";
    clientNameJ = "";
    clientExePathJ = "";
    clientDataPathJ = "";
    clientAssemblyPathJ = "";

    connect(ui->sendInputCmd, SIGNAL(clicked()), this, SLOT(sendCmdLine()));

    QTimer* timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshPlayerList()));
    timer->start(5000);
}

Form::~Form()
{
    disconnect(ui->sendInputCmd, SIGNAL(clicked()), this, SLOT(sendCmdLine()));

    delete ui;
}

void Form::MsgBox(MsgBoxType type, QString title, QString text)
{
    QApplication::beep(); // Windows Only
    if (type == MsgBoxType::Warning) QMessageBox::warning(this,title,text);
    else if (type == MsgBoxType::Question) QMessageBox::question(this,title,text);
    else if (type == MsgBoxType::Info) QMessageBox::information(this,title,text);
    else if (type == MsgBoxType::Critical) QMessageBox::critical(this,title,text);
}

void Form::externCmdLine(QString str)
{
    //ui->cmdLine->clear();
    //ui->cmdLine->insert(str);
    //ui->cmdLine->returnPressed();
    //ui->cmdLine->clear();
    //cin >> str;
    ui->cmdInput->clear();
    ui->cmdInput->insert(str);
    ui->cmdInput->returnPressed();
    ui->cmdInput->clear();
}
void Form::logStatusMsg(QString msg)
{
    ui->lblServStatus->setText(msg);
}


void Form::sendCmdLine()
{
    QSettings loeWctConfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
    QSettings playerConfig(PLAYERSPATH+win.cmdPeer->name.toLatin1()+"/player.ini", QSettings::IniFormat);
    QString serverPrefix = loeWctConfig.value("serverPrefix", "[LoEWCT]").toString();

    if (!win.enableGameServer)
    {
        win.logMessage("This is not a game server, commands are disabled");
        return;
    }

    //QString str = ui->cmdLine->text();
    //QString str = cin.readLine();
    QString str = ui->cmdInput->text();

    /*if (str == "clear")
    {
        ui.
        return;
    }*/
    if (str == "stop")
    {
        cmd.cmdStopServer();
    }
    else if (str == "start")
    {
        cmd.cmdStartServer();
    }
    else if (str == "help")
    {
        cmd.cmdShowHelp();
    }
    else if (str == "helpDebug")
    {
        cmd.cmdShowDebugHelp();
    }
    else if (str == "listTcpPlayers")
    {
        cmd.cmdListTcpPlayers();
        return;
    }
    else if (str.startsWith("setPlayer"))
    {
        if (win.udpPlayers.size() == 1)
        {
            cmd.cmdSetPlayer(); return;
        }

        str = str.right(str.size()-10);
        QStringList args = str.split(":");
        bool ok;
        if (args.size() != 2)
        {
            quint16 id = args[0].toUInt(&ok);
            if (!ok)
            {
                win.logMessage("[INFO] UDP: setPlayer takes a player ID as a function/argument");
                return;
            }
            else
            {
                cmd.cmdSetPlayer(id); return;
            }
        }
        else
        {
            quint16 port = args[1].toUInt(&ok);
            if (!ok)
            {
                win.logMessage("[INFO] UDP: setPlayer takes a player ID as a function/argument");
                return;
            }
            else
            {
                cmd.cmdSetPlayer(args[0], port); return;
            }
        }
    }
    else if (str.startsWith("listPlayers"))
    {
        if (str.size() <= 12)
        {
            cmd.cmdListPlayers(); return;
        }
        else
        {
            str = str.right(str.size()-12);
            cmd.cmdListPlayers(str); return;
        }
    }
    else if (str.startsWith("listVortexes"))
    {
        cmd.cmdListVortexes();
        return;
    }
    else if (str.startsWith("sync"))
    {
        cmd.cmdSync();
        return;
    }
    // DEBUG global commands from now on
    else if (str==("dbgStressLoad"))
    {
        cmd.cmdDebugStressLoad();
    }
    else if (str.startsWith("tp"))
    {
        str = str.right(str.size()-3);
        QStringList args = str.split(" ", QString::SkipEmptyParts);

        cmd.cmdTpPlayerToPlayer(args[0].toUInt(), args[1].toUInt());
    }
    else if (str.startsWith("say"))
    {
        str = str.right(str.size()-4);

        cmd.cmdServerSay(str);
    }
    else if (str.startsWith("gm"))
    {
        str = str.right(str.size()-3);
        if (str == "deathmatch")
        {
            for (int i=0; i<win.udpPlayers.size(); i++)
            {
                sendAnnouncementMessage(win.udpPlayers[i], "Gamemode changed to Deathmatch mode! You will be reloaded in 20 seconds.",20);
            }
        }
    }
    else if (str.startsWith("announceall"))
    {
        str = str.right(str.size()-12);
        QStringList args = str.split("|", QString::SkipEmptyParts);
        cmd.cmdAnnouncement(args[1], args[0].toFloat());
    }

    if (win.cmdPeer->IP=="")
    {
        win.logMessage("[INFO] Select a player first with setPlayer or look for some with listPlayers");
        MsgBox(MsgBoxType::Warning, "Error", "Please set a player from the player list first.");
        return;
    }
    else // Refresh peer info
    {
        win.cmdPeer = Player::findPlayer(win.udpPlayers,win.cmdPeer->IP, win.cmdPeer->port);
        if (win.cmdPeer->IP=="")
        {
            win.logMessage(QString("UDP: Player not found"));
            return;
        }
    }

    // User commands from now on (requires setPlayer)
    if (str.startsWith("kick"))
    {
        str = str.right(str.size()-5);
        QByteArray data(1,0);
        data[0] = 0x7f; // Request number

        if (str.size() <= 5)
        {
            win.logMessage("[ERROR] kick has these arguments: kick <reason>. You NEED to provide a reason of why you kicked the player.");
            return;
        }

        data += stringToData(str);

        win.logMessage(QString("[INFO] Kicking the player..."));
        sendMessage(win.cmdPeer,MsgDisconnect, "Kicked by Admin \n\nReason \n-------------------------------\n"+data);
        Player::disconnectPlayerCleanup(win.cmdPeer); // Save game and remove the player

        for (int i=0; i<win.udpPlayers.size(); i++)
          {
            sendChatMessage(win.udpPlayers[i], "Kicked <span color=\"cyan\">"+win.cmdPeer->pony.name+" ("+win.cmdPeer->name+")</span> for <span color\"orange\">"+str+"</span>", serverPrefix, ChatGeneral);
          }
    }
    else if (str.startsWith("ban"))
    {
        str = str.right(str.size()-4);

        // kick session
        QByteArray data(1,0);
        data[0] = 0x7f; // Request number

        if (str.size() <= 5)
        {
            win.logMessage("[ERROR] ban has these arguments: ban <reason>. You NEED to provide a reason of why you banned the player.");
            return;
        }

        data += stringToData(str);

        win.logMessage(QString("[INFO] Banning the player..."));
        sendMessage(win.cmdPeer,MsgDisconnect, "Banned by Admin \n\nReason \n-------------------------------\n"+data);
        Player::disconnectPlayerCleanup(win.cmdPeer); // Save game and remove the player

        for (int i=0; i<win.udpPlayers.size(); i++)
          {
            sendChatMessage(win.udpPlayers[i], "Banned <span color=\"cyan\">"+win.cmdPeer->pony.name+" ("+win.cmdPeer->name+")</span> for <span color\"orange\">"+str+"</span>", serverPrefix, ChatGeneral);
          }

        // Ban session
        playerConfig.setValue("isBanned", true);
        playerConfig.setValue("banReason", str);
        playerConfig.setValue("ip", win.cmdPeer->IP);

        // Yes its that short and crappy, I made something more complicated, but didn't work.
    }
    else if (str.startsWith("load"))
    {
        str = str.mid(5);
        sendLoadSceneRPC(win.cmdPeer, str);
    }
    else if (str.startsWith("getPos"))
    {
        win.logMessage(QString("Pos : x=") + QString().setNum(win.cmdPeer->pony.pos.x)
                   + ", y=" + QString().setNum(win.cmdPeer->pony.pos.y)
                   + ", z=" + QString().setNum(win.cmdPeer->pony.pos.z));
    }
    else if (str.startsWith("getRot"))
    {
        win.logMessage(QString("Rot : x=") + QString().setNum(win.cmdPeer->pony.rot.x)
                   + ", y=" + QString().setNum(win.cmdPeer->pony.rot.y)
                   + ", z=" + QString().setNum(win.cmdPeer->pony.rot.z)
                   + ", w=" + QString().setNum(win.cmdPeer->pony.rot.w));
    }
    else if (str.startsWith("getPonyData"))
    {
        win.logMessage("[INFO] ponyData for this player: "+win.cmdPeer->pony.ponyData.toBase64());
    }
    else if (str.startsWith("sendPonies"))
    {
        win.logMessage("[INFO] Sending ponies to this player.");
        sendPonies(win.cmdPeer);
    }
    else if (str.startsWith("sendUtils3"))
    {
        win.logMessage("[INFO] UDP: Sending Utils3 request");
        QByteArray data(1,3);
        sendMessage(win.cmdPeer,MsgUserReliableOrdered6,data);
    }
    else if (str.startsWith("setPlayerId"))
    {
        str = str.right(str.size()-12);
        QByteArray data(3,4);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
        {
            win.logMessage("[INFO] UDP: Sending setPlayerId request");
            data[1]=(quint8)(id&0xFF);
            data[2]=(quint8)((id >> 8)&0xFF);
            sendMessage(win.cmdPeer,MsgUserReliableOrdered6,data);
        }
        else
            win.logStatusMessage("Error : Player ID is a number");
    }
    else if (str.startsWith("reloadNpc"))
    {
        win.logMessage("[INFO] Reloading the NPC Database...");
        str = str.mid(10);
        Pony* npc = NULL;
        for (int i=0; i<win.npcs.size(); i++)
            if (win.npcs[i]->name == str)
            {
                npc = win.npcs[i];
                break;
            }
        if (npc != NULL)
        {
            // Reload the NPCs from the DB
            win.npcs.clear();
            win.quests.clear();
            unsigned nQuests = 0;
            QDir npcsDir("data/npcs/");
            QStringList files = npcsDir.entryList(QDir::Files);
            for (int i=0; i<files.size(); i++, nQuests++) // For each vortex file
            {
                Quest *quest = new Quest("data/npcs/"+files[i], NULL);
                win.quests << *quest;
                win.npcs << quest->npc;
            }
            win.logMessage("[INFO] Loaded "+QString().setNum(nQuests)+" quests/npcs.");

            // Resend the NPC if needed
            if (npc->sceneName.toLower() == win.cmdPeer->pony.sceneName.toLower())
            {
                sendNetviewRemove(win.cmdPeer, npc->netviewId);
                sendNetviewInstantiate(npc, win.cmdPeer);
            }
        }
        else
            win.logMessage("[INFO] NPC not found");
    }
    else if (str.startsWith("removekill"))
      {
        str = str.right(str.size()-11);
        QByteArray data(4,2);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
          {
            win.logMessage("[INFO] UDP: Sending remove request with kill reason code");
            data[1]=id;
            data[2]=id >> 8;
            data[3]=NetviewRemoveReasonKill;
            sendMessage(win.cmdPeer, MsgUserReliableOrdered6, data);
          }
        else
          win.logStatusMessage("[ERROR] Removekill needs the id of the view to remove");
      }
    else if (str.startsWith("remove"))
    {
        str = str.right(str.size()-7);
        QByteArray data(3,2);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
        {
            win.logMessage("[INFO] UDP: Sending remove request");
            data[1]=id;
            data[2]=id >> 8;
            sendMessage(win.cmdPeer,MsgUserReliableOrdered6,data);
        }
        else
            win.logStatusMessage("[INFO] Error : Remove needs the id of the view to remove");
    }
    else if (str.startsWith("sendPonyData"))
    {
        str = str.right(str.size()-13);

        QByteArray pData = stringToData(str);

        QByteArray data(3,0xC8);
        data[0] = (quint8)(win.cmdPeer->pony.netviewId&0xFF);
        data[1] = (quint8)((win.cmdPeer->pony.netviewId>>8)&0xFF);
        //data += win.cmdPeer->pony.ponyData;
        data += pData;
        sendMessage(win.cmdPeer, MsgUserReliableOrdered18, data);
        return;
    }
    else if (str.startsWith("setStat"))
    {
        str = str.right(str.size()-8);
        QStringList args = str.split(' ');
        if (args.size() != 2)
        {
            win.logStatusMessage("[INFO] Error : usage is setStat StatID StatValue");
            return;
        }
        bool ok,ok2;
        quint8 statID = args[0].toInt(&ok);
        float statValue = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            win.logStatusMessage("[INFO] Error : usage is setStat StatID StatValue");
            return;
        }
        sendSetStatRPC(win.cmdPeer, statID, statValue);
    }
    else if (str.startsWith("setMaxStat"))
    {
        str = str.right(str.size()-11);
        QStringList args = str.split(' ');
        if (args.size() != 2)
        {
            win.logStatusMessage("[INFO] Error : usage is setMaxStat StatID StatValue");
            return;
        }
        bool ok,ok2;
        quint8 statID = args[0].toInt(&ok);
        float statValue = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            win.logStatusMessage("[INFO] Error : usage is setMaxState StatID StatValue");
            return;
        }
        sendSetMaxStatRPC(win.cmdPeer, statID, statValue);
    }
    else if (str.startsWith("instantiate"))
    {
        if (str == "instantiate")
        {
            win.logMessage("[INFO] UDP: Cloning/instantiating this player.");
            sendNetviewInstantiate(win.cmdPeer);
            return;
        }

        QByteArray data(1,1);
        str = str.right(str.size()-12);
        QStringList args = str.split(' ');

        if (args.size() != 3 && args.size() != 6 && args.size() != 10)
        {
            win.logStatusMessage(QString("[INFO] Error : Instantiate takes 0,3,6 or 10 arguments").append(str));
            return;
        }
        // Au as au moins les 3 premiers de toute facon
        data += stringToData(args[0]);
        unsigned viewId, ownerId;
        bool ok1, ok2;
        viewId = args[1].toUInt(&ok1);
        ownerId = args[2].toUInt(&ok2);
        if (!ok1 || !ok2)
        {
            win.logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
            return;
        }
        QByteArray params1(4,0);
        params1[0] = (quint8)(viewId&0xFF);
        params1[1] = (quint8)((viewId >> 8)&0xFF);
        params1[2] = (quint8)(ownerId&0xFF);
        params1[3] = (quint8)((ownerId >> 8)&0xFF);
        data += params1;
        float x1=0,y1=0,z1=0;
        float x2=0,y2=0,z2=0,w2=0;
        if (args.size() >= 6) // Si on a le vecteur position on l'ajoute
        {
            bool ok1, ok2, ok3;
            x1=args[3].toFloat(&ok1);
            y1=args[4].toFloat(&ok2);
            z1=args[5].toFloat(&ok3);

            if (!ok1 || !ok2 || !ok3)
            {
                win.logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
                return;
            }
        }
        data+=floatToData(x1);
        data+=floatToData(y1);
        data+=floatToData(z1);

        if (args.size() == 10) // Si on a le quaternion rotation on l'ajoute
        {
            bool ok1, ok2, ok3, ok4;
            x2=args[6].toFloat(&ok1);
            y2=args[7].toFloat(&ok2);
            z2=args[8].toFloat(&ok3);
            w2=args[9].toFloat(&ok4);

            if (!ok1 || !ok2 || !ok3 || !ok4)
            {
                win.logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
                return;
            }
        }
        data+=floatToData(x2);
        data+=floatToData(y2);
        data+=floatToData(z2);
        data+=floatToData(w2);

        win.logMessage(QString("[INFO] UDP: Instantiating player").append(args[0]));
        sendMessage(win.cmdPeer,MsgUserReliableOrdered6,data);
    }
    else if (str.startsWith("beginDialog"))
    {
        QByteArray data(1,0);
        data[0] = 11; // Request number

        sendMessage(win.cmdPeer,MsgUserReliableOrdered4, data);
    }
    else if (str.startsWith("endDialog"))
    {
        QByteArray data(1,0);
        data[0] = 13; // Request number

        sendMessage(win.cmdPeer,MsgUserReliableOrdered4, data);
    }
    else if (str.startsWith("setDialogMsg"))
    {
        str = str.right(str.size()-13);
        QStringList args = str.split(" ", QString::SkipEmptyParts);
        if (args.size() != 2)
            win.logMessage("[INFO]setDialogMsg takes two args : dialog and npc name");
        else
        {
            QByteArray data(1,0);
            data[0] = 0x11; // Request number
            data += stringToData(args[0]);
            data += stringToData(args[1]);
            data += (char)0; // emoticon
            data += (char)0; // emoticon

            sendMessage(win.cmdPeer,MsgUserReliableOrdered4, data);
        }
    }
    else if (str.startsWith("setDialogOptions"))
    {
        str = str.right(str.size()-17);
        QStringList args = str.split(" ", QString::SkipEmptyParts);
        sendDialogOptions(win.cmdPeer, args);
    }
    else if (str.startsWith("move"))
    {
        str = str.right(str.size()-5);
        QByteArray data(1,0);
        data[0] = 0xce; // Request number

        // Serialization : float x, float y, float z
        QStringList coords = str.split(' ');
        if (coords.size() != 3)
            return;

        sendMove(win.cmdPeer, coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat());
    }
    else if (str.startsWith("error"))
    {
        str = str.right(str.size()-6);
        QByteArray data(1,0);
        data[0] = 0x7f; // Request number

        data += stringToData(str);

        sendMessage(win.cmdPeer,MsgUserReliableOrdered4, data);
    }
    else if (str==("listQuests"))
      {
        for (const Quest& quest : win.cmdPeer->pony.quests)
          {
            win.logMessage("[INFO] Quest "+QString().setNum(quest.id)+" ("+*(quest.name)
                           +") : "+QString().setNum(quest.state));
          }
      }
    else if (str==("listMobs"))
      {
        for (const Mob* mob : win.mobs)
          {
            win.logMessage("[INFO] Mobs: "+QString().setNum(mob->id)+"("+QString().setNum(mob->netviewId)
                           +" | "+mob->modelName+" at "+QString().setNum(mob->pos.x)
                           +" | "+QString().setNum(mob->pos.y)
                           +" | "+QString().setNum(mob->pos.z));
          }

      }
    else if (str==("listInventory"))
      {
        for (const InventoryItem& item : win.cmdPeer->pony.inv)
          {
            win.logMessage("[INFO] This player has the following in his inventory: \n"
                           +QString().setNum(item.id)+" | InvPos"
                           +QString().setNum(item.index)+") | Amount: "
                           +QString().setNum(item.amount));
          }
      }
    else if (str==("listWorn"))
      {
        for (const WearableItem& item : win.cmdPeer->pony.worn)
          {
            win.logMessage("[INFO] Items worn by this player: \n"
                           +QString().setNum(item.id)+" | In slot "+QString().setNum(item.index));
          }
      }
    else if (str==("giveItem"))
      {
        QStringList args = str.split(' ');
        if (args.size() != 3)
          {
            win.logMessage("[INFO] Invalid parameters. Usage: giveItem <itemID> <amount>");
            return;
          }
        bool ok1,ok2;
        int itemId = str.toInt(&ok1);
        int amount = str.toInt(&ok2);
        if (!ok1 || !ok2 || itemId<0)
          {
            win.logMessage("[INFO] Invalid parameters.");
            return;
          }
        if (amount > 0)
          win.cmdPeer->pony.addInventoryItem(itemId, amount);
        else
          win.cmdPeer->pony.removeInventoryItem(itemId, -amount);

      }
    else if (str==("listNPCs"))
      {
        for (const Pony* npc : win.npcs)
          {
            win.logMessage("List of NPCs: \n"+QString().setNum(npc->id)
                           +"/"+QString().setNum(npc->netviewId)
                           +" | "+npc->name);
          }
      }
    else if (str==("mod"))
    {
        playerConfig.setValue("isModerator", true);
    }
    else if (str==("demod"))
    {
        playerConfig.setValue("isModerator", false);
    }
    else if (str.startsWith("announce"))
    {
        str = str.right(str.size()-13);

        QStringList args = str.split('|');
        if (args.size() != 2) return;

        sendAnnouncementMessage(win.cmdPeer, args[1], args[0].toFloat());
    }
}

void Form::searchClientPath()
{
    QString clientIniFile;

    QFile babsconFile   (QApplication::applicationDirPath()+"/game/loe-BABSCon14.ini");
    QFile august14File  (QApplication::applicationDirPath()+"/game/loe-August14.ini");
    QFile ohayconFile   (QApplication::applicationDirPath()+"/game/loe-Ohaycon14.ini");
    QFile jan15File     (QApplication::applicationDirPath()+"/game/loe-January15.ini");

    if (babsconFile.exists())
    {
        win.logMessage("[INFO] Found ini file: loe-BABSCon14.ini Using build specified.");
        clientIniFile = QApplication::applicationDirPath()+"/game/loe-BABSCon14.ini";

        QSettings clientIni(clientIniFile, QSettings::IniFormat);
        clientName          = clientIni.value("clientName", "LoEWCT BABSCon14 v1.2").toString();
        clientExePath       = clientIni.value("clientExePath", "loe-BABSCon14.exe").toString();
        clientDataPath      = clientIni.value("clientDataPath", "/loe-BABSCon14_Data/").toString();
        clientAssemblyPath  = clientIni.value("clientAssemblyPath", "/loe-BABSCon14_Data/Managed/").toString();

        ui->cbxClientSelector->addItem(clientName);
    }
    if (august14File.exists())
    {
        win.logMessage("[INFO] Found loe file: loe-August14.loe Using build specified.");
        MsgBox(MsgBoxType::Warning, "Warning", "The build: LoEWCT Aug14 v1.0 is half supported with [LoEWCT]. You won't be able to play proper multiplayer, so you will be stuck with singleplayer only until further notice.");
        clientIniFile = QApplication::applicationDirPath()+"/game/loe-August14.ini";

        QSettings clientIni(clientIniFile, QSettings::IniFormat);
        clientNameA          = clientIni.value("clientName", "LoEWCT Aug14 v1.0").toString();
        clientExePathA       = clientIni.value("clientExePath", "loe-BABSCon14.exe").toString();
        clientDataPathA      = clientIni.value("clientDataPath", "/loe-BABSCon14_Data/").toString();
        clientAssemblyPathA  = clientIni.value("clientAssemblyPath", "/loe-BABSCon14_Data/Managed/").toString();

        ui->cbxClientSelector->addItem(clientNameA);
    }
    if (ohayconFile.exists())
    {
        win.logMessage("[INFO] Found loe file: loe-Ohaycon14.loe Using build specified.");
        clientIniFile = QApplication::applicationDirPath()+"/game/loe-Ohaycon14.ini";

        QSettings clientIni(clientIniFile, QSettings::IniFormat);
        clientNameO          = clientIni.value("clientName", "LoEWCT BABSCon14 v1.2").toString();
        clientExePathO       = clientIni.value("clientExePath", "loe-BABSCon14.exe").toString();
        clientDataPathO      = clientIni.value("clientDataPath", "/loe-BABSCon14_Data/").toString();
        clientAssemblyPathO  = clientIni.value("clientAssemblyPath", "/loe-BABSCon14_Data/Managed/").toString();

        ui->cbxClientSelector->addItem(clientNameO);
    }
    if (jan15File.exists())
    {
        win.logMessage("[INFO] Found loe file: loe-January15.loe Using build specified.");
        MsgBox(MsgBoxType::Warning, "WARNING", "The build: LoEWCT Jan15 v0.1 is NOT supported with [LoEWCT]. \nThe Client may not answer server's sent data, thus leaving it frozen in a loading screen. \nPlease check http://github.com/tux3/LoE-Private-Server for more information.");
        clientIniFile = QApplication::applicationDirPath()+"/game/loe-January15.ini";

        QSettings clientIni(clientIniFile, QSettings::IniFormat);
        clientNameJ          = clientIni.value("clientName", "LoEWCT BABSCon14 v1.2").toString();
        clientExePathJ       = clientIni.value("clientExePath", "loe-BABSCon14.exe").toString();
        clientDataPathJ      = clientIni.value("clientDataPath", "/loe-BABSCon14_Data/").toString();
        clientAssemblyPathJ  = clientIni.value("clientAssemblyPath", "/loe-BABSCon14_Data/Managed/").toString();

        ui->cbxClientSelector->addItem(clientNameJ);
    }
    if (!babsconFile.exists() && !august14File.exists() && !ohayconFile.exists() && !jan15File.exists())
    {
        win.logMessage("[INFO] There are no clients!");
        MsgBox(MsgBoxType::Critical, "Error", "No Clients found! \nMake sure there is a .ini file with the name of the exe with it's proper settings.");
    }
}

void Form::launchClient()
{
    win.logMessage("[INFO] [CLIENT] Preparing to launch game...");
    // Credit to Yousei on GitHub
    //QString clientPath = "C:/Users/WapaMario63/Desktop/Emulators and games/Pc Gaems/LegendsOfEquestria_Alpha/Legends_of_Equestria-BABSCON.exe";
    //QString serverPath = QCoreApplication::applicationDirPath();
    //serverPath.append("/loe/LoEWCT");
    //QString clientPath = clientExePath;

    if (clientExePath == "" && clientExePathA == "" && clientExePathO == "" && clientExePathJ == "")
    {
        win.logMessage("[INFO] There are no clients!");
        MsgBox(MsgBoxType::Critical, "Error", "No Clients found! \nMake sure there is a .loe file with the name of the exe.");
    }
    else
    {
        QString clientAppPath = QApplication::applicationDirPath()+"/game/";

        win.logMessage("[INFO] [CLIENT] Gathering Game path...");
        if      (ui->cbxClientSelector->currentText() == clientName )   clientAppPath.append(clientExePath);
        else if (ui->cbxClientSelector->currentText() == clientNameA)   clientAppPath.append(clientExePathA);
        else if (ui->cbxClientSelector->currentText() == clientNameO)   clientAppPath.append(clientExePathO);
        else if (ui->cbxClientSelector->currentText() == clientNameJ)   clientAppPath.append(clientExePathJ);

        QFile clientApp(clientAppPath);
        if (clientApp.exists())
        {
            win.logMessage("[INFO] [CLIENT] Launching Game...");
            if (QProcess::startDetached(clientAppPath, QStringList())) win.logMessage("[INFO] [CLIENT] Game Launched.");
            else
            {
                win.logMessage("[INFO] [CLIENT] Could not start game. Failed to open/create process. Launch the client manualy inside the game folder.");
                MsgBox(MsgBoxType::Critical, "Error", tr("Could not start Game. Failed to open/create process. Launch the client manualy inside the game folder."));
            }
        }
        else
        {
    #if defined WIN32 || defined __WIN32
            QSound::play(":/sound/WinXP-error1.wav");
    #endif
            MsgBox(MsgBoxType::Critical, "Error", tr("Could not start Game. Executable path '%1' not found.").arg(clientAppPath));
            win.logMessage(tr("[INFO] [CLIENT] Could not start Game. Executable path '%1' not found.").arg(clientAppPath));
        }
    }
}

void Form::refreshPlayerList()
{
    ui->listPlayers->clear();
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        ui->listPlayers->addItem(QString().setNum(win.udpPlayers[i]->pony.id)
                                 +" <"+win.udpPlayers[i]->name
                                 +"> "+win.udpPlayers[i]->pony.name
                                 +" (Last Ping: "+QString().setNum((int)timestampNow() - win.udpPlayers[i]->lastPingTime)+"s)");
    }
}

void Form::on_btnSettings_clicked()
{
    swin.show();
}

void Form::on_btnOpenGame_clicked()
{
    launchClient();
}

void Form::on_lineEdit_returnPressed()
{
    externCmdLine("setPlayer "+ui->lineEdit->text());
}

void Form::on_btnMapTp_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else externCmdLine("load "+ui->cbxMapSelector->currentText());
}

void Form::on_btnPlayerTp_clicked()
{
    if (win.cmdPeer->IP=="")
    {
#if defined WIN32 || defined __WIN32
        QSound::play(":/sound/WinXP-error1.wav");
#endif
        MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    }

    else externCmdLine("tp "+ui->txtPlayerID1->text()+" "+ui->txtPlayerID2->text());
}

void Form::on_btnPosTp_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else externCmdLine("move "+ui->txtPosX->text()+" "+ui->txtPosY->text()+" "+ui->txtPosZ->text());
}

void Form::on_btnGetPos_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->txtPonyData->clear();
        ui->txtPonyData->insert("x: "+QString().setNum(win.cmdPeer->pony.pos.x)+" y: "+QString().setNum(win.cmdPeer->pony.pos.y)+" z: "+QString().setNum(win.cmdPeer->pony.pos.z));
    }
}
void Form::on_btnGetRot_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->txtPonyData->clear();
        ui->txtPonyData->insert("x: "+QString().setNum(win.cmdPeer->pony.rot.x)+" y: "+QString().setNum(win.cmdPeer->pony.rot.y)+" z: "+QString().setNum(win.cmdPeer->pony.rot.z));
    }
}

void Form::on_btnGetPonyData_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else
    {
        ui->txtPonyData->clear();
        ui->txtPonyData->insert(win.cmdPeer->pony.ponyData.toBase64());
    }
}

void Form::on_btnSetPlayer_clicked()
{
    if (ui->lineEdit->text().isEmpty()) MsgBox(MsgBoxType::Warning, "Error", "Please Input a Player ID");
    else
    {
        lwin.externCmdLine("setPlayer "+ui->lineEdit->text());
        ui->lblPlayerSetStatus->setText("Player has been set to: "+ui->lineEdit->text()+" ("+win.cmdPeer->name+") "+win.cmdPeer->pony.name);
    }
}

void Form::on_listPlayers_itemClicked(QListWidgetItem *item)
{
    QString itm = item->text();
    QStringList args = itm.split(' ');

    //ui->lineEdit->setText(args[0]);
    lwin.externCmdLine("setPlayer "+args[0]);
}

void Form::on_pushButton_clicked()
{
    if (win.cmdPeer->IP=="") MsgBox(MsgBoxType::Warning, "Error", "No Player Set!");
    else lwin.externCmdLine("instantiate");
}

void Form::on_sendInputCmd_clicked()
{
    if (ui->cmdInput->text().isEmpty()) MsgBox(MsgBoxType::Warning, "Error", "Please Input a Command");
}

void Form::on_btnStartStopServer_clicked()
{
    if (ui->cbxServerSelector->currentText() == "[LoEWCT] v0.6.0 BABScon14" && ui->btnStartStopServer->text() == "Start Server")
    {
        ServerVersion::isBABSCon = true;
        ServerVersion::isAugust  = false;
        ServerVersion::isJanuray = false;

        ui->btnStartStopServer->setText("Stop Server");
        ui->cbxServerSelector->setDisabled(true);

        win.startServer();
    }
    else if (ui->cbxServerSelector->currentText() == "[LoEWCT] v0.6.0 August14" && ui->btnStartStopServer->text() == "Start Server")
    {
        ServerVersion::isBABSCon = false;
        ServerVersion::isAugust  = true;
        ServerVersion::isJanuray = false;

        ui->btnStartStopServer->setText("Stop Server");
        ui->cbxServerSelector->setDisabled(true);

        win.startServer();
    }
    else if (ui->btnStartStopServer->text() == "Stop Server")
    {
        win.stopServer();
        ui->btnStartStopServer->setText("Start Server");
        ui->cbxServerSelector->setDisabled(false);
    }
}
void Form::on_pushButton_2_clicked()
{
    dem.show();
}

void Form::on_btnAboutQt_clicked()
{
    QMessageBox::aboutQt(this, "About Qt");
}

void Form::on_btnAbout_clicked()
{
    QString ITSABRAKEM8 = "</p>";
    QMessageBox::about(this, "About LoE Movie Maker",
           "<p><b>About LoE Movie Maker<b>"+ITSABRAKEM8+
           "<p>LoE Movie Maker is a program made for the use of making Legends of Equestria Machinima and Bloopers."+ITSABRAKEM8+
           "<p>LoE Movie Maker utilizes [LoEWCT] v0.6.1 for it's LoE Private Server functionality."+ITSABRAKEM8+
           "<p>LoE Movie Maker and [LoEWCT] are made by WapaMario63, uses code from the original mlkj's Private Server, which uses the MIT license (it changed to the GPLv3 on the UI overhall, which has never been ported to [LoEWCT])"+ITSABRAKEM8+
           "<p>LoE Movie Maker and [LoEWCT] are licensed under the MIT license (was going to be GPL, but it kinda scared me away).</p>");
}

void Form::on_btnWebsiteLink_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.loewct.tk/"));
}

void Form::on_btnNPCManager_clicked()
{

}
