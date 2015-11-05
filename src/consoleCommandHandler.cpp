#include <console.h>
#include <loewct.h>
#include <widget.h>
#include <Utils/serialize.h>

void Console::inputCmdHandler(QString str)
{
    if (str ==("start"))
    {
        cmdStartServer();
    }
    else if (str ==("stop"))
    {
        cmdStopServer();
    }
    else if (str ==("help") || str == "?")
    {
        cmdShowHelp();
    }
    else if (str ==("helpDebug"))
    {
        cmdShowDebugHelp();
    }
    else if (str ==("listTcpPlayers"))
    {
        cmdListTcpPlayers();
    }
    else if (str ==("setPlayer"))
    {
        if (win.udpPlayers.size() == 1)
        {
            cmdSetPlayer();
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

            }
            else
            {
                cmd.cmdSetPlayer(id);
            }
        }
        else
        {
            quint16 port = args[1].toUInt(&ok);
            if (!ok)
            {
                logErrMsg("UDP: setPlayer takes a player ID as a function/argument");

            }
            else
            {
                cmdSetPlayer(args[0], port);
            }
        }
    }
    else if (str ==("listPlayers"))
    {
        if (str.size() <= 12)
        {
            cmdListPlayers();
        }
        else
        {
            str = str.right(str.size()-12);
            cmdListPlayers(str);
        }
    }
    else if (str ==("listVortexes"))
    {
        cmdListVortexes();
    }
    else if (str ==("sync"))
    {
        cmdSync();
    }
    else if (str ==("dbgStressLoad"))
    {
        cmdDebugStressLoad();
    }
    else if (str ==("tp"))
    {
        str = str.right(str.size()-3);
        QStringList args = str.split(" ", QString::SkipEmptyParts);

        cmdTpPlayerToPlayer(args[0].toUInt(), args[1].toUInt());

    }
    else if (str ==("say"))
    {
        str = str.right(str.size()-4);

        cmdServerSay(str);
    }
    else if (str.startsWith("announce"))
    {
        str = str.right(str.size()-9);

        if (str == "all")
        {
            str = str.right(str.size()-4);
            QStringList args = str.split("|", QString::SkipEmptyParts);

            cmdAnnouncement(args[1],args[0].toFloat());

        }
        else if (str == "player")
        {
            isPlayerNotSet();
            str = str.right(str.size()-7);
            QStringList args = str.split("|", QString::SkipEmptyParts);
            cmdAnnouncePlayer(cmdPlayer, args[1], args[0].toFloat());

        }
    }
    else if (str ==("kick"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-5);
        cmdKickPlayer(cmdPlayer, str);

    }
    else if (str ==("ban"))
    {
        logInfoMsg("Banning Functionality disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");

    }
    else if (str ==("loadscene"))
    {
        isPlayerNotSet();
        str = str.mid(10);
        cmdLoadScene(cmdPlayer, str);

    }
    else if (str ==("getPos"))
    {
        isPlayerNotSet();
        cmdGetPosition(cmdPlayer);

    }
    else if (str ==("getRot"))
    {
        isPlayerNotSet();
        cmdGetRotation(cmdPlayer);

    }
    else if (str ==("getPonyData"))
    {
        isPlayerNotSet();
        logInfoMsg("ponyData for "+cmdPlayer->pony.name+":\n"+
                   cmdPlayer->pony.ponyData.toBase64());

    }
    else if (str ==("dbgSendPonies"))
    {
        isPlayerNotSet();
        logInfoMsg("Sending ponies list to: "+cmdPlayer->name);
        cmdSendPonies(cmdPlayer);

    }
    else if (str ==("dbgSendUtils3"))
    {
        isPlayerNotSet();
        logInfoMsg("Sending Utils3 request to: "+cmdPlayer->name);
        cmdSendUtils3(cmdPlayer);

    }
    else if (str ==("dbgSetPlayerId"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-15);
        cmdSetPlayerId(cmdPlayer, str.toUInt());

    }
    else if (str ==("dbgReloadNpcs"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-14);
        cmdReloadNpcs(cmdPlayer, str);

    }
    else if (str ==("dbgSendPonyData"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-16);
        cmdSendPonyData(cmdPlayer, stringToData(str).toBase64());

    }
    else if (str ==("dbgSetStat"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-11);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        if (args.size() != 2)
        {
            logErrMsg("Usage is 'dbgSetStat <statId> <statValue>'");

        }
        bool ok, ok2;
        quint8 statID = args[0].toInt(&ok);
        float statVal = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logErrMsg("StatId is an 8bit unsigned integer and statValue is a floating number.");

        }
        else
        {
            cmdSetStat(cmdPlayer, statID, statVal);

        }
    }
    else if (str ==("dbgSetMaxStat"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-14);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        if (args.size() != 2)
        {
            logErrMsg("Usage is 'dbgSetMaxStat <statId> <statValue>'");

        }
        bool ok, ok2;
        quint8 statID = args[0].toInt(&ok);
        float statVal = args[1].toFloat(&ok2);
        if (!ok || !ok2)
        {
            logErrMsg("StatId is an 8bit unsigned integer and statValue is a floating number.");

        }
        else
        {
            cmdSetMaxStat(cmdPlayer, statID, statVal);

        }
    }
    else if (str ==("instantiate"))
    {
        isPlayerNotSet();

        if (str == "instantiate")
        {
            cmdInstantiate(cmdPlayer);

        }
        else
        {
            str = str.right(str.size()-12);
            QStringList args = str.split(' ', QString::SkipEmptyParts);
            if (args.size() == 2)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt());

            }
            else if (args.size() == 5)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt(), args[2].toFloat(), args[3].toFloat(), args[4].toFloat());

            }
            else if (args.size() == 9)
            {
                cmdInstantiate(cmdPlayer, args[0].toUInt(), args[1].toUInt(), args[2].toFloat(), args[3].toFloat(), args[4].toFloat(), args[5].toFloat(), args[6].toFloat(), args[7].toFloat(), args[8].toFloat());

            }
            else
            {
                logErrMsg("Usage is 'instantiate <viewId> <ownerId> [x] [y] [z] [rx] [ry] [rz] [rw]'");

            }
        }
    }
    else if (str ==("dbgBeginDialog"))
    {
        isPlayerNotSet();
        cmdBeginDialog(cmdPlayer);

    }
    else if (str ==("dbgEndDialog"))
    {
        isPlayerNotSet();
        cmdEndDialog(cmdPlayer);

    }
    else if (str ==("dbgSetDialogMsg"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-16);
        cmdSetDialogMsg(cmdPlayer, str);

    }
    else if (str ==("dbgSetDialogOptions"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-20);

        cmdSetDialogOptions(cmdPlayer, str);

    }
    else if (str ==("move"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-5);

        QStringList args = str.split(" ", QString::SkipEmptyParts);
        cmdMove(cmdPlayer, args[0].toFloat(), args[1].toFloat(), args[2].toFloat());

    }
    else if (str ==("error"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-6);
        cmdErrorMessage(cmdPlayer, str);

    }
    else if (str ==("listQuests"))
    {
        isPlayerNotSet();
        cmdListQuests(cmdPlayer);

    }
    else if (str ==("listMobs"))
    {
        cmdListMobs();
    }
    else if (str ==("listInventory"))
    {
        isPlayerNotSet();
        cmdListInventory(cmdPlayer);

    }
    else if (str ==("listWorn"))
    {
        isPlayerNotSet();
        cmdListWornItems(cmdPlayer);

    }
    else if (str ==("giveItem"))
    {
        isPlayerNotSet();
        str = str.right(str.size()-9);
        QStringList args = str.split(" ", QString::SkipEmptyParts);

        cmdGiveItem(cmdPlayer, args[0].toInt(), args[1].toInt());

    }
    else if (str ==("listNpcs"))
    {
        cmdListNpcs();
    }
    else if (str ==("giveMod"))
    {
        logInfoMsg("Moderation Functionality is disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");

    }
    else if (str ==("deMod"))
    {
        logInfoMsg("Moderation Functionality is disabled in this [LoEWCT] version as it's deprecated and it doesn't work properly.");

    }
    else
    {
        logErrMsg("Unknown command. Type 'help' or '?' for a list of commands.");

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
