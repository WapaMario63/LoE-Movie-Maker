#include "widget.h"
#include "ui_widget.h"
#include "message.h"
#include "utils.h"
#include "serialize.h"
#include "mob.h"
#include "chat_widget.h"
#include "form.h"

#include <QtMath>

QSettings loeWctConfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
QString serverPrefix = loeWctConfig.value("serverPrefix", "[LoEWCT]").toString();

void Form::cmdStopServer()
{
    win.logMessage(QString("[INFO] Stopping server..."));
    win.stopServer();
    win.logStatusMessage(QString("[INFO] Server Stopped"));

    win.logMessage(QString("[INFO] You may close the window now."));
}
void Form::cmdStartServer()
{
    if (!win.udpSocket)
      {
        win.logMessage(QString("[INFO] Starting Server..."));
        win.startServer();
      }
}
void Form::cmdShowHelp()
{
    win.logMessage(QString("[INFO] Here is a list of normal commands: clear, stop, help, listTopPlayers, tp <player> <player>, setPlayer, listPlayers, move <x> <y> <z>, load <map>, getPos, getRot, error <message>, kick. \nFor debug commands use helpDebug."));
}
void Form::cmdShowDebugHelp()
{
    win.logMessage(QString("[INFO] Debug commands, don't mess with these much: listVortexes, sync, dbgStressLoad, getPonyData, sendPonies, setPlayerId, reloadNpc, sendPonyData, setStat, setMaxStat, instantiate, beginDialog, endDialog, setDialogMsg, setDialogOptions"));
}
void Form::cmdListTcpPlayers()
{
    for (int i=0; i<win.tcpPlayers.size(); i++)
    {
        Player* p = win.tcpPlayers[i];
        win.logMessage(p->name+" "+p->IP+":"+QString().setNum(p->port));
    }
}

void Form::cmdSetPlayer()
{
    win.cmdPeer = win.udpPlayers[0];
    QString peerName = win.cmdPeer->IP + ":" + QString().setNum(win.cmdPeer->port);
    win.logMessage(QString("[INFO] UDP: Player has been set to ").append(peerName));
}

void Form::cmdSetPlayer(quint16 id)
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        if (VAR_ALL_PLAYERS->pony.id == id)
        {
            win.cmdPeer = Player::findPlayer(win.udpPlayers, VAR_ALL_PLAYERS->IP, VAR_ALL_PLAYERS->port);
            win.logMessage(QString("[INFO] UDP: Player set to "+VAR_ALL_PLAYERS->pony.name));
        }
        else
        {
            win.logMessage(QString("[ERROR] UDP: Player not found (ID ").append(id).append(")"));
        }
    }
}

void Form::cmdSetPlayer(QString IP, quint16 port)
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        if (VAR_ALL_PLAYERS->IP == IP && VAR_ALL_PLAYERS->port)
        {
            win.cmdPeer = Player::findPlayer(win.udpPlayers, IP, port);
            win.logMessage(QString("[INFO] UDP: Player set to "+VAR_ALL_PLAYERS->pony.name));
        }
        else
        {
            win.logMessage(QString("[ERROR] UDP: Player not found (").append(IP).append(":").append(port).append(")"));
        }
    }
}

void Form::cmdListPlayers()
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        for (int i=0; i<win.udpPlayers.size();i++)
            win.logMessage(QString().setNum(VAR_ALL_PLAYERS->pony.id)
                           //+"("+QString().setNum(VAR_ALL_PLAYERS->pony.netviewId)+")"
                           +" | "+VAR_ALL_PLAYERS->pony.name
                           +" | "+VAR_ALL_PLAYERS->IP
                           +":"+QString().setNum(VAR_ALL_PLAYERS->port)
                           +" | "+QString().setNum((int)timestampNow()-VAR_ALL_PLAYERS->lastPingTime)+"s");
    }

    /*for (const Player& player : win.udpPlayers)
    {
        win.logMessage(QString().setNum(player->pony.id)
                       //+"("+QString().setNum(VAR_ALL_PLAYERS->pony.netviewId)+")"
                       +" | "+player->pony.name
                       +" | "+player->IP
                       +":"+QString().setNum(player->port)
                       +" | "+QString().setNum((int)timestampNow()-player->lastPingTime)+"s");
    }*/
}

void Form::cmdListPlayers(QString scenename)
{
    Scene* scene = findScene(scenename);
    if (scene->name.isEmpty())
        win.logMessage("[ERROR] Scene Not found");
    else
    {
        for (int j=0; j<scene->players.size(); j++)
        {
            win.logMessage(win.udpPlayers[j]->IP
                           +":"+QString().setNum(win.udpPlayers[j]->port)
                           +" "+QString().setNum((int)timestampNow()-win.udpPlayers[j]->lastPingTime)+"s");
        }

    }
}

