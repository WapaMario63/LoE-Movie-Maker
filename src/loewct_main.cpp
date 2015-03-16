#include "loewct_main.h"
#include "message.h"
#include "character.h"
#include "widget.h"
#include "serialize.h"
#include "receiveChatMessage.h"
#include <QSettings>
#include <QDateTime>
#include <QTime>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

// Added ability to write and read files, this is just so plugin makers can generate and read config files if needed.
// Not used in main server code, because it's TOO simple.
void LoEWCT_Main::write(QString filename, QString contents)
{
    QFile mFile(filename);

    if(!mFile.open(QFile::ReadOnly | QFile::Text))
    {
        win.logMessage("[INFO] The file: "+filename+" could not be opened for writting.");
        return;
    }

    //QTextStream out(&mFile);
    //out << contents;

    mFile.flush();
    mFile.close();
}

void LoEWCT_Main::read(QString filename, quint64 line)
{
    QFile mFile(filename);

    if(!mFile.open(QFile::WriteOnly | QFile::Text))
    {
        win.logMessage("[INFO] The file: "+filename+" could not be opened for reading.");
        return;
    }

    QTextStream in(&mFile);
    QString mText = in.readLine(line);

    mFile.close();
}

// These are just foundations on what is about to come in the future.

// Want to help me out? PM me in Ponyforums.net ~WapaMario63

void LoEWCT_Main::bannedPlayerHandler()
{

}

