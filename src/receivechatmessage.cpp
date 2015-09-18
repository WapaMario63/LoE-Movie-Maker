#include "receiveChatMessage.h"
#include "character.h"
#include "serialize.h"
#include "message.h"
#include "widget.h"
#include "chat_widget.h"
#include "loewct_main.h"
#include "settings.h"
#include "form.h"
#include <demo.h>

void receiveChatMessage(QByteArray msg, Player *player)
{
    QSettings loeWctConfig(LOEWCTCONFIGFILEPATH, QSettings::IniFormat);
    QString motdMessage = loeWctConfig.value("motdMessage", "Parties at Sugarcane Corner are always AWESOME! Why don't you join the party?").toString();
    QString rulesMessage = loeWctConfig.value("rulesMessage", "Server rules: \n1.No Swearing excesively \n2.Respect Others \n3.HAVE FUN!").toString();
    QString helpMessage = loeWctConfig.value("helpMessage", "Welcome to our Legends of Equestria Server! \nType !?cmds to view some commands for this server.").toString();
    QString serverPrefix = loeWctConfig.value("serverPrefix", "[LoEWCT]").toString();
    bool mapTpEnable = loeWctConfig.value("playerMapTp", true).toBool();
    int rollCoolDown = loeWctConfig.value("rollCoolDown", 10).toInt();

  QString txt = dataToString(msg.mid(7));
  QString author = player->pony.name;
  //win.logMessage("Chat "+author+":"+txt);
  //cwin.logChatMessage("<"+author+"> "+txt);

  if (txt.startsWith("/stuck") || txt.startsWith("unstuck me"))
  {
      sendLoadSceneRPC(player, player->pony.sceneName);
  }
  else if (txt.startsWith("!anhero"))
  {
      QTimer *anheroTimer = new QTimer();
      anheroTimer->setSingleShot(true);

      QString deadPlayerName = player->name;
      QObject::connect(anheroTimer, &QTimer::timeout, [=]()
      {
          // find player again instead of reusing pointer, in case they disconnect
          Player* deadPlayer = Player::findPlayer(win.udpPlayers, deadPlayerName);
          if (deadPlayer->connected && deadPlayer->pony.dead)
          {
              sendSetStatRPC(deadPlayer, 1, deadPlayer->pony.health);
              Scene* scene = findScene(deadPlayer->pony.sceneName);
              for (Player* other : scene->players) sendNetviewInstantiate(&deadPlayer->pony, other);
              deadPlayer->pony.dead = false;
          }
          delete anheroTimer;
          // Don't delete deadPlayer here. that kills the player's session.
      });
      if (!player->pony.dead)
      {
          player->pony.dead = true;
          sendSetStatRPC(player, 1, 0);
          anheroTimer->start(swin.anHeroDelay);
          Scene* scene = findScene(player->pony.sceneName);
          for (Player* other : scene->players)
              sendNetviewRemove(other, player->pony.netviewId, NetviewRemoveReasonKill);
      }
  }
  else if (txt.startsWith("!shout"))
  {
      txt.remove(0, 7);
      for (int i=0; i<win.udpPlayers.size(); i++)
      {
          sendChatMessage(win.udpPlayers[i], "<span color=\"yellow\">"+txt+"</span>", "[BROADCAST]", ChatSystem);
      }
  }
  else if (txt.startsWith("!motd"))
    {
      QByteArray motdData(1,0);
      motdData[0] = 0x7f; // Request Number
      motdData += stringToData(motdMessage);
      sendMessage(player, MsgUserReliableOrdered4, motdData); // 31 max characters per line
    }
  else if (txt.startsWith("!rules"))
    {
      QByteArray ruleData(1,0);
      ruleData[0] = 0x7f; // Request Number
      ruleData += stringToData(rulesMessage);
      sendMessage(player, MsgUserReliableOrdered4, ruleData);
    }
  else if (txt.startsWith("!help"))
    {
      sendChatMessage(player, "<span color=\"orange\">"+helpMessage+"</span>", serverPrefix, ChatGeneral);
    }
  else if (txt.startsWith("!commands"))
    {
      sendChatMessage(player, "Commands: !help, !motd, !rules, !roll, !msg <ponyName> <msg>, !players, !mapTP <map>, !me <action>. More Commands Coming soon!", serverPrefix, ChatGeneral);
    }
  else if (txt.startsWith("!rtd"))
    {
      sendChatMessage(player, "Roll the dice not available at the moment do to lack of many features of this LoE Private Server version", serverPrefix, ChatGeneral);
    }
  else if (txt.startsWith("!msg"))
  {
      if(txt.count(" ") < 2)
          sendChatMessage(player, "Usage: !msg [player] [message]<br /><span color=\"yellow\">Player names are case-sensitive. You don't need to type spaces when typing out their full name.</span>", author, ChatLocal);
      else
      {
          for (int i=0; i<win.udpPlayers.size(); i++)
              if (win.udpPlayers[i]->inGame>=2 && win.udpPlayers[i]->pony.name.toLower().remove(" ").startsWith(txt.toLower().section(" ", 1, 1)))
              {
                  txt = txt.remove(0, txt.indexOf(" ", 5) + 1);
                  sendChatMessage(win.udpPlayers[i], "<span color=\"yellow\">[PM] </span>" + txt, author, ChatLocal);
                  sendChatMessage(win.udpPlayers[i], "[You -> "+ win.udpPlayers[i]->pony.name +"] "+ txt, serverPrefix, ChatLocal);
              }
      }

  }
  else if (txt.startsWith("!broadcast"))
  {
       txt.remove(0, 10);
       for (int i=0; i<win.udpPlayers.size(); i++)
       {
           sendChatMessage(win.udpPlayers[i], "<span color=\"yellow\">"+txt+"</span>", "[BROADCAST]", ChatGeneral);
       }
  }
  else if (txt.startsWith("!players"))
    {
      QString nameList = "<span color=\"yellow\">Players currently ingame:</span>";

      for (int i=0; i<win.udpPlayers.size(); i++)
        {
          if (win.udpPlayers[i]->inGame>=2)
            {
              nameList += "<span color=\"yellow\">"+win.udpPlayers[i]->pony.name+" ("+win.udpPlayers[i]->name+") </span>";
            }
          sendChatMessage(player, nameList, serverPrefix, ChatGeneral);
        }
    }
  // Gave an option to enable this command or not, check bool mapTpEnable.
  else if (txt.startsWith("!mapTP"))
    {
        if (mapTpEnable)
          {
            if (txt.count(" ") < 1)
              {
                QString mapTpMsgSend = "Usage: <em>!mapTP [map]</em><br /><span color=\"yellow\">Available Maps (must be in case sensitive):</span><em>";

                for (int i=0; i<win.scenes.size(); i++)
                  {
                     mapTpMsgSend += "<br />" + win.scenes[i].name;
                  }
                sendChatMessage(player, mapTpMsgSend +"</em>", serverPrefix, ChatGeneral);
                }
            else
              {
                sendLoadSceneRPC(player, txt.remove(0, 7));
              }
          }
        else
          {
            sendChatMessage(player, "<span color=\"red\">You are not allowed to use this command.</span>", serverPrefix, ChatSystem);
          }
    }
    else if (txt.startsWith("!vote"))
    {
        // This command only starts working on the class GameMode, we only get the number input here for use over said class.
#include "gamemode.h"

        //txt.remove(0, 6);
        //if (!GameMode::votemap) sendChatMessage(player, "<span color=\"red\">There is no vote session!</span>", serverPrefix, ChatSystem);
        //else GameMode::votenum = txt.toInt();
    }
    /*else if (txt.startsWith("!demo"))
    {
        sendChatMessage(player,"<span color=\"orange\">[LoEWCT] Demo Recorder \nArguments are: \n!demo record - Records a demo. \n!demo stop - Stops recording a demo</span>", "[LoEWCT Demo Manager]", ChatSystem);
    }*/
    else if (txt.startsWith("!demorecord"))
    {
        txt.remove(0, 13);
        if (dem.isRecording) sendChatMessage(player,"<span color=\"orange\">You are already recording!</span>", "[LoEWCT Demo Manager]", ChatSystem);
        else
        {
            sendChatMessage(player,"<span color=\"orange\">Recording demo with name "+txt+".</span>", "[LoEWCT Demo Manager]", ChatSystem);
            dem.demoSyncRecord("/demos/"+txt+".loedemo");
            dem.isRecording = true;
        }
    }
    else if (txt.startsWith("!demostop"))
    {
        if (dem.isRecording)
        {
            dem.stopDemo(true);
            dem.isRecording = false;
            sendChatMessage(player,"<span color=\"orange\">Stopped recording demo. It has been saved in the server's demos folder.</span>", "[LoEWCT Demo Manager]", ChatSystem);
        }
        else sendChatMessage(player,"<span color=\"orange\">You are not recording a demo!</span>", "[LoEWCT Demo Manager]", ChatSystem);
    }
    else if (txt == ("!demoplay"))
    {

    }
    else if (txt.startsWith("!generatenpc"))
    {
        sendChatMessage(player,"<span color=\"orange\">An npc of yourself is being generated...</span>", "[LoEWCT]", ChatSystem);
        dem.generateNpc(player, player->pony.name);
        lwin.externCmdLine("reloadNpc "+player->pony.name);
        sendLoadSceneRPC(player,player->pony.sceneName.toLower(),UVector(player->pony.pos.x,player->pony.pos.y,player->pony.pos.z));
        sendChatMessage(player,"<span color=\"orange\">Npc succesfully generated!</span>", "[LoEWCT]", ChatSystem);
    }
  else
  {
      // Broadcast the message
      int rollnum = -1;
      QString rollStr;
      bool actMsg = false;

      if (txt == ("!roll"))
        {
          if (player->chatRollCooldownEnd < QDateTime::currentDateTime())
            {
              rollnum = qrand() % 100;
              rollStr.sprintf("<span color=\"yellow\">#b%s#b rolls %02d</span>", author.toLocal8Bit().data(), rollnum);
              // Add cooldown to roll, server admin sets interval in seconds (QDateTime rollCoolDown) in LoEWCT_config.ini
              player->chatRollCooldownEnd = QDateTime::currentDateTime().addSecs(rollCoolDown);
              //if (player->chatRollCooldownEnd < rollCoolDown);
              //{
              //  sendChatMessage(player, "<span color=\"yellow\">You must wait "+ rollCoolDown +" seconds to roll again.</span>");
              //}
            }
        }
      if (txt.startsWith("!me"))
        {
          actMsg = true;
          txt.remove(0, 3);
          txt = "<em>#b <span color=\"yellow\">"+ author +"</span>#b <span color=\"cyan\">"+ txt +"</span></em>";
        }
      if ((quint8)msg[6] == 8) // Local chat only
      {
          Scene* scene = findScene(player->pony.sceneName);
          if (scene->name.isEmpty())
            {
              win.logMessage("[INFO] UDP: Can't find the scene for chat message, aborting");
            }
          else
              for (int i=0; i<scene->players.size(); i++)
                {
                  if (scene->players[i]->inGame>=2)
                    {
                      if (rollnum > -1)
                        {
                            sendChatMessage(scene->players[i], rollStr, serverPrefix, ChatLocal);
                        }
                      else if (actMsg)
                        {
                          sendChatMessage(scene->players[i], txt, "", ChatLocal);
                        }
                      else if (txt.startsWith(">"))
                        {
                          sendChatMessage(scene->players[i], "<span color=\"green\">"+ txt +"</span>", author, ChatLocal);
                        }
                      else
                        {
                          sendChatMessage(scene->players[i], txt, author, ChatLocal);
                        }
                    }
                }
      }

      else // Send globally
          for (int i=0; i<win.udpPlayers.size(); i++)
              if (win.udpPlayers[i]->inGame>=2)
              {
                  if (rollnum > -1)
                      sendChatMessage(win.udpPlayers[i], rollStr, serverPrefix, ChatGeneral);
                  else if (actMsg)
                      sendChatMessage(win.udpPlayers[i], txt, "", ChatGeneral);
                  else if (txt.startsWith(">"))
                      sendChatMessage(win.udpPlayers[i], "<span color=\"green\">" + txt + "</span>", author, ChatGeneral);
                  //else if (txt.contains("&0"))
                  //    sendChatMessage(win.udpPlayers[i], "<span color=\"black\">"+ txt + "</span>", author, ChatGeneral);
                  else
                  {
                      QSettings playerConfig(PLAYERSPATH+win.udpPlayers[i]->name.toLatin1()+"/player.ini", QSettings::IniFormat);
                      PlayerSettings::isModerator = playerConfig.value("isModerator", false).toBool();
                      if (PlayerSettings::isModerator)
                      {
                          for (int i=0; i<win.udpPlayers.size(); i++)
                          {
                            sendChatMessage(win.udpPlayers[i], txt, "<span color=\"cyan\">[MOD]</span>"+author, ChatGeneral);
                          }
                      } else sendChatMessage(win.udpPlayers[i], txt, author, ChatGeneral);
                   }

              }

  }
}
