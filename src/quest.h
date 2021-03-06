#ifndef QUEST_H
#define QUEST_H

#include <QString>
#include <QList>

class Player;
class Pony;


struct QuestFunction
{
    enum function
    {
        q_sendChatMessage,
        q_sendAnnouncementMessage,
        q_sendMessageBox,
        q_teleportPlayer,
        q_teleportPlayerScene,
        q_spawnInstantiate,
        q_spawnNpc,
        q_logToServer,
        q_downloadCustomContent
    };

    static void launchFunction(function func, QString args, Player *player);
};

// Quests are bound to one NPC
// They define the NPC, what he say, and the reactions.
struct Quest
{
public:
    Quest(QString path, Player* Owner);
    void runScript(); // Runs the script from the start until the end or the next dialog
    bool doCommand(int commandEip); // Runs the command at eip. Returns false if we should stop running the script (e.g because we're waiting for an answer)
    void processAnswer(int answer); // Called when a client picks an answer in a dialog
    void processAnswer(); // Called when a client clicks on a dialog with no proposed answer
    int findLabel(QString label); // Returns the eip of this label
    void logError(QString message); // Logs errors while running a quest script
    void exception(QString msg);
    void setOwner(Player* Owner);

private:
    void runScript(int Eip); // Runs the script until the end or the next dialog
    QString concatAfter(QList<QString> list, int id) const; // Concatenate the strings after id in the list

public:
    QList<QList<QString> >* commands; // List of commands and their arguments, parsed from the quest file.
    quint16 eip; // Instruction pointer inside the commands.
    quint16 state; // State (progress) of the quest.
    quint16 id; // Unique id of the quest.
    QString* name; // Name of the quest.
    QString* descr; // Description of the quest.
    Pony* npc; // NPC of this quest's script
    Player* owner; // Player who owns this quest save / runs the script
};

#endif // QUEST_H
