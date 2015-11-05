#include "quest.h"
#include "widget.h"
#include "message.h"
#include "items.h"
#include "Utils/serialize.h"
#include "settings.h"
#include "form.h"
#include <QFile>

void Quest::exception(QString msg)
{
    QString QSE = "QuestScript Exception \n---------------------\n";
    QByteArray data(1,0);
    data[0] = 0x7f;
    data += stringToData(QSE+msg);

    sendMessage(owner, MsgUserReliableOrdered4, data);
}

void QuestFunction::launchFunction(function func, QString args, Player *player)
{
    Quest* q;

    if (func == q_sendChatMessage)
    {
        sendChatMessage(player, args, "[QUEST]", ChatGeneral);
    }
    else if (func == q_sendAnnouncementMessage)
    {
        QStringList arg = args.split('|');

        if (arg.size() < 2)
        {
            q->exception("Function sendAnnouncementMessage, not enough arguments.");
        }
        else
        {
            sendAnnouncementMessage(player, arg[1], arg[0].toFloat());
        }
    }
    else if (func == q_teleportPlayer)
    {
        QStringList arg2 = args.split(',');
        if (arg2.size() < 3)
        {
            q->exception("Function teleportPlayer, not enough arguments.");
        }
        else
        {
            sendMove(player, arg2[0].toFloat(), arg2[1].toFloat(), arg2[2].toFloat());
        }
    }
    else if (func == q_teleportPlayerScene)
    {
        sendLoadSceneRPC(player, args);
    }
    else if (func == q_spawnInstantiate)
    {
        sendNetviewInstantiate(player);
    }
    else if (func == q_logToServer)
    {
        win.logMessage("[INFO] "+args);
    }
    else
    {
        win.logMessage("[ERROR][QUESTSCRIPT] Unknown function called.");
    }
}


