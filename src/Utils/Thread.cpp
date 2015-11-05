#include "Thread.h"
#include "form.h"

LoEWCT_Thread::LoEWCT_Thread(QObject *parent)
    : QThread(parent)
{

}

void LoEWCT_Thread::run()
{
    emit CommandSend(lwin.commandString);
}
