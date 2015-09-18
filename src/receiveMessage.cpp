#include "message.h"
#include "character.h"
#include "widget.h"
#include "sync.h"
#include "utils.h"
#include "serialize.h"
#include "receiveack.h"
#include "receiveChatMessage.h"
#include "mob.h"
#include "packetloss.h"
#include "skill.h"
#include "settings.h"

#define DEBUG_LOG false

void receiveMessage(Player* player)
{
    //bool debugLogEnable = loeWctConfig.value("debugLogEnable", false).toBool();
    QSettings loewctconfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
    QString serverPrefix = loewctconfig.value("serverPrefix", "[LoEWCT]").toString();
    bool enablePVP = loewctconfig.value("pvp", true).toBool();
    bool playerJoined = loewctconfig.value("playerJoinedMsg", true).toBool();
    int maxPacketDups = loewctconfig.value("maxPacketDups", 1000).toInt();

    QByteArray msg = *(player->receivedDatas);
    int msgSize=5 + (((unsigned char)msg[3]) + (((unsigned char)msg[4]) << 8))/8;

#if UDP_SIMULATE_PACKETLOSS
    if (qrand() % 100 <= UDP_RECV_PERCENT_DROPPED)
    {
        //win.logMessage("UDP: Received packet dropped !");
        *(player->receivedDatas) = player->receivedDatas->mid(msgSize);
        if (player->receivedDatas->size())
            receiveMessage(player);
        return; // When we're done with the recursion, we still need to skip this message.
    }
    else
    {
        //win.logMessage("UDP: Received packet got through !");
    }
#endif
    // Check the sequence (seq) of the received message
    if ((unsigned char)msg[0] >= MsgUserReliableOrdered1 && (unsigned char)msg[0] <= MsgUserReliableOrdered32)
    {
        quint16 seq = (quint8)msg[1] + ((quint8)msg[2]<<8);
        quint8 channel = ((unsigned char)msg[0])-MsgUserReliableOrdered1;
        if (seq <= player->udpRecvSequenceNumbers[channel] && player->udpRecvSequenceNumbers[channel]!=0)
        {
            // If this is a missing packet, accept it
            MessageHead missingMsg;
            missingMsg.channel = channel;
            missingMsg.seq = seq;
            if (player->udpRecvMissing.contains(missingMsg))
            {
                win.logMessage("[INFO] UDP: Processing retransmission (-"+QString().setNum(player->udpRecvSequenceNumbers[channel]-seq)
                               +") from "+QString().setNum(player->pony.netviewId));
                for (int i=0; i<player->udpRecvMissing.size(); i++)
                    if (player->udpRecvMissing[i] == missingMsg)
                        player->udpRecvMissing.remove(i);
            }
            else
            {
                // We already processed this packet, we should discard it
                win.logMessage("[INFO] UDP: Discarding double message (-"+QString().setNum(player->udpRecvSequenceNumbers[channel]-seq)
                               +") from "+QString().setNum(player->pony.netviewId));
                win.logMessage("[INFO] UDP: Message was : "+QString(player->receivedDatas->left(msgSize).toHex().data()));
                player->nReceivedDups++;
                if (player->nReceivedDups >= maxPacketDups) // Kick the player if he's infinite-looping on us
                {
                    win.logMessage(QString("[INFO] UDP: Kicking "+QString().setNum(player->pony.netviewId)+" : Too many packet dups."));
                    sendMessage(player,MsgDisconnect, "Kicked by Server \nReason \n-------------------------------\n Lagging the server (too many packet dups)");
                    Player::disconnectPlayerCleanup(player); // Save game and remove the player
                    return;
                }
                *(player->receivedDatas) = player->receivedDatas->mid(msgSize);

                // Ack if needed, so that the client knows to move on already.
                if ((unsigned char)msg[0] >= MsgUserReliableOrdered1 && (unsigned char)msg[0] <= MsgUserReliableOrdered32) // UserReliableOrdered
                {
                    //win.logMessage("UDP: ACKing discarded message");
                    QByteArray data(3,0);
                    data[0] = (quint8)(msg[0]); // ack type
                    data[1] = (quint8)(((quint8)msg[1])/2); // seq
                    data[2] = (quint8)(((quint8)msg[2])/2); // seq
                    sendMessage(player, MsgAcknowledge, data);
                }
                if (player->receivedDatas->size())
                    receiveMessage(player);
                return; // When we're done with the recursion, we still need to skip this message.
            }
        }
        else if (seq > player->udpRecvSequenceNumbers[channel]+2) // If a message was skipped, keep going.
        {
            win.logMessage("[INFO] UDP: Unordered message (+"+QString().setNum(seq-player->udpRecvSequenceNumbers[channel])
                           +") received from "+QString().setNum(player->pony.netviewId));
            player->udpRecvSequenceNumbers[channel] = seq;

            // Mark the packets we skipped as missing
            for (int i=player->udpRecvSequenceNumbers[channel]+2; i<seq; i+=2)
            {
                MessageHead missing;
                missing.channel = channel;
                missing.seq = i;
                player->udpRecvMissing.append(missing);
            }
        }
        else
        {
            if (player->nReceivedDups > 0) // If he stopped sending dups, remove 10 dups at a time slowly
                player->nReceivedDups -= 10; // old: player->nReceivedDups--;
            //win.logMessage("UDP: Received message (="+QString().setNum(seq)
            //               +") from "+QString().setNum(player->pony.netviewId));
            player->udpRecvSequenceNumbers[channel] = seq;
        }
    }

    // Process the received message
    if ((unsigned char)msg[0] == MsgPing) // Ping
    {
        //win.logMessage("UDP: Ping received from "+player->IP+":"+QString().setNum(player->port)
        //        +" ("+QString().setNum((timestampNow() - player->lastPingTime))+"s)");
        player->lastPingNumber = (quint8)msg[5];
        player->lastPingTime = timestampNow();
        sendMessage(player,MsgPong);
    }
    else if ((unsigned char)msg[0] == MsgPong) // Pong
    {
        win.logMessage("[INFO] UDP: Pong received");
    }
    else if ((unsigned char)msg[0] == MsgConnect) // Connect SYN
    {
        msg.resize(18); // Supprime le message LocalHail et le Timestamp
        msg = msg.right(13); // Supprime le Header
#if DEBUG_LOG
        win.logMessage(QString("UDP: Connecting ..."));
#endif
        for (int i=0; i<32; i++) // Reset sequence counters
            player->udpSequenceNumbers[i]=0;

        if (!player->connected)
            sendMessage(player, MsgConnectResponse, msg);

    }
    else if ((unsigned char)msg[0] == MsgConnectionEstablished) // Connect ACK
    {
        if (player->connected)
          win.logMessage("[INFO] UDP: Receved duplicate connect ACK");
        else
          {
             win.logMessage("[INFO] UDP: Connected to client");
             player->connected=true;
             for (int i=0; i<32; i++) // Reset sequence counters
                player->udpSequenceNumbers[i]=0;

             onConnectAckReceived(player); // Clean the reliable message queue from SYN|ACKs

              // Start game
#if DEBUG_LOG
              win.logMessage(QString("UDP: Starting game"));
#endif
              // Set local player id
              win.lastIdMutex.lock();
              player->pony.id = win.getNewId();
              player->pony.netviewId = win.getNewNetviewId();
              win.lastIdMutex.unlock();

              win.logMessage("[INFO] UDP: Set id request : " + QString().setNum(player->pony.id) + "/" + QString().setNum(player->pony.netviewId));

              QByteArray id(3,0); // Set player Id request
              id[0]=4;
              id[1]=(quint8)(player->pony.id&0xFF);
              id[2]=(quint8)((player->pony.id>>8)&0xFF);
              sendMessage(player,MsgUserReliableOrdered6,id); // Sends a 48

              // Load Characters screen request
              QByteArray data(1,5);
              data += stringToData("Characters");
              sendMessage(player,MsgUserReliableOrdered6,data); // Sends a 48
          }
    }
    else if ((unsigned char)msg[0] == MsgAcknowledge) // Acknowledge
    {
        onAckReceived(msg, player);
    }
    else if ((unsigned char)msg[0] == MsgDisconnect) // Disconnect
    {
        win.logMessage("[INFO] UDP: Client disconnected");
        Player::disconnectPlayerCleanup(player); // Save game and remove the player

        if (playerJoined)
        {
            for (int i=0; i<win.udpPlayers.size(); i++)
            {
                sendChatMessage(win.udpPlayers[i],"<span color=\"yellow\">"+player->pony.name+" ("+player->name+") left the game.", serverPrefix, ChatGeneral);
            }
        }

        return; // We can't use Player& player anymore, it refers to free'd memory.
    }
    else if ((unsigned char)msg[0] >= MsgUserReliableOrdered1 && (unsigned char)msg[0] <= MsgUserReliableOrdered32) // UserReliableOrdered
    {
        //win.logMessage("UDP: Data received (hex) : ");
        //win.logMessage(player->receivedDatas->toHex() : \n"+player->receivedDatas->toHex().constData());

        QByteArray data(3,0);
        data[0] = (quint8)msg[0]; // ack type
        data[1] = (quint8)(((quint8)msg[1])/2); // seq
        data[2] = (quint8)(((quint8)msg[2])/2); // seq
        sendMessage(player, MsgAcknowledge, data);

        if ((unsigned char)msg[0]==MsgUserReliableOrdered6 && (unsigned char)msg[3]==8 && (unsigned char)msg[4]==0 && (unsigned char)msg[5]==6 ) // Prefab (player/mobs) list instantiate request
        {
            sendEntitiesList(player); // Called when the level was loaded on the client side
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered6 && (unsigned char)msg[3]==0x18 && (unsigned char)msg[4]==0 && (unsigned char)msg[5]==8 ) // Player game info (inv/ponyData/...) request
        {
            sendPonySave(player, msg); // Called when instantiate finished on the client side
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered4 && (unsigned char)msg[5]==0xf) // Chat
        {
            receiveChatMessage(msg, player);
        }
        else if ((quint8)msg[0]==MsgUserReliableOrdered4 && (quint8)msg[5]==0x1 && player->inGame!=0) // Edit ponies request error (happens if you click play twice quicly, for example)
        {
            win.logMessage("[INFO] UDP: Rejecting game start request from "+QString().setNum(player->pony.netviewId)
                           +" : player already in game");
            // Fix the buggy state we're now in
            // Reload to hide the "saving ponies" message box
            QByteArray data(1,5);
            data += stringToData(player->pony.sceneName);
            sendMessage(player,MsgUserReliableOrdered6,data);
            // Try to cancel the loading callbacks with inGame=1
            player->inGame = 1;
        }
        else if ((quint8)msg[0]==MsgUserReliableOrdered4 && (quint8)msg[5]==0x1 && player->inGame==0) // Edit ponies request
        {
            QList<Pony> ponies = Player::loadPonies(player);
            QByteArray ponyData = msg.right(msg.size()-10);
            win.logMessage(QString("[INFO] Pony data size: %1").arg(ponyData.size()));
            Pony pony{player};

#if IF_AUGUST_BUILD
            // fix invalid names (August Build)
            QString ponyName = dataToString(ponyData);
            if (ponyName.count(' ') > 1)
            {
                QStringList words = ponyName.split(' ');
                ponyName = words[0] + ' ' + words[1];
            }
#endif

            if ((unsigned char)msg[6]==0xff && (unsigned char)msg[7]==0xff && (unsigned char)msg[8]==0xff && (unsigned char)msg[9]==0xff)
            {
                // Create the new pony for this player
                pony.ponyData = ponyData;
                pony.name = dataToString(ponyData);
                pony.sceneName = "ponyville";
                pony.pos = findVortex(pony.sceneName, 0).destPos;
                ponies += pony;
            }
            else
            {
                quint32 id = (quint8)msg[6] +((quint8)msg[7]<<8) + ((quint8)msg[8]<<16) + ((quint8)msg[9]<<24);
                if (ponies.size()<0 || (quint32)ponies.size() <= id)
                {
                    win.logMessage("[INFO] UDP: Received invalid id in 'edit ponies' request. Kicking player.");
                    sendMessage(player,MsgDisconnect, "Kicked by Server \nReason \n-------------------------------\nFor sending invalid data and/or trying to crash the server with pony-id overflows.\nThis might be caused by an invalid pony name, so change it now.");
                    Player::disconnectPlayerCleanup(player); // Save game and remove the player
                    return; // It's ok, since we just disconnected the player
                }
                ponies[id].ponyData = ponyData;
                pony = ponies[id];
            }
            pony.id = player->pony.id;
            pony.netviewId = player->pony.netviewId;
            player->pony = pony;

            Player::savePonies(player, ponies);
            //Player::savePlayerData(player, ponies);

            player->pony.loadQuests();
            if (!player->pony.loadInventory()) // Create a default inventory if we can't find one saved
            {
                // Removing these default items. (6/11/2014)
                // Should have done that ever since shops were implemented.
                //InventoryItem raincloudHat{0,73};
                //InventoryItem goggles{1,17};
                //InventoryItem hat{2,62};
                //InventoryItem bag{3,60};
                //InventoryItem thbbbpt{4,65};
                //InventoryItem fluffleHat{5,66};
                //player->pony.inv << raincloudHat << goggles << hat << bag << thbbbpt << fluffleHat;
                player->pony.nBits = 15;
                player->pony.saveInventory();
            }

            sendLoadSceneRPC(player, player->pony.sceneName, player->pony.pos);
            // Send instantiate to the players of the new scene

            // Causes a duplication bug on sceneload, commented incase it is used in the future. (1/30/2015)
            /*Scene* scene = findScene(player->pony.sceneName);
            for (int i=0; i<scene->players.size(); i++)
                if (scene->players[i]->pony.netviewId!=player->pony.netviewId && scene->players[i]->inGame>=2)
                    sendNetviewInstantiate(&player->pony, scene->players[i]);*/

            //Send the 46s init messages
            //win.logMessage(QString("UDP: Sending the 46 init messages"));
            sendMessage(player,MsgUserReliableOrdered4,QByteArray::fromHex("141500000000")); // Sends a 46, init friends
            sendMessage(player,MsgUserReliableOrdered4,QByteArray::fromHex("0e00000000")); // Sends a 46, init journal

            // [LoEWCT] Check if the player that joins is banned
            // Running this in the edit ponies request because checking inside MsgConnectionEstablished, causes the server the send unecesary messages forever, meaning more memory usage.
            // In here the server does send unecesary messages, but then stops due to a ping timeout.
            // -> Moved them down here because it won't print the pony name anymore on playerJoined
            QDir playerDirs(PLAYERSPATH);
            for (int i=0; i<win.udpPlayers.size(); i++)
            {
                playerDirs.cd(win.udpPlayers[i]->name.toLatin1());
                QSettings playerConfig(PLAYERSPATH+win.udpPlayers[i]->name.toLatin1()+"/player.ini", QSettings::IniFormat);
                PlayerSettings::isBanned = playerConfig.value("isBanned", false).toBool();
                PlayerSettings::banReason = playerConfig.value("banReason", "The Ban Hammer has Spoken!").toString();
                PlayerSettings::ip = playerConfig.value("ip", "127.0.0.1").toString();

                if (PlayerSettings::isBanned)
                {
                    if (player->IP == win.udpPlayers[i]->IP || player->IP == PlayerSettings::ip)
                    {
                         QByteArray data(1,0);
                         data[0] = 0x7f; // Request number

                         data += stringToData(PlayerSettings::banReason);

                         win.logMessage(QString("[INFO] Banned player joined, kicking..."));
                         sendMessage(win.udpPlayers[i],MsgDisconnect, "Banned by Admin \n\nReason \n-------------------------------\n"+data);
                         return;
                    }
                    return;
                }
                else
                {
                    if (playerJoined)
                    {
                        sendChatMessage(win.udpPlayers[i],"<span color=\"yellow\">"+player->pony.name+" ("+player->name+") joined the game.</span>", serverPrefix, ChatGeneral);
                    }
                }

            }
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered20 && (unsigned char)msg[3]==0x18 && (unsigned char)msg[4]==0) // Vortex messages
        {
            if (player->inGame>=2)
            {
                quint8 id = (quint8)msg[5];
                Vortex vortex = findVortex(player->pony.sceneName, id);
                if (vortex.destName.isEmpty())
                {
                    win.logMessage("[SEVERE] Can't find vortex "+QString().setNum(id)+" on map "+player->pony.sceneName);
                }
                else
                {
                    sendLoadSceneRPC(player, vortex.destName, vortex.destPos);
                }
            }
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered4 && (unsigned char)msg[5]==0x2) // Delete pony request
        {
            win.logMessage(QString("[INFO] UDP: Deleting a character"));
            QList<Pony> ponies = Player::loadPonies(player);
            quint32 id = (quint8)msg[6] +((quint8)msg[7]<<8) + ((quint8)msg[8]<<16) + ((quint8)msg[9]<<24);
            ponies.removeAt(id);

            Player::savePonies(player,ponies);
            //Player::savePlayerData(player, ponies);
            sendPonies(player);
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered12 && (unsigned char)msg[7]==0xCA) // Animation
        {
            //win.logMessage("UDP: Broadcasting animation");
            // Send to everyone
            Scene* scene = findScene(player->pony.sceneName);
            if (scene->name.isEmpty())
                win.logMessage("[SEVERE] UDP: Can't find the scene for animation message, aborting");
            else
            {
                if (player->lastValidReceivedAnimation.isEmpty() ||
                    (quint8)player->lastValidReceivedAnimation[3] != (quint8)0x01 || (quint8)msg[5 + 3] == 0x00)
                  {
                    // Don't accept invalid animation (0x01 Flying 0x00 Landing)
                    // XXX The game lets players send nonsense (dancing while sitting down), those should be filtered
                    player->lastValidReceivedAnimation = msg.mid(5, msgSize - 5);
                    for (int i=0; i<scene->players.size(); i++)
                    {
                        if (scene->players[i] == player)
                            continue; // Don't send the animation to ourselves, it'll be played regardless
                        else if (scene->players[i]->inGame>=2)
                            sendMessage(scene->players[i], MsgUserReliableOrdered12, player->lastValidReceivedAnimation); // Broadcast
                    }
                }
            }
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x3D) // Skill
                {
#if DEBUG_LOG
            win.logMessage("[INFO] UDP: Broadcasting skill "+QString().setNum(dataToUint32(msg.mid(8))));
#endif

                    bool skillOk = true;
                    QByteArray reply;
                    uint32_t skillId = dataToUint32(msg.mid(8));
                    if (skillId == 2) // Teleport is a special case
                    {
                        if (msgSize == 28)
                        {
                            reply += msg.mid(5, 7); // Netview, RPC, skill IDs
                            reply += msg.mid(16, 4*3); // Pos X, Y, Z (floats)
                            reply += uint32ToData(0); // Skill upgrade (0)
                            reply += floatToData(timestampNow());
                        }
                        else if (msgSize == 18)
                        {
                            // Targeted teleport. First try to find the target in the udp players
                            quint16 targetNetId = dataToUint16(msg.mid(16));
                            Player* target = Player::findPlayer(win.udpPlayers, targetNetId);
                            Pony* targetPony = nullptr;
                            if (target->pony.netviewId == targetNetId && target->connected)
                                targetPony = &target->pony;
                            else
                            {
                                // The target isn't a player. Check if it's a NPC
                                for (Pony* npc : win.npcs)
                                    if (npc->netviewId == targetNetId)
                                        targetPony = npc;
                            }

                            if (targetPony != nullptr)
                            {
                                reply += msg.mid(5, 7);
                                reply += floatToData(targetPony->pos.x);
                                reply += floatToData(targetPony->pos.y);
                                reply += floatToData(targetPony->pos.z);
                                reply += uint32ToData(0); // Skill upgrade (0)
                                reply += floatToData(timestampNow());
                            }
                            else
                                win.logMessage(QObject::tr("UDP: Teleport target not found"));
                        }
                    }
                    else // Apply Skill
                        reply =  msg.mid(5, msgSize - 5);

                    // Mob health test
                    if (skillId==5 || skillId==10 || skillId==11 || skillId==14 || skillId==15 || skillId==20)
                    {
                        if (msgSize == 18)
                        {
                            // Targeted skill First try to find the target in the mobs
                            quint16 targetNetId = dataToUint16(msg.mid(16));
                            for (Mob* mob : win.mobs)
                            {
                                if (mob->netviewId == targetNetId)
                                {
                                    if (skillId == 20) mob->takeDamage(75);
                                    else mob->takeDamage(25);
                                    skillOk = Skill::applySkill(skillId, *mob, SkillTarget::Enemy);
                                    break;
                                }
                            }
                            //if (enablePVP)
                            //{
                                //Player* target = Player::findPlayer(win.udpPlayers, targetNetId);
                                //if (target->pony.netviewId != player->pony.netviewId)
                                //{
                                //    if (target && skillId == 20) target->pony.takeDamage(75);
                                //    else if (target) target->pony.takeDamage(25);
                                //}
                            //}
                            Player* target = Player::findPlayer(win.udpPlayers, targetNetId);
                            if (target->pony.netviewId == player->pony.netviewId) Skill::applySkill(skillId, target->pony, SkillTarget::Self);
                            else if (enablePVP) Skill::applySkill(skillId, target->pony, SkillTarget::Enemy); // PVP
                            else Skill::applySkill(skillId, target->pony, SkillTarget::Friendly);
                       }

                        // Apply animation
                        if (skillOk)
                        {
                            Skill& skill = Skill::skills[skillId];
                            SkillUpgrade& upgrade(skill.upgrades[0]);
                            sendAnimation(player, upgrade.casterAnimation);
                        }
                    }

                    // Send to everypony
                    Scene* scene = findScene(player->pony.sceneName);
                    if (scene->name.isEmpty())
                        win.logMessage(QObject::tr("UDP: Can't find the scene for skill message, aborting"));
                    else
                    {
                        for (int i=0; i<scene->players.size(); i++)
                            if (scene->players[i]->inGame>=2)
                                sendMessage(scene->players[i], MsgUserReliableOrdered11,reply); // Broadcast
                    }
                }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x08) // Wear request
        {
            quint8 index = msg[9];
            Scene* scene = findScene(player->pony.sceneName);
            if (scene->name.isEmpty())
                win.logMessage("[SEVERE] UDP: Can't find the scene for wear message, aborting");
            else
            {
                if (player->pony.tryWearItem(index))
                {
                    for (int i=0; i<scene->players.size(); i++)
                        if (scene->players[i]->inGame>=2
                                && scene->players[i]->pony.netviewId != player->pony.netviewId)
                            sendWornRPC(&player->pony, scene->players[i], player->pony.worn);
                }
                else
                {
                    win.logMessage("[INFO] Error trying to wear item");
                }
            }
        }
        // BeginShop Request (4/20/2014)
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x16)
        {
            // BeginShop doesn't specify which shop you want to buy from
            // We'll assume that there's never more than one shop per scene.

            // Does this mean that only one shop allowed per scene? :C ~WapaMario63


            uint16_t netviewId = dataToUint16(msg.mid(5));
            //win.logMessage("[INFO] Begin Shop with newviewId "+QString().setNum(netviewId));



            Pony* targetNpc = nullptr;
            for (Pony* npc : win.npcs)
              {
                // -> Oh, netviewId, well that can allow more than one shop, I hope ~WapaMario63
                // --> Aparently, at least for me, the server crashes upon opening, if I comment the shop command of an NPC, the server will load correctly.
                // ----> it din't, but I found a syntax error in the shop command inside quest.cpp, fixed.ww
                if (npc->netviewId == netviewId && npc->inv.size()) // If has a shop
                  {
                    targetNpc = npc;
                    break;
                  }
              }
            if (targetNpc)
              sendBeginShop(player, targetNpc);
            else
              win.logMessage("[SEVERE] UDP: Can't find a shop on map "+player->pony.sceneName+" for BeginShop");
        }
        // EndShop Request
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x17)
        {
            sendEndShop(player);
        }
        // Buy Item Request
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x0A)
        {
            /// TODO: At the moment we don't actually pay for items, since there are no monsters.
            ///
            /// -> Tux3, why not just add the option to pay for the item and commment it until the monsters are added. ~WapaMario63
            uint16_t itemId = dataToUint32(msg.mid(8));
            uint16_t amount = dataToUint32(msg.mid(12));

            player->pony.addInventoryItem(itemId, amount);
            sendSetBitsRPC(player);
        }
        // Sell Item Request
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x0B)
        {
            /// TODO: At the moment we don't actually pay for items, since there are no monsters.
            ///
            /// -> Tux3, why not just add the option to pay/sell of the item and commment it until the monsters are added. ~WapaMario63
            uint16_t itemId = dataToUint32(msg.mid(8));
            uint16_t amount = dataToUint32(msg.mid(12));

            player->pony.removeInventoryItem(itemId, amount);
            sendSetBitsRPC(player);
        }

        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x04) // Get worn items request
        {
            quint16 targetId = ((quint16)(quint8)msg[5]) + (((quint16)(quint8)msg[6])<<8);
            Player* target = Player::findPlayer(win.udpPlayers, targetId);
            if (target->pony.netviewId == targetId && target->connected)
                sendWornRPC(&target->pony, player, target->pony.worn);
            else
            {
                Pony* targetNpc = nullptr;
                for (Pony* npc : win.npcs)
                {
                    if (npc->netviewId == targetId)
                    {
                        targetNpc = npc;
                        break;
                    }
                }
                if (targetNpc)
                    sendWornRPC(targetNpc, player, targetNpc->worn);
                else
                    win.logMessage("[SEVERE] UDP: Can't find netviewId "+QString().setNum(targetId)+" to send worn items");
            }
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x09) // Unwear item request
        {
            quint16 targetId = ((quint16)(quint8)msg[5]) + (((quint16)(quint8)msg[6])<<8);
            Player* target = Player::findPlayer(win.udpPlayers, targetId);
            if (target->pony.netviewId == targetId)
            {
                target->pony.unwearItemAt(dataToUint8(msg.mid(8)));
            }
            else
                win.logMessage("[SEVERE] UDP: Can't find netviewId "+QString().setNum(targetId)+" to unwear item");
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered11 && (unsigned char)msg[7]==0x31) // Run script (NPC) request
        {
            quint16 targetId = ((quint16)(quint8)msg[5]) + (((quint16)(quint8)msg[6])<<8);
            //win.logMessage("UDP: Quest "+QString().setNum(targetId)+" requested");
            for (int i=0; i<player->pony.quests.size(); i++)
                if (player->pony.quests[i].id == targetId)
                {
                    player->pony.lastQuest = i;
                    player->pony.quests[i].runScript();
                    break;
                }
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered4 && (unsigned char)msg[5]==0xB) // Continue dialog
        {
            //win.logMessage("UDP: Resuming script for quest "+QString().setNum(player->pony.lastQuest));
            player->pony.quests[player->pony.lastQuest].processAnswer();
        }
        else if ((unsigned char)msg[0]==MsgUserReliableOrdered4 && (unsigned char)msg[5]==0xC) // Continue dialog (with answer)
        {
            quint32 answer = ((quint32)(quint8)msg[6])
                            + (((quint32)(quint8)msg[7])<<8)
                            + (((quint32)(quint8)msg[8])<<16)
                            + (((quint32)(quint8)msg[9])<<24);
            //win.logMessage("Resuming script with answer "+QString().setNum(answer)
            //               +" for quest "+QString().setNum(player->pony.lastQuest));
            player->pony.quests[player->pony.lastQuest].processAnswer(answer);
        }
        else
        {
            // Display data
            quint32 unknownMsgSize =  (((quint16)(quint8)msg[3]) +(((quint16)(quint8)msg[4])<<8)) / 8;
            win.logMessage("[INFO] UDP: Unknown message received : "
                           +QString(player->receivedDatas->left(unknownMsgSize+5).toHex().data()));
            *player->receivedDatas = player->receivedDatas->mid(unknownMsgSize+5);
            msgSize=0;
        }
    }
    else if ((unsigned char)msg[0]==MsgUserUnreliable) // Sync (position) update
    {
        if (dataToUint16(msg.mid(5)) == player->pony.netviewId)
            Sync::receiveSync(player, msg);
    }
    else
    {
        // Display data
        win.logMessage("[INFO] Unknown data received (UDP) (hex) : ");
        win.logMessage(QString(player->receivedDatas->toHex().data()));
        quint32 unknownMsgSize = (((quint16)(quint8)msg[3]) +(((quint16)(quint8)msg[4])<<8)) / 8;
        *player->receivedDatas = player->receivedDatas->mid(unknownMsgSize+5);
        msgSize=0;
    }

    *player->receivedDatas = player->receivedDatas->mid(msgSize);
    if (player->receivedDatas->size())
        receiveMessage(player);
}