void Form::cmdListVortexes()
{
    for (int i=0; i<win.scenes.size(); i++)
    {
        win.logMessage("[INFO] Scene "+win.scenes[i].name);
        for (int j=0; j<win.scenes[i].vortexes.size(); j++)
            win.logMessage("[INFO] Vortex "+QString().setNum(win.scenes[i].vortexes[j].id)
                           +" to "+win.scenes[i].vortexes[j].destName+" "
                           +QString().setNum(win.scenes[i].vortexes[j].destPos.x)+" "
                           +QString().setNum(win.scenes[i].vortexes[j].destPos.y)+" "
                           +QString().setNum(win.scenes[i].vortexes[j].destPos.z));
    }
}

void Form::cmdSync()
{
    win.logMessage("[INFO] UDP: Syncing players manually");
    win.sync.doSync();
}

void Form::cmdDebugStressLoad()
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        sendLoadSceneRPC(VAR_ALL_PLAYERS, "gemmines");
    }
}

void Form::cmdTpPlayerToPlayer(quint16 sourceId, quint16 destId)
{
    Player* sourcePlayer;
    FUNCTION_SELECT_ALL_PLAYERS
    {
        if (VAR_ALL_PLAYERS->pony.id == sourceId)
        {
            sourcePlayer = VAR_ALL_PLAYERS;
            break; // We break here and start another loop because we may have skipped the destId during the loop!
        }
        else
        {
            win.logStatusMessage("[INFO] Error: Source player is not on the server or doesn't exist!");
            return;
        }
    }

    FUNCTION_SELECT_ALL_PLAYERS
    {
        if (VAR_ALL_PLAYERS->pony.id == destId)
        {
            win.logMessage(QString("[INFO] UDP: Teleporting "+sourcePlayer->pony.name+" to "+VAR_ALL_PLAYERS->pony.name));
            if (VAR_ALL_PLAYERS->pony.sceneName.toLower() != sourcePlayer->pony.sceneName.toLower())
            {
                sendLoadSceneRPC(sourcePlayer, VAR_ALL_PLAYERS->pony.sceneName, VAR_ALL_PLAYERS->pony.pos);
            }
            else
            {
                sendMove(sourcePlayer, VAR_ALL_PLAYERS->pony.pos.x, VAR_ALL_PLAYERS->pony.pos.y, VAR_ALL_PLAYERS->pony.pos.z);
            }
            return;
        }
        else
        {
            win.logStatusMessage("[INFO] Error: Destination player is not on the server or doesn't exist!");
            return;
        }
    }
}

void Form::cmdServerSay(QString msg)
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        sendChatMessage(VAR_ALL_PLAYERS, "<span color=\"cyan\">"+msg+"</span>", "[SERVER]", ChatGeneral);
    }
}

void Form::cmdAnnouncement(QString msg, float duration)
{
    FUNCTION_SELECT_ALL_PLAYERS
    {
        sendAnnouncementMessage(VAR_ALL_PLAYERS, msg, duration);
    }
}
//------------
void Form::cmdAnnouncePlayer(Player *player, QString msg, float duration)
{
    sendAnnouncementMessage(player, msg, duration);
}

void Form::cmdKickPlayer(Player *player, QString reason)
{
    QByteArray data(1,0);
    data[0] = 0x7F;
    data += stringToData(reason);

    win.logMessage("[INFO] Kicking Player...");
    sendMessage(player, MsgDisconnect, "Kicked by Admin \n\nReason \n-------------------------------\n"+data);

    FUNCTION_SELECT_ALL_PLAYERS
    {
        sendChatMessage(VAR_ALL_PLAYERS, "Kicked <span color=\"cyan\">"+player->pony.name+" ("+player->name+")</span> for <span color\"orange\">"+reason+"</span>", serverPrefix, ChatGeneral);
    }
}

void Form::cmdLoadScene(Player *player, QString scenename)
{
    sendLoadSceneRPC(player, scenename);
}

void Form::cmdGetPosition(Player *player)
{
    win.logMessage(QString("Pos : x=") + QString().setNum(player->pony.pos.x)
               + ", y=" + QString().setNum(player->pony.pos.y)
               + ", z=" + QString().setNum(player->pony.pos.z));
}
UVector Form::cmdGetPositionU(Player *player) { return player->pony.pos; }

