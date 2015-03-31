#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>

struct GameMode
{
    enum Type
    {
        gm_loe_vanilla,
        gm_loe_deathmatch,
        gm_loe_hideandseek,
    };
    bool votemap;
    int votenum;

    void setGamemode(Type gm) const;
};

class GmDeathMatch //: QObject
{
    //Q_OBJECT
public:
    GmDeathMatch();
    //~gamemode();

    QString lobbyMap;

    void GmLoad();

    void removeVortexes(bool n);
    void setLobbyMap(QString map);
    void voteForMap();
    void addScore(int s);

    bool onPlayerKill();

};

#endif // GAMEMODE_H
