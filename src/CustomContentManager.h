#ifndef CUSTOMCONTENTMANAGER_H
#define CUSTOMCONTENTMANAGER_H

#include <QObject>

class ContentManager : QObject
{
    Q_OBJECT
public:
    QStringList contentnames;


public:
    void sendCustomContent();
    void receiveCustomContent();
};

#endif // CUSTOMCONTENTMANAGER_H