void Form::cmdGetRotation(Player *player)
{
    win.logMessage(QString("Rot : x=") + QString().setNum(player->pony.rot.x)
               + ", y=" + QString().setNum(player->pony.rot.y)
               + ", z=" + QString().setNum(player->pony.rot.z)
               + ", w=" + QString().setNum(player->pony.rot.w));
}
UQuaternion Form::cmdGetRotationU(Player *player) { return player->pony.rot; }

void Form::cmdGetPonyData(Player *player)
{
    win.logMessage("[INFO] ponyData for this player: "+win.cmdPeer->pony.ponyData.toBase64());
}
QByteArray Form::cmdGetPonyDataD(Player *player) { return player->pony.ponyData; }

void Form::cmdSendPonies(Player *player)
{
    win.logMessage("[INFO] Sending ponies to this player.");
    sendPonies(player);
}

void Form::cmdSendUtils3(Player *player)
{
    win.logMessage("[INFO] UDP: Sending Utils3 request");
    QByteArray data(1,3);
    sendMessage(player,MsgUserReliableOrdered6,data);
}

void Form::cmdSetPlayerId(Player *player, unsigned id)
{
    QByteArray data(3,4);

    win.logMessage("[INFO] UDP: Sending setPlayerId request");
    data[1] = (quint8)(id&0xFF);
    data[2] = (quint8)((id >> 8)&0xFF);
    sendMessage(player, MsgUserReliableOrdered6, data);
}

void Form::cmdReloadNpcs(Player *player, QString npcName)
{
    win.logMessage("[INFO] Reloading the NPC Database...");

    Pony* npc = nullptr;

    for (int i=0; i<win.npcs.size(); i++)
    {
        if (win.npcs[i]->name == npcName)
        {
            npc = win.npcs[i];
            break;
        }
    }

    if (npc != nullptr)
    {
        // Reload the entire NPC database
        win.npcs.clear();
        win.quests.clear();
        unsigned quests = 0;

        QDir npcsDir("data/npcs/");
        QStringList files = npcsDir.entryList(QDir::Files);
        for (int i=0; i<files.size(); i++, quests++) // For each vortex file
        {
            Quest *quest = new Quest("data/npcs/"+files[i], nullptr);
            win.quests << *quest;
            win.npcs << quest->npc;
        }
        win.logMessage("[INFO] Reloaded "+QString().setNum(quests)+" quests/npcs.");

        // Resend if needed
        if (npc->sceneName.toLower() == player->pony.sceneName.toLower())
        {
            sendNetviewRemove(player, npc->netviewId);
            sendNetviewInstantiate(npc, win.cmdPeer);
        }
    }
    else
    {
        win.logMessage("[INFO] NPC not found");
    }
}

void Form::cmdSendPonyData(Player *player, QString ponyData)
{
    QByteArray pData = stringToData(ponyData);

    QByteArray data(3, 0xC8);
    data[0] = (quint8)(player->pony.netviewId&0xFF);
    data[1] = (quint8)((player->pony.netviewId>>8)&0xFF);
    data += pData;

    sendMessage(player, MsgUserReliableOrdered18, data);
}
void Form::cmdSendPonyData(Player *player, QByteArray ponyData)
{
    QByteArray data(3, 0xC8);
    data[0] = (quint8)(player->pony.netviewId&0xFF);
    data[1] = (quint8)((player->pony.netviewId>>8)&0xFF);
    data += ponyData;

    sendMessage(player, MsgUserReliableOrdered18, data);
}

void Form::cmdSetStat(Player *player, quint8 statId, float statValue)
{
    sendSetStatRPC(player, statId, statValue);
}

void Form::cmdSetMaxStat(Player *player, quint8 statId, float statValue)
{
    sendSetMaxStatRPC(player, statId, statValue);
}

void Form::cmdInstantiate(Player *player)
{
    win.logMessage("[INFO] UDP: Cloning/instantiating this player.");
    sendNetviewInstantiate(player);
}

void Form::cmdInstantiate(Player *player, unsigned viewId, unsigned ownerId, float posx, float posy, float posz, float rotx, float roty, float rotz, float rotw)
{
    const unsigned key = qFloor((qrand()*65535)+200); // This is the key, which no one knows what it was for, it was never documented, so, we will just send a random number
    QByteArray data(1,1);

    data += key;

    QByteArray params(4,0);
    params[0] = (quint8)(viewId&0xFF);
    params[1] = (quint8)((viewId >> 8)&0xFF);
    params[2] = (quint8)(ownerId&0xFF);
    params[3] = (quint8)((ownerId >> 8)&0xFF);
    data += params;

    data += floatToData(posx);
    data += floatToData(posy);
    data += floatToData(posz);

    data += floatToData(rotx);
    data += floatToData(roty);
    data += floatToData(rotz);
    data += floatToData(rotw);

    win.logMessage(QString("[INFO] UDP: Instantiating player with advanced info ").append(key));
    sendMessage(player, MsgUserReliableOrdered6, data);
}

