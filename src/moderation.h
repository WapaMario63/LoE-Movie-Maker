#ifndef MODERATION_H
#define MODERATION_H

#include <QObject>
#include <utils.h>
#include <character.h>

// ONLY AVAILABLE IN BABSCON15 WITH A FEW EXCEPTIONS

class Moderation : public QObject
{
    Q_OBJECT
public:
    explicit Moderation(QObject *parent = 0);
    ~Moderation();

    // Player Access Level list
    enum AccessLevel
    {
        Admin = 0xFF,
        Default = 0, // Invalid!
        Implementor = 0x19,
        Moderator = 30,
        Player = 1, // If someone is this, none of these functions will work on them, except report
        TeamMember = 20
    };

    // Debug Level
    enum DebugLevel
    {
        Error = 2,
        Info = 0,
        Warning = 1
    };

    // Well you know something? Let's log some LoE Debug stuff to the mods/admins
    void debugError(DebugLevel errorlevel, QString error);
    void debugLine(int lineNumber, QString line);

    // From Utils.h
    // Might move this to the messages files, since I am kinda cracking up some little things.
    //virtual void MessageBox(QString title, QString msg, Utilities::MessageBoxButtons btns);

    // Moderator/Admin Only Functions
    void renamePony(QString oldName, QString newName);

    // Admin only Functions
    void deleteAdmin(QString username, QString ponyname);

    // Functions that anyone can execute in the server
    void whoIsThisPony(QString ponyname);
    void whoIsThisPony(int ponyId);

signals:

public slots:
};

#endif // MODERATION_H