Quest::Quest(QString path, Player *Owner)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly))
    {
        win.logMessage("[SEVERE] Error reading quest DB.");
        win.stopServer();
        throw std::exception();
    }

    QList<QString> lines = QString(file.readAll().replace('\r',"")).split('\n');

    owner = Owner;
    commands = new QList<QList<QString> >;
    name = new QString();
    descr = new QString();
    npc = new Pony(nullptr); // A NPC doesn't have an owner player !
    npc->id = 0;
    npc->netviewId = 0;
    id = 0;
    state = 0;
    eip = 0;

    /*
     * Tux3/mlkj says that people ignore the errors printed.
     * He then proceeded to change code here to make the server crash/stop
     * if there was an error on a line of a questScript.
     *
     * I call bullshit, so for [LoEWCT] we simply make the error messages
     * more notisable with [ERROR][QUESTSCRIPT] before the error message.
     *
     * It's more easier on people's eyes. Especially those familiar with
     * running minecraft servers. (4/20/2014) ~WapaMario63
     *
     */
    try
    {
        // Parse the metadata, add everything else as quest commands
        for (int i=0; i<lines.size(); i++)
        {
            QList<QString> line = lines[i].split(" ", QString::SkipEmptyParts);
            if (!line.size() || lines[i][0]=='#')
                continue;
            if (line[0] == "name")
                if (line.size()>=2)
                {
                    npc->name = lines[i].mid(line[0].size()+1);
                }
                else throw QString("[ERROR][QUESTSCRIPT] Error reading name, quest "+path);
            else if (line[0] == "scene")
                if (line.size()>=2)
                    npc->sceneName = lines[i].mid(line[0].size()+1).toLower();
                else throw QString("[ERROR][QUESTSCRIPT] Error reading scene, quest "+path);
            else if (line[0] == "ponyData")
                if (line.size()==2)
                {
                    if (ServerVersion::isBABSCon) npc->ponyData = QByteArray::fromBase64(line[1].toLatin1());
                    else if (ServerVersion::isAugust)
                    {
                        QByteArray ponyData = QByteArray::fromBase64(line[1].toLatin1());
                        /// Read the ponyData
                        unsigned strlen;
                        unsigned lensize=0;
                        {
                            unsigned char num3; int num=0, num2=0;
                            do {
                                num3 = ponyData[lensize]; lensize++;
                                num |= (num3 & 0x7f) << num2;
                                num2 += 7;
                            } while ((num3 & 0x80) != 0);
                            strlen = (uint) num;
                        }
                        int nameSize = strlen + lensize;
                        int ponyDataSize = ponyData.size() - nameSize;
                        if (ponyDataSize == 43)
                        {
                            win.logMessage(QString("[INFO] [QUESTSCRIPT] ponyData of quest %1 is in old BABScon format, converting...").arg(path));
                            ponyData += uint32ToData(0); // Member ID
                            ponyData.insert(nameSize+17+3, ponyData.mid(nameSize+17,3)); // Hair Color 2 (copy of color 1)
                        }
                        npc->ponyData = ponyData;
                    }
                }
                else throw QString("[ERROR][QUESTSCRIPT] Error reading ponyData, quest "+path);
            else if (line[0] == "pos")
                if (line.size()==4)
                {
                    bool ok1, ok2, ok3;
                    npc->pos = UVector(line[1].toFloat(&ok1), line[2].toFloat(&ok2),
                                        line[3].toFloat(&ok3));
                    if (!(ok1 && ok2 && ok3))
                        throw QString("[ERROR][QUESTSCRIPT] Error reading pos, quest "+path);
                }
                else throw QString("[ERROR][QUESTSCRIPT] Error reading pos, quest "+path);
            else if (line[0] == "rot")
                if (line.size()==5)
                {
                    bool ok1, ok2, ok3, ok4;
                    npc->rot = UQuaternion(line[1].toFloat(&ok1), line[2].toFloat(&ok2),
                                            line[3].toFloat(&ok3), line[4].toFloat(&ok4));
                    if (!(ok1 && ok2 && ok3 && ok4))
                        throw QString("[ERROR][QUESTSCRIPT] Error reading rot, quest "+path);
                }
                else throw QString("[ERROR][QUESTSCRIPT] Error reading rot, quest "+path);
            else if (line[0] == "wear")
            {
                for (int i=1; i<line.size(); i++)
                {
                    bool ok;
                    int itemId = line[i].toInt(&ok);
                    if (!ok)
                        throw QString("[ERROR][QUESTSCRIPT] Error reading wear, quest "+path);
                    WearableItem item;
                    item.id = itemId;
                    item.index = wearablePositionsToSlot(win.wearablePositionsMap[itemId]);
                    npc->worn << item;

                }
            }
            else if (line[0] == "shop")
              {
                // -> Jesus, I placed a 1 instead of an i in i<lines.size(); inside the for statement.
                // --> Ladies and Gentlemen, the biggest derp of all of [LoEWCT]'s development. For now.
                for (int i=1; i<line.size(); i++)
                  {
                    bool ok;
                    int itemId = line[i].toInt(&ok);
                    if(!ok)
                        throw QString("[ERROR][QUESTSCRIPT] Error Reading shop, quest "+path);
                    InventoryItem item;
                    item.id = itemId;
                    item.index = i-1;
                    item.amount = (quint32)-1;
                    npc->inv << item;
                  }
              }
            else if (line[0] == "questId")
                if (line.size()==2)
                {
                    id = line[1].toInt();

                    win.lastIdMutex.lock();
                    npc->id = 0;
                    npc->netviewId = id;
                    win.lastIdMutex.unlock();
                }
                else throw QString("[ERROR][QUESTSCRIPT] Error reading questId, quest "+path);
            else if (line[0] == "questName")
                if (line.size()>=2)
                    *name = lines[i].mid(line[0].size()+1);
                else throw QString("[ERROR][QUESTSCRIPT] Error reading questName, quest "+path);
            else if (line[0] == "questDescr")
                if (line.size()>=2)
                    *descr = lines[i].mid(line[0].size()+1);
                else throw QString("[ERROR][QUESTSCRIPT] Error reading questDescr, quest "+path);
            else
                commands->append(line);
        }
    }
    catch (QString& error)
    {
        win.logMessage(error);
        win.stopServer();
    }
}

QString Quest::concatAfter(QList<QString> list, int id) const
{
    QString result;
    if (list.size() <= id)
        return result;
    result = list[id];
    for (int i=id+1; i<list.size(); i++)
        result += " " + list[i];
    return result;
}

int Quest::findLabel(QString label)
{
    for (int i=0; i<commands->size(); i++)
        if ((*commands)[i].size()==2 && (*commands)[i][0] == "label" && (*commands)[i][1].trimmed()==label.trimmed())
            return i;
    return -1;
}

void Quest::logError(QString message)
{
    win.logMessage("[INFO] Error running quest script "+QString().setNum(id)
                   +", eip="+QString().setNum(eip)+" : "+message);
}