void Form::cmdBeginDialog(Player *player)
{
    QByteArray data(1,0);
    data[0] = 11; // Request number

    sendMessage(player,MsgUserReliableOrdered4, data);
}
void Form::cmdEndDialog(Player *player)
{
    QByteArray data(1,0);
    data[0] = 13; // Request number

    sendMessage(player,MsgUserReliableOrdered4, data);
}
void Form::cmdSetDialogMsg(Player *player, QString msg)
{
    QStringList args = msg.split(" ", QString::SkipEmptyParts);

    QByteArray data(1,0);
    data[0] = 0x11; // Request number
    data += stringToData(args[0]);
    data += stringToData(args[1]);
    data += (char)0; // emoticon
    data += (char)0; // emoticon

    sendMessage(player,MsgUserReliableOrdered4, data);
}
void Form::cmdSetDialogOptions(Player *player, QString options)
{
    QStringList args = options.split(" ", QString::SkipEmptyParts);
    sendDialogOptions(player, args);
}

void Form::cmdMove(Player *player, float x, float y, float z)
{
    sendMove(player, x, y, z);
}

void Form::cmdErrorMessage(Player *player, QString msg)
{
    QByteArray data(1,0);
    data[0] = 0x7f; // Request number

    data += stringToData(msg);

    sendMessage(player,MsgUserReliableOrdered4, data);
}

void Form::cmdListQuests(Player *player)
{
    for (const Quest& quest : player->pony.quests)
    {
        win.logMessage("[INFO] Quest "+QString().setNum(quest.id)+" ("+*(quest.name)
                       +") : "+QString().setNum(quest.state));
    }
}

void Form::cmdListMobs()
{
    for (const Mob* mob : win.mobs)
    {
        win.logMessage("[INFO] Mobs: "+QString().setNum(mob->id)+"("+QString().setNum(mob->netviewId)
                       +" | "+mob->modelName+" at "+QString().setNum(mob->pos.x)
                       +" | "+QString().setNum(mob->pos.y)
                       +" | "+QString().setNum(mob->pos.z));
    }
}

void Form::cmdListInventory(Player *player)
{
    for (const InventoryItem& item : player->pony.inv)
    {
        win.logMessage("[INFO] This player has the following in his inventory: \n"
                       +QString().setNum(item.id)+" | InvPos"
                       +QString().setNum(item.index)+") | Amount: "
                       +QString().setNum(item.amount));
    }
}

void Form::cmdListWornItems(Player *player)
{
    for (const WearableItem& item : player->pony.worn)
    {
        win.logMessage("[INFO] Items worn by this player: \n"
                       +QString().setNum(item.id)+" | In slot "+QString().setNum(item.index));
    }
}

void Form::cmdListNpcs()
{
    for (const Pony* npc : win.npcs)
    {
        win.logMessage("List of NPCs: \n"+QString().setNum(npc->id)
                       +"/"+QString().setNum(npc->netviewId)
                       +" | "+npc->name);
    }
}

void Form::cmdGiveItem(Player *player, int itemId, int amount)
{
    if (amount > 0)
    {
        player->pony.addInventoryItem(itemId, amount);
    }
    else
    {
        player->pony.addInventoryItem(itemId, -amount);
    }
}

// Processes the commands entered directly in the server, not the chat messages

// This file is entirely commented for LoE Movie Maker
// A Copy of the entirety of this file is moved to form.cpp

