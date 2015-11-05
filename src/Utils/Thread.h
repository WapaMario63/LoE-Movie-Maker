#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QString>

class LoEWCT_Thread : public QThread
{
    Q_OBJECT
public:
    explicit LoEWCT_Thread(QObject *parent = 0);

    void run();
signals:
    void CommandSend(QString); // For sending a command to the console from another thread
                               // Example: the form.h UI interface.
                               // We don't directly input the command and then submit it anymore.
                               // We only submit it.
};

#endif // THREAD_H
