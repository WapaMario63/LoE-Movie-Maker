#ifndef LOEWCT_MAIN_H
#define LOEWCT_MAIN_H

#include <QString>
#include <QObject>
#include <QByteArray>
#include <QList>
#include <QFile>
#include "serialize.h"
#include "widget.h"
#include "message.h"
#include "items.h"
#include "utils.h"


/* This file is reserved for future use same with the .cpp counterpart.
 *
 * Guess what things might come in here. :3
 *
 * HINT: look at the includes and empty functions.
 */

class LoEWCT_Main
{
public:

    //loewct::hideAndSeek();
    void bannedPlayerHandler();
    //void bannedPlayerFile(QString path, Player* player);
    void write(QString filename, QString contents);
    void read(QString filename, quint64 line);

};

extern LoEWCT_Main loewct;

#endif // LOEWCT_MAIN_H
