#include "widget.h"
#include "utils.h"
#include "message.h"

void Widget::checkPingTimeouts()
{
    //win.logMessage("CHECKING PING TIMEOUT :");
    for (int i=0;i<udpPlayers.size();i++)
    {
        //if (!udpPlayers[i].connected || !udpPlayers[i].port)
        //    continue;

        // check ping timeout
        int time = (timestampNow()-udpPlayers[i]->lastPingTime);
        //win.logMessage(QString().setNum(time)+"s");
        if (time > pingTimeout)
        {
            logMessage("[INFO] UDP: Ping timeout ("+QString().setNum(((int)timestampNow()-udpPlayers[i]->lastPingTime))+"s) for "
                       +QString().setNum(udpPlayers[i]->pony.netviewId)+" (player "+udpPlayers[i]->name+")");
            udpPlayers[i]->connected = false;
            sendMessage(udpPlayers[i], MsgDisconnect, "Lost Connection. \n\n(Ping Time Out)");
            Player::disconnectPlayerCleanup(udpPlayers[i]);
        }

        // Send a ping to prevent the client timing out on us

        // Disabling this for a moment. Hope this fixes the infinite loading. ~WapaMario63
        // -> Did not do it, but it did make the server stop outputting the pong received thing, so, what now? :S
        //sendMessage(udpPlayers[i], MsgPing);
    }
}