/*void Widget::externCmdLine(QString str)
{
    //ui->cmdLine->clear();
    //ui->cmdLine->insert(str);
    //ui->cmdLine->returnPressed();
    //ui->cmdLine->clear();
    //cin >> str;
    lwin.inputString = str;
}

void Widget::sendCmdLine()
{
    QSettings loeWctConfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
    QSettings playerConfig(PLAYERSPATH+cmdPeer->name.toLatin1()+"/player.ini", QSettings::IniFormat);
    QString serverPrefix = loeWctConfig.value("serverPrefix", "[LoEWCT]").toString();

    if (!enableGameServer)
    {
        logMessage("This is not a game server, commands are disabled");
        return;
    }

    //QString str = ui->cmdLine->text();
    //QString str = cin.readLine();

    if (str == "clear")
    {
        ui->log->clear();
        return;
    }
    else if (str == "stop")
    {
        logMessage(QString("[INFO] Stopping server..."));
        stopServer();
        logStatusMessage(QString("[INFO] Server Stopped"));

        logMessage(QString("[INFO] You may close the window now."));
    }
    else if (str == "start")
      {
        if (!udpSocket)
          {
            logMessage(QString("[INFO] Starting Server..."));
            startServer();
          }
      }
    else if (str == "help")
      {
        logMessage(QString("[INFO] Here is a list of normal commands: clear, stop, help, listTopPlayers, tp <player> <player>, setPlayer, listPlayers, move <x> <y> <z>, load <map>, getPos, getRot, error <message>, kick. \nFor debug commands use helpDebug."));
      }
    else if (str == "helpDebug")
      {
        logMessage(QString("[INFO] Debug commands, don't mess with these much: listVortexes, sync, dbgStressLoad, getPonyData, sendPonies, setPlayerId, reloadNpc, sendPonyData, setStat, setMaxStat, instantiate, beginDialog, endDialog, setDialogMsg, setDialogOptions"));
      }
    else if (str == "listTcpPlayers")
    {
        for (int i=0; i<tcpPlayers.size(); i++)
        {
            Player* p = tcpPlayers[i];
            logMessage(p->name+" "+p->IP+":"+QString().setNum(p->port));
        }
        return;
    }
    else if (str.startsWith("setPlayer"))
    {
        if (udpPlayers.size() == 1)
        {
            cmdPeer = udpPlayers[0];
            QString peerName = cmdPeer->IP + ":" + QString().setNum(cmdPeer->port);
            logMessage(QString("[INFO] UDP: Player has been set to ").append(peerName));
            return;
        }

        str = str.right(str.size()-10);
        QStringList args = str.split(':');
        bool ok;
        if (args.size() != 2)
        {
            if (args.size() != 1)
            {
                logMessage("[INFO] UDP: setPlayer takes a Player's ID or an IP:port combination from a player");
                return;
            }
            quint16 id = args[0].toUInt(&ok);
            if (!ok)
            {
                logMessage("[INFO] UDP: setPlayer takes a player ID as a function/argument");
                return;
            }
            for (int i=0; i<udpPlayers.size();i++)
            {
                if (udpPlayers[i]->pony.id == id)
                {
                    cmdPeer = Player::findPlayer(udpPlayers,udpPlayers[i]->IP, udpPlayers[i]->port);
                    logMessage(QString("[INFO] UDP: Player set to "+udpPlayers[i]->pony.name));
                    return;
                }
            }
            logMessage(QString("[ERROR] UDP: Player not found (ID ").append(args[0]).append(") :/"));
            return;
        }

        quint16 port = args[1].toUInt(&ok);
        if (!ok)
        {
            logMessage("[INFO] UDP: setPlayer takes a port as a function/argument");
            return;
        }

        cmdPeer = Player::findPlayer(udpPlayers,args[0], port);
        if (cmdPeer->IP!="")
            logMessage(QString("[INFO] UDP: Player set to ").append(str));
        else
            logMessage(QString("[ERROR] UDP: Player not found (").append(str).append(")"));
        return;
    }
    else if (str.startsWith("listPlayers"))
    {
        if (str.size()<=12)
        {
            for (int i=0; i<win.udpPlayers.size();i++)
                win.logMessage(QString().setNum(VAR_ALL_PLAYERS->pony.id)
                               //+"("+QString().setNum(VAR_ALL_PLAYERS->pony.netviewId)+")"
                               +" | "+VAR_ALL_PLAYERS->pony.name
                               +" | "+VAR_ALL_PLAYERS->IP
                               +":"+QString().setNum(VAR_ALL_PLAYERS->port)
                               +" | "+QString().setNum((int)timestampNow()-VAR_ALL_PLAYERS->lastPingTime)+"s");
            return;
        }
        str = str.right(str.size()-10);
        Scene* scene = findScene(str);
        if (scene->name.isEmpty())
            win.logMessage("[INFO] There are no players on or a scene was not found");
        else
            for (int i=0; i<scene->players.size();i++)
                win.logMessage(VAR_ALL_PLAYERS->IP
                               +":"+QString().setNum(VAR_ALL_PLAYERS->port)
                               +" "+QString().setNum((int)timestampNow()-VAR_ALL_PLAYERS->lastPingTime)+"s");
        return;
    }
    else if (str.startsWith("listVortexes"))
    {
        for (int i=0; i<scenes.size(); i++)
        {
            win.logMessage("[INFO] Scene "+scenes[i].name);
            for (int j=0; j<scenes[i].vortexes.size(); j++)
                win.logMessage("[INFO] Vortex "+QString().setNum(scenes[i].vortexes[j].id)
                               +" to "+scenes[i].vortexes[j].destName+" "
                               +QString().setNum(scenes[i].vortexes[j].destPos.x)+" "
                               +QString().setNum(scenes[i].vortexes[j].destPos.y)+" "
                               +QString().setNum(scenes[i].vortexes[j].destPos.z));
        }
        return;
    }
    else if (str.startsWith("sync"))
    {
        win.logMessage("[INFO] UDP: Syncing players manually");
        sync.doSync();
        return;
    }
    // DEBUG global commands from now on
    else if (str==("dbgStressLoad"))
    {
        // Send all the players to the GemMines at the same time
        for (int i=0; i<udpPlayers.size(); i++)
            sendLoadSceneRPC(udpPlayers[i], "GemMines");
        return;
    }
    else if (str.startsWith("dbgStressLoad"))
    {
        str = str.mid(14);
        // Send all the players to the given scene at the same time
        for (int i=0; i<udpPlayers.size(); i++)
            sendLoadSceneRPC(udpPlayers[i], str);
        return;
    }
    else if (str.startsWith("tp"))
    {
        str = str.right(str.size()-3);
        QStringList args = str.split(' ');
        if (args.size() != 2)
        {
            logStatusMessage("[INFO] Error: Usage is tp <playerID to move> <destination playerID>");
            return;
        }
        bool ok;
        bool ok1;
        bool ok2 = false;
        quint16 sourceID = args[0].toUInt(&ok);
        quint16 destID = args[1].toUInt(&ok1);
        Player* sourcePeer;
        if (!ok && !ok1)
        {
            logStatusMessage("[INFO] Error: Usage is tp <playerID to move> <destination playerID>");
            return;
        }
        for (int i=0; i<udpPlayers.size();i++)
        {
            if (udpPlayers[i]->pony.id == sourceID)
            {
                sourcePeer = udpPlayers[i];
                ok2 = true;
                break;
            }
        }
        if (!ok2)
        {
            logStatusMessage("[INFO] Error: Source player is not on the server or doesn't exist!");
            return;
        }
        for (int i=0; i<udpPlayers.size();i++)
        {
            if (udpPlayers[i]->pony.id == destID)
            {
                logMessage(QString("[INFO] UDP: Teleported "+sourcePeer->pony.name+" to "+udpPlayers[i]->pony.name));
                if (udpPlayers[i]->pony.sceneName.toLower() != sourcePeer->pony.sceneName.toLower())
                {
                    sendLoadSceneRPC(sourcePeer, udpPlayers[i]->pony.sceneName, udpPlayers[i]->pony.pos);
                }
                else
                {
                    sendMove(sourcePeer, udpPlayers[i]->pony.pos.x, udpPlayers[i]->pony.pos.y, udpPlayers[i]->pony.pos.z);
                }
                return;
            }
        }
        logMessage("[INFO] Error: Source player is not on the server or doesn't exist!");
    }
    else if (str.startsWith("say"))
    {
        str = str.right(str.size()-4);
        for (int i=0; i<udpPlayers.size(); i++)
          {
            sendChatMessage(udpPlayers[i], "<span color=\"cyan\">"+str+"</span>", "[SERVER]", ChatGeneral);
          }
        //cwin.logChatMessage("<SERVER> "+str);
    }
    if (cmdPeer->IP=="")
    {
        logMessage("[INFO] Select a player first with setPlayer or look for some with listPlayers");
        return;
    }
    else // Refresh peer info
    {
        cmdPeer = Player::findPlayer(udpPlayers,cmdPeer->IP, cmdPeer->port);
        if (cmdPeer->IP=="")
        {
            logMessage(QString("UDP: Player not found"));
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
            logMessage("[ERROR] kick has these arguments: kick <reason>. You NEED to provide a reason of why you kicked the player.");
            return;
        }

        data += stringToData(str);

        logMessage(QString("[INFO] Kicking the player..."));
        sendMessage(cmdPeer,MsgDisconnect, "Kicked by Admin \n\nReason \n-------------------------------\n"+data);
        Player::disconnectPlayerCleanup(cmdPeer); // Save game and remove the player

        for (int i=0; i<udpPlayers.size(); i++)
          {
            sendChatMessage(udpPlayers[i], "Kicked <span color=\"cyan\">"+cmdPeer->pony.name+" ("+cmdPeer->name+")</span> for <span color\"orange\">"+str+"</span>", serverPrefix, ChatGeneral);
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
            logMessage("[ERROR] ban has these arguments: ban <reason>. You NEED to provide a reason of why you banned the player.");
            return;
        }

        data += stringToData(str);

        logMessage(QString("[INFO] Banning the player..."));
        sendMessage(cmdPeer,MsgDisconnect, "Banned by Admin \n\nReason \n-------------------------------\n"+data);
        Player::disconnectPlayerCleanup(cmdPeer); // Save game and remove the player

        for (int i=0; i<udpPlayers.size(); i++)
          {
            sendChatMessage(udpPlayers[i], "Banned <span color=\"cyan\">"+cmdPeer->pony.name+" ("+cmdPeer->name+")</span> for <span color\"orange\">"+str+"</span>", serverPrefix, ChatGeneral);
          }

        // Ban session
        playerConfig.setValue("isBanned", true);
        playerConfig.setValue("banReason", str);
        playerConfig.setValue("ip", cmdPeer->IP);

        // Yes its that short and crappy, I made something more complicated, but didn't work.
    }
    else if (str.startsWith("load"))
    {
        str = str.mid(5);
        sendLoadSceneRPC(cmdPeer, str);
    }
    else if (str.startsWith("getPos"))
    {
        logMessage(QString("Pos : x=") + QString().setNum(cmdPeer->pony.pos.x)
                   + ", y=" + QString().setNum(cmdPeer->pony.pos.y)
                   + ", z=" + QString().setNum(cmdPeer->pony.pos.z));
    }
    else if (str.startsWith("getRot"))
    {
        logMessage(QString("Rot : x=") + QString().setNum(cmdPeer->pony.rot.x)
                   + ", y=" + QString().setNum(cmdPeer->pony.rot.y)
                   + ", z=" + QString().setNum(cmdPeer->pony.rot.z)
                   + ", w=" + QString().setNum(cmdPeer->pony.rot.w));
    }
    else if (str.startsWith("getPonyData"))
    {
        logMessage("[INFO] ponyData for this player: "+cmdPeer->pony.ponyData.toBase64());
    }
    else if (str.startsWith("sendPonies"))
    {
        logMessage("[INFO] Sending ponies to this player.");
        sendPonies(cmdPeer);
    }
    else if (str.startsWith("sendUtils3"))
    {
        logMessage("[INFO] UDP: Sending Utils3 request");
        QByteArray data(1,3);
        sendMessage(cmdPeer,MsgUserReliableOrdered6,data);
    }
    else if (str.startsWith("setPlayerId"))
    {
        str = str.right(str.size()-12);
        QByteArray data(3,4);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
        {
            logMessage("[INFO] UDP: Sending setPlayerId request");
            data[1]=(quint8)(id&0xFF);
            data[2]=(quint8)((id >> 8)&0xFF);
            sendMessage(cmdPeer,MsgUserReliableOrdered6,data);
        }
        else
            logStatusMessage("Error : Player ID is a number");
    }
    else if (str.startsWith("reloadNpc"))
    {
        logMessage("[INFO] Reloading the NPC Database...");
        str = str.mid(10);
        Pony* npc = NULL;
        for (int i=0; i<npcs.size(); i++)
            if (npcs[i]->name == str)
            {
                npc = npcs[i];
                break;
            }
        if (npc != NULL)
        {
            // Reload the NPCs from the DB
            npcs.clear();
            quests.clear();
            unsigned nQuests = 0;
            QDir npcsDir("data/npcs/");
            QStringList files = npcsDir.entryList(QDir::Files);
            for (int i=0; i<files.size(); i++, nQuests++) // For each vortex file
            {
                Quest *quest = new Quest("data/npcs/"+files[i], NULL);
                quests << *quest;
                npcs << quest->npc;
            }
            logMessage("[INFO] Loaded "+QString().setNum(nQuests)+" quests/npcs.");

            // Resend the NPC if needed
            if (npc->sceneName.toLower() == cmdPeer->pony.sceneName.toLower())
            {
                sendNetviewRemove(cmdPeer, npc->netviewId);
                sendNetviewInstantiate(npc, cmdPeer);
            }
        }
        else
            logMessage("[INFO] NPC not found");
    }
    else if (str.startsWith("removekill"))
      {
        str = str.right(str.size()-11);
        QByteArray data(4,2);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
          {
            logMessage("[INFO] UDP: Sending remove request with kill reason code");
            data[1]=id;
            data[2]=id >> 8;
            data[3]=NetviewRemoveReasonKill;
            sendMessage(cmdPeer, MsgUserReliableOrdered6, data);
          }
        else
          logStatusMessage("[ERROR] Removekill needs the id of the view to remove");
      }
    else if (str.startsWith("remove"))
    {
        str = str.right(str.size()-7);
        QByteArray data(3,2);
        bool ok;
        unsigned id = str.toUInt(&ok);
        if (ok)
        {
            logMessage("[INFO] UDP: Sending remove request");
            data[1]=id;
            data[2]=id >> 8;
            sendMessage(cmdPeer,MsgUserReliableOrdered6,data);
        }
        else
            logStatusMessage("[INFO] Error : Remove needs the id of the view to remove");
    }
    else if (str.startsWith("sendPonyData"))
    {
        QByteArray data(3,0xC8);
        data[0] = (quint8)(cmdPeer->pony.netviewId&0xFF);
        data[1] = (quint8)((cmdPeer->pony.netviewId>>8)&0xFF);
        data += cmdPeer->pony.ponyData;
        sendMessage(cmdPeer, MsgUserReliableOrdered18, data);
        return;
    }
    else if (str.startsWith("setStat"))
    {
        str = str.right(str.size()-8);
        QStringList args = str.split(' ');
        if (args.size() != 2)
        {
            logStatusMessage("[INFO] Error : usage is setState StatID StatValue");
            return;
        }
        bool ok,ok2;
        quint8 statID = args[0].toInt(&ok);
        float statValue = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logStatusMessage("[INFO] Error : usage is setState StatID StatValue");
            return;
        }
        sendSetStatRPC(cmdPeer, statID, statValue);
    }
    else if (str.startsWith("setMaxStat"))
    {
        str = str.right(str.size()-11);
        QStringList args = str.split(' ');
        if (args.size() != 2)
        {
            logStatusMessage("[INFO] Error : usage is setMaxStat StatID StatValue");
            return;
        }
        bool ok,ok2;
        quint8 statID = args[0].toInt(&ok);
        float statValue = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logStatusMessage("[INFO] Error : usage is setMaxState StatID StatValue");
            return;
        }
        sendSetMaxStatRPC(cmdPeer, statID, statValue);
    }
    else if (str.startsWith("instantiate"))
    {
        if (str == "instantiate")
        {
            logMessage("[INFO] UDP: Cloning/instantiating this player.");
            sendNetviewInstantiate(cmdPeer);
            return;
        }

        QByteArray data(1,1);
        str = str.right(str.size()-12);
        QStringList args = str.split(' ');

        if (args.size() != 3 && args.size() != 6 && args.size() != 10)
        {
            logStatusMessage(QString("[INFO] Error : Instantiate takes 0,3,6 or 10 arguments").append(str));
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
            logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
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
                logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
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
                logStatusMessage(QString("[INFO] Error : instantiate key viewId ownerId x1 y1 z1 x2 y2 z2 w2"));
                return;
            }
        }
        data+=floatToData(x2);
        data+=floatToData(y2);
        data+=floatToData(z2);
        data+=floatToData(w2);

        logMessage(QString("[INFO] UDP: Instantiating player").append(args[0]));
        sendMessage(cmdPeer,MsgUserReliableOrdered6,data);
    }
    else if (str.startsWith("beginDialog"))
    {
        QByteArray data(1,0);
        data[0] = 11; // Request number

        sendMessage(cmdPeer,MsgUserReliableOrdered4, data);
    }
    else if (str.startsWith("endDialog"))
    {
        QByteArray data(1,0);
        data[0] = 13; // Request number

        sendMessage(cmdPeer,MsgUserReliableOrdered4, data);
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

            sendMessage(cmdPeer,MsgUserReliableOrdered4, data);
        }
    }
    else if (str.startsWith("setDialogOptions"))
    {
        str = str.right(str.size()-17);
        QStringList args = str.split(" ", QString::SkipEmptyParts);
        sendDialogOptions(cmdPeer, args);
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

        sendMove(cmdPeer, coords[0].toFloat(), coords[1].toFloat(), coords[2].toFloat());
    }
    else if (str.startsWith("error"))
    {
        str = str.right(str.size()-6);
        QByteArray data(1,0);
        data[0] = 0x7f; // Request number

        data += stringToData(str);

        sendMessage(cmdPeer,MsgUserReliableOrdered4, data);
    }
    else if (str==("listQuests"))
      {
        for (const Quest& quest : cmdPeer->pony.quests)
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
        for (const InventoryItem& item : cmdPeer->pony.inv)
          {
            win.logMessage("[INFO] This player has the following in his inventory: \n"
                           +QString().setNum(item.id)+" | InvPos"
                           +QString().setNum(item.index)+") | Amount: "
                           +QString().setNum(item.amount));
          }
      }
    else if (str==("listWorn"))
      {
        for (const WearableItem& item : cmdPeer->pony.worn)
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
          cmdPeer->pony.addInventoryItem(itemId, amount);
        else
          cmdPeer->pony.removeInventoryItem(itemId, -amount);

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
    else if (str.startsWith("announcement"))
    {
        str = str.right(str.size()-13);

        QStringList args = str.split('|');
        if (args.size() != 2) return;

        sendAnnouncementMessage(cmdPeer, args[1], args[0].toFloat());
    }
}*/
