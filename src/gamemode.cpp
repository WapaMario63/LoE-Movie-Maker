#include "gamemode.h"
#include "widget.h"
#include "message.h"

void GameMode::setGamemode(Type gm) const
{
    /*switch (gm)
    {
    case gm_loe_vanilla:
        // Restoring things back

    case gm_loe_deathmatch:
        // Disable and save things from the vanilla gamemode


        // Call the gamemode loader
        //GmDeathMatch::GmLoad();
    }*/
}

void GmDeathMatch::GmLoad()
{

}

void GmDeathMatch::removeVortexes(bool n)
{

}

void GmDeathMatch::setLobbyMap(QString map)
{
    lobbyMap = map.toLower();
}

void GmDeathMatch::voteForMap()
{
    //GameMode::votemap = true;
    win.logMessage("[INFO] Map Vote started");

    QTimer* votetime;
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        votetime->start(20000);
        sendAnnouncementMessage(win.udpPlayers[i], "Vote for the next map! (See Chat)", 20);

        sendChatMessage(win.udpPlayers[i], "Vote For map! Type !vote <number> to choose the map!\n1 ponyville\n2 canterlot\n3 cloudsdale\n4 sweetappleacres", "[LoEDM]", ChatSystem);

        //int map1, map2, map3, map4;

        /*switch (GameMode::votenum)
        {
        case 1: votenum += map1;
        case 2: votenum += map2;
        case 3: votenum += map3;
        case 4: votenum += map4;
        }*/
    }
}