void Quest::setOwner(Player* Owner)
{
    owner = Owner;
}

void Quest::runScript()
{
    runScript(0);
}

void Quest::runScript(int Eip)
{
    for (eip=Eip; eip<commands->size();)
        if (doCommand(eip))
            eip++;
        else
            break;
}

bool Quest::doCommand(int commandEip)
{
    if (!owner)
    {
        win.logMessage("[INFO] Quest::doCommand called with no owner");
        return false;
    }

    //win.logMessage("Executing command "+QString().setNum(eip));

    QStringList command = (*commands)[commandEip];

    if (command[0] == "label")
        return true;
    else if (command[0] == "goto")
    {
        if (command.size() != 2)
        {
            exception("command variable 'goto' takes exactly one argument.");
            logError("goto takes exactly one argument");
            return false;
        }
        int newEip = findLabel(command[1]);
        if (newEip == -1)
        {
            exception("Label '"+command[1]+"' not found.");
            logError("label not found");
            return false;
        }
        eip = newEip;
    }
    else if (command[0] == "end")
    {
        sendEndDialog(owner);
        return false;
    }
    else if (command[0] == "say")
    {
        QString msg, npcName;
        QList<QString> answers;
        quint16 iconId=0;
        bool hasIconId=false;
        if (command.size() >= 2)
        {
            msg = concatAfter(command, 1);
            npcName = npc->name;
        }
        else
        {
            logError("say takes 2 arguments");
            return false;
        }

        // Parse answers, icon, and name
        for (int i=commandEip+1; i<commands->size();i++)
        {
            if ((*commands)[i][0] == "answer")
                answers << concatAfter((*commands)[i], 2);
            else if ((*commands)[i][0] == "sayName")
                npcName = concatAfter((*commands)[i], 1);
            else if ((*commands)[i][0] == "sayIcon")
            {
                bool ok;
                int id = (*commands)[i][1].toInt(&ok);
                if (!ok || id < 0)
                {
                    logError("invalid icon id");
                    return false;
                }
                hasIconId = true;
                iconId = id;
            }
            else
                break;
        }

        // Replace special variables
        msg.replace("{PLAYERNAME}", owner->pony.name);
        msg.replace("{PLAYER_BITS}", QString().setNum(owner->pony.nBits));
        msg.replace("{PLAYER_HEALTH}", QString().setNum(owner->pony.health));
        for (QString& s : answers)
        {
            s.replace("{PLAYERNAME}", owner->pony.name);
            s.replace("{PLAYER_BITS}", QString().setNum(owner->pony.nBits));
            s.replace("{PLAYER_HEALTH}", QString().setNum(owner->pony.health));
        }

        // Send
        sendBeginDialog(owner);
        // If there is a 2D sprite in the game's files for this NPC, use them.
        if (hasIconId)
        {
            sendDialogMessage(owner, msg, npcName, iconId);
            sendDialogMessage(owner, msg, npcName, iconId);
        }
        // If not, use the 3D view.
        else
        {
            sendDialogMessage(owner, msg, npcName, npc->netviewId, iconId);
            sendDialogMessage(owner, msg, npcName, npc->netviewId, iconId);
        }
        sendDialogOptions(owner, answers);
        return false; // We stop the script until we get a reply
    }
    else if (command[0] == "answer") // can only be ran after a say, sayName, sayIcon, or another answer
    {
        logError("trying to run answer command by itself");
        return false;
    }
    else if (command[0] == "sayName") // can only be ran after a say, sayIcon, answer, or another sayName
    {
        logError("trying to run sayName command by itself");
        return false;
    }
    else if (command[0] == "sayIcon") // Answer can only be ran after a say, sayName, answer, or another sayIcon
    {
        logError("trying to run sayIcon command by itself");
        return false;
    }
    else if (command[0] == "give")
    {
        if (command.size() != 3)
        {
            logError("give takes 2 arguments");
            return false;
        }
        bool ok1,ok2;
        int itemId = command[1].toInt(&ok1);
        int qty = command[2].toInt(&ok2);
        if (!ok1 || !ok2 || itemId<0)
        {
            logError("invalid arguments for command give");
            return false;
        }
        if (qty > 0)
            owner->pony.addInventoryItem(itemId, qty);
        else
            owner->pony.removeInventoryItem(itemId, -qty);
        //sendInventoryRPC(owner, owner->pony.inv, owner->pony.worn, owner->pony.nBits);
    }
    else if (command[0] == "giveBits")
    {
        if (command.size() != 2)
        {
            logError("giveBits takes 1 argument");
            return false;
        }
        bool ok1;
        int qty = command[1].toInt(&ok1);
        if (!ok1)
        {
            logError("invalid argument for command giveBits");
            return false;
        }
        if (qty<0 && (quint32)-qty > owner->pony.nBits)
            owner->pony.nBits = 0;
        else
            owner->pony.nBits += qty;
        sendSetBitsRPC(owner);
    }
    else if (command[0] == "setQuestState")
    {
        if (command.size() == 2)
        {
            bool ok1;
            int newState = command[1].toInt(&ok1);
            if (!ok1 || newState<0)
            {
                logError("invalid argument for command setQuestState");
                return false;
            }
            this->state = newState;
        }
        else if (command.size() == 3)
        {
            bool ok1,ok2;
            int newState = command[1].toInt(&ok1);
            int id = command[2].toInt(&ok2);
            if (!ok1 || !ok2 || newState<0 || id<0)
            {
                logError("invalid arguments for command setQuestState");
                return false;
            }
            for (Quest& quest : owner->pony.quests)
                if (quest.id == id)
                    quest.state = newState;
        }
        else
        {
            logError("setQuestState takes 1 or 2 arguments");
            return false;
        }
    }
    else if (command[0] == "hasItem")
    {
        int itemId, qty=1, yesEip, noEip;
        if (command.size() == 4)
        {
            bool ok1;
            itemId = command[1].toInt(&ok1);
            if (!ok1 || itemId<0)
            {
                logError("invalid arguments for command hasItem");
                return false;
            }
            yesEip = findLabel(command[2]);
            noEip = findLabel(command[3]);
        }
        else if (command.size() == 5)
        {
            bool ok1,ok2;
            itemId = command[1].toInt(&ok1);
            qty = command[2].toInt(&ok2);
            if (!ok1 || !ok2 || qty<=0 || itemId<0)
            {
                logError("invalid arguments for command hasItem");
                return false;
            }
            yesEip = findLabel(command[3]);
            noEip = findLabel(command[4]);
        }
        else
        {
            logError("hasItem takes 3 or 4 arguments");
            return false;
        }
        if (yesEip == -1)
        {
            logError("'yes' label not found");
            return false;
        }
        else if (noEip == -1)
        {
            logError("'no' label not found");
            return false;
        }
        if (owner->pony.hasInventoryItem(itemId, qty))
            eip=yesEip;
        else
            eip=noEip;
        return true;
    }
    else if (command[0] == "hasBits")
    {
        if (command.size() != 4)
        {
            logError("hasBits takes 3 arguments");
            return false;
        }
        int qty, yesEip, noEip;
        bool ok1;
        qty = command[1].toInt(&ok1);
        if (!ok1 || qty<=0)
        {
            logError("invalid arguments for command hasBits");
            return false;
        }
        yesEip = findLabel(command[2]);
        noEip = findLabel(command[3]);
        if (yesEip == -1)
        {
            logError("'yes' label not found");
            return false;
        }
        else if (noEip == -1)
        {
            logError("'no' label not found");
            return false;
        }
        eip = owner->pony.nBits >= (quint32)qty ? yesEip : noEip;
        return true;
    }
    else if (command[0] == "gotoIfState")
    {
        if (command.size() == 3)
        {
            int uState, destEip;
            bool ok1;
            uState = command[1].toInt(&ok1);
            destEip = findLabel(command[2]);
            if (!ok1 || uState<0)
            {
                logError("invalid arguments for command gotoIfState");
                return false;
            }
            else if (destEip<0)
            {
                logError("can't find dest label for command gotoIfState");
                return false;
            }
            if (this->state == uState)
                eip = destEip;
            return true;
        }
        else if (command.size() == 4)
        {
            int uState, questId, destEip;
            bool ok1,ok2;
            uState = command[1].toInt(&ok1);
            questId = command[2].toInt(&ok2);
            destEip = findLabel(command[3]);
            if (!ok1 || !ok2 || questId<0 || uState<0)
            {
                logError("invalid arguments for command gotoIfState");
                return false;
            }
            else if (destEip<0)
            {
                logError("can't find dest label for command gotoIfState");
                return false;
            }
            for (const Quest& quest : owner->pony.quests)
            {
                if (quest.id == questId)
                {
                    if (quest.state == uState)
                        eip = destEip;
                    return true;
                }
            }
            logError("invalid quest id for command gotoIfState");
            return false;
        }
        else
        {
            logError("gotoIfState takes 2 or 3 arguments");
            return false;
        }
    }
    else if (command[0] == "gotoAfterState")
    {
        if (command.size() == 3)
        {
            int uState, destEip;
            bool ok1;
            uState = command[1].toInt(&ok1);
            destEip = findLabel(command[2]);
            if (!ok1 || uState<0)
            {
                logError("invalid arguments for command gotoAfterState");
                return false;
            }
            else if (destEip<0)
            {
                logError("can't find dest label for command gotoAfterState");
                return false;
            }
            if (this->state >= uState)
                eip = destEip;
            return true;
        }
        else if (command.size() == 4)
        {
            int uState, questId, destEip;
            bool ok1,ok2;
            uState = command[1].toInt(&ok1);
            questId = command[2].toInt(&ok2);
            destEip = findLabel(command[3]);
            if (!ok1 || !ok2 || questId<0 || uState<0)
            {
                logError("invalid arguments for command gotoAfterState");
                return false;
            }
            else if (destEip<0)
            {
                logError("can't find dest label for command gotoAfterState");
                return false;
            }
            for (const Quest& quest : owner->pony.quests)
            {
                if (quest.id == questId)
                {
                    if (quest.state >= uState)
                        eip = destEip;
                    return true;
                }
            }
            logError("invalid quest id for command gotoAfterState");
            return false;
        }
        else
        {
            logError("gotoAfterState takes 2 or 3 arguments");
            return false;
        }
    }
    else if (command[0] == "runFunction")
    {
        QString msg, npcName;

        if (command.size() >= 3)
        {
            msg = concatAfter(command, 2);
            npcName = npc->name;
        }
        else
        {
            logError("runFunction takes 3 arguments: runFunction <Server Function|Lua Function> [function argument");
            return false;
        }

        // Replace special variables
        msg.replace("{PLAYERNAME}", owner->pony.name);
        msg.replace("{PLAYER_BITS}", QString().setNum(owner->pony.nBits));
        msg.replace("{PLAYER_HEALTH}", QString().setNum(owner->pony.health));

        if (command[1] == "sendChatMessage")
        {
            QuestFunction::launchFunction(QuestFunction::q_sendChatMessage, msg, owner);
        }
        else if (command[1] == "sendAnnouncement")
        {
            QuestFunction::launchFunction(QuestFunction::q_sendAnnouncementMessage, msg, owner);
        }
        /*else if (command[1] == "teleportToPosition")
        {
            QuestFunction::launchFunction(QuestFunction::q_teleportPlayer, msg, owner);
            eip--;
        }*/
        else if (command[1] == "teleportToScene")
        {
            QuestFunction::launchFunction(QuestFunction::q_teleportPlayerScene, msg, owner);
        }
        else
        {
            exception("Unknown function: "+command[1]);
            sendEndDialog(owner);
        }
    }
    else
    {
        exception("'"+command[0]+"' was not declared or an unknown command variable.");
        logError("unknown command : "+command[0]);
        return false;
    }
    return true;
}

void Quest::processAnswer(int answer)
{
    int curAnswer = 0;
    for (int i=eip+1; i<commands->size();i++)
    {
        if ((*commands)[i][0] == "answer")
        {
            if (curAnswer == answer)
            {
                int newEip = findLabel((*commands)[i][1]);
                if (newEip == -1)
                {
                    logError("label not found for answer "+QString().setNum(answer));
                    sendEndDialog(owner);
                    return;
                }
                eip = newEip;
                sendEndDialog(owner);
                runScript(eip);
                return;
            }
            else
                curAnswer++;
        }
        else if ((*commands)[i][0] == "sayName")
            continue;
        else if ((*commands)[i][0] == "sayIcon")
            continue;
        else
        {
            logError("answer "+QString().setNum(answer)+" not found");
            sendEndDialog(owner);
            return;
        }
    }
}

void Quest::processAnswer()
{
    for (int i=eip+1; i<commands->size();i++)
    {
        if ((*commands)[i][0] == "answer")
            continue;
        else if ((*commands)[i][0] == "sayName")
            continue;
        else if ((*commands)[i][0] == "sayIcon")
            continue;
        else
        {
            eip = i;
            sendEndDialog(owner);
            runScript(eip);
            return;
        }
    }
}
