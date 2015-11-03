#include <console.h>
#include <loewct.h>
#include <widget.h>
#include <serialize.h>

void Console::inputCmdHandler(QString str)
{
    if (str.startsWith("start"))
    {
        cmdStartServer(); return;
    }
    else if (str.startsWith("stop"))
    {
        cmdStopServer(); return;
    }
    else if (str.startsWith("help"))
    {
        cmdShowHelp(); return;
    }
    else if (str.startsWith("helpDebug"))
    {
        cmdShowDebugHelp(); return;
    }
    else if (str.startsWith("listTcpPlayers"))
    {
        cmdListTcpPlayers(); return;
    }
    else if (str.startsWith("setPlayer"))
    {
        if (win.udpPlayers.size() == 1)
        {
            cmdSetPlayer(); return;
        }

        str = str.right((str.size()-10));
        QStringList args = str.split(":");
        bool ok;
        if (args.size() != 2)
        {
            quint16 id = args[0].toUInt(&ok);
            if (!ok)
            {
                logErrMsg("UDP: setPlayer takes a player ID as a function/argument");
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
                logErrMsg("UDP: setPlayer takes a player ID as a function/argument");
                return;
            }
            else
            {
                cmdSetPlayer(args[0], port); return;
            }
        }
    }
    else if (str.startsWith("listPlayers"))
    {
        if (str.size() <= 12)
        {
            cmdListPlayers(); return;
        }
        else
        {
            str = str.right(str.size()-12);
            cmdListPlayers(str); return;
        }
    }
    else if (str.startsWith("listVortexes"))
    {
        cmdListVortexes(); return;
    }
    else if (str.startsWith("sync"))
    {
        cmdSync(); return;
    }
    else if (str.startsWith("dbgStressLoad"))
    {
        cmdDebugStressLoad(); return;
    }
    else if (str.startsWith("tp"))
    {
        str = str.right(str.size()-3);
        QStringList args = str.split(" ", QString::SkipEmptyParts);

        cmdTpPlayerToPlayer(args[0].toUInt(), args[1].toUInt());
        return;
    }
    else if (str.startsWith("say"))
    {
        str = str.right(str.size()-4);

        cmdServerSay(str); return;
    }
    else if (str.startsWith("announce"))
    {
        str = str.right(str.size()-9);

        if (str == "all")
        {
            str = str.right(str.size()-4);
            QStringList args = str.split("|", QString::SkipEmptyParts);

            cmdAnnouncement(args[1],args[0].toFloat());
            return;
        }
        else if (str == "player")
        {
            isPlayerNotSet();
            str = str.right(str.size()-7);
            QStringList args = str.split("|", QString::SkipEmptyParts);
            cmdAnnouncePlayer(cmdPlayer, args[1], args[0].toFloat());
            return;
        }
    }
    else if (str.startsWith("kick"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-5);
        cmdKickPlayer(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("ban"))
    {
        logInfoMsg("Banning Functionality disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");
        return;
    }
    else if (str.startsWith("loadscene"))
    {
        isPlayerNotSet();
        str = str.mid(10);
        cmdLoadScene(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("getPos"))
    {
        isPlayerNotSet();
        cmdGetPosition(cmdPlayer);
        return;
    }
    else if (str.startsWith("getRot"))
    {
        isPlayerNotSet();
        cmdGetRotation(cmdPlayer);
        return;
    }
    else if (str.startsWith("getPonyData"))
    {
        isPlayerNotSet();
        logInfoMsg("ponyData for "+cmdPlayer->pony.name+":\n"+
                   cmdPlayer->pony.ponyData.toBase64());
        return;
    }
    else if (str.startsWith("dbgSendPonies"))
    {
        isPlayerNotSet();
        logInfoMsg("Sending ponies list to: "+cmdPlayer->name);
        cmdSendPonies(cmdPlayer);
        return;
    }
    else if (str.startsWith("dbgSendUtils3"))
    {
        isPlayerNotSet();
        logInfoMsg("Sending Utils3 request to: "+cmdPlayer->name);
        cmdSendUtils3(cmdPlayer);
        return;
    }
    else if (str.startsWith("dbgSetPlayerId"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-15);
        cmdSetPlayerId(cmdPlayer, str.toUInt());
        return;
    }
    else if (str.startsWith("dbgReloadNpcs"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-14);
        cmdReloadNpcs(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("dbgSendPonyData"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-16);
        cmdSendPonyData(cmdPlayer, stringToData(str).toBase64());
        return;
    }
    else if (str.startsWith("dbgSetStat"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-11);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        if (args.size() != 2)
        {
            logErrMsg("Usage is 'dbgSetStat <statId> <statValue>'");
            return;
        }
        bool ok, ok2;
        quint8 statID = args[0].toInt(&ok);
        float statVal = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logErrMsg("StatId is an 8bit unsigned integer and statValue is a floating number.");
            return;
        }
        else
        {
            cmdSetStat(cmdPlayer, statID, statVal);
            return;
        }
    }
    else if (str.startsWith("dbgSetMaxStat"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-14);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        if (args.size() != 2)
        {
            logErrMsg("Usage is 'dbgSetMaxStat <statId> <statValue>'");
            return;
        }
        bool ok, ok2;
        quint8 statID = args[0].toInt(&ok);
        float statVal = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logErrMsg("StatId is an 8bit unsigned integer and statValue is a floating number.");
            return;
        }
        else
        {
            cmdSetMaxStat(cmdPlayer, statID, statVal);
            return;
        }
    }
    else if (str.startsWith("instantiate"))
    {
        isPlayerNotSet();

        if (str == "instantiate")
        {
            cmdInstantiate(cmdPlayer);
            return;
        }
        else
        {
            str = str.right(str.size()-12);
            QStringList args = str.split(' ', QString::SkipEmptyParts);
            if (args.size() == 2)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt());
                return;
            }
            else if (args.size() == 5)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt(), args[2].toFloat(), args[3].toFloat(), args[4].toFloat());
                return;
            }
            else if (args.size() == 9)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt(), args[2].toFloat(), args[3].toFloat(), args[4].toFloat(), args[5].toFloat(), args[6].toFloat(), args[7].toFloat(), args[8].toFloat());
                return;
            }
            else
            {
                logErrMsg("Usage is 'instantiate <viewId> <ownerId> [x] [y] [z] [rx] [ry] [rz] [rw]'");
                return;
            }
        }
    }
    else if (str.startsWith("dbgBeginDialog"))
    {
        isPlayerNotSet();
        cmdBeginDialog(cmdPlayer);
        return;
    }
    else if (str.startsWith("dbgEndDialog"))
    {
        isPlayerNotSet();
        cmdEndDialog(cmdPlayer);
        return;
    }
    else if (str.startsWith("dbgSetDialogMsg"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-16);
        cmdSetDialogMsg(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("dbgSetDialogOptions"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-20);

        cmdSetDialogOptions(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("move"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-5);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        cmdMove(cmdPlayer, args[0].toFloat(), args[1].toFloat(), args[2].toFloat());
        return;
    }
    else if (str.startsWith("error"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-6);
        cmdErrorMessage(cmdPlayer, str);
        return;
    }
    else if (str.startsWith("listQuests"))
    {
        isPlayerNotSet();
        cmdListQuests(cmdPlayer);
        return;
    }
    else if (str.startsWith("listMobs"))
    {
        cmdListMobs(); return;
    }
    else if (str.startsWith("listInventory"))
    {
        isPlayerNotSet();
        cmdListInventory(cmdPlayer);
        return;
    }
    else if (str.startsWith("listWorn"))
    {
        isPlayerNotSet();
        cmdListWornItems(cmdPlayer);
        return;
    }
    else if (str.startsWith("giveItem"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-9);
        QStringList args = str.split(" ", QString::SkipEmptyParts);

        cmdGiveItem(cmdPlayer, args[0].toInt(), args[1].toInt());
        return;
    }
    else if (str.startsWith("listNpcs"))
    {
        cmdListNpcs(); return;
    }
    else if (str.startsWith("giveMod"))
    {
        logInfoMsg("Moderation Functionality is disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");
        return;
    }
    else if (str.startsWith("deMod"))
    {
        logInfoMsg("Moderation Functionality is disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");
        return;
    }
    else
    {
        logErrMsg("Unknown command. Type 'help' or '?' for a list of commands.");
        return;
    }
}

void Console::run()
{
    while (true)
    {
        cmdPlayer = Player::findPlayer(win.udpPlayers, cmdPlayer->IP, cmdPlayer->port);

        cin >> cmdString;
        inputCmdHandler(cmdString);
    }
}
