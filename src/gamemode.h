#ifndef GAMEMODE_H
#define GAMEMODE_H

#include <QObject>

struct GameMode
{
public:
    enum Type
    {
        gm_loe_vanilla,
        gm_loe_deathmatch,
        gm_loe_hideandseek,
        gm_loe_race
    };
    bool votemap;
    int votenum;

    void setGamemode(Type gm) const;
protected:
    virtual void GmLoad();
};

class GmDeathMatch //: QObject
{
    //Q_OBJECT
public:
    GmDeathMatch();
    //~gamemode();

    QString lobbyMap;
protected:
    virtual void GmLoad();
public:
    void removeVortexes(bool n);
    void setLobbyMap(QString map);
    void voteForMap();
    void addScore(int s);

    bool onPlayerKill();

};

class GmRace
{
public:
    QString lobbyMap;
    bool useButtkart;

    void spawnBoundarieNPC();
    void spawnStartLineNPC();
    void spawnFinishLineNPC();

    bool lockPlayerMovement();

    void drawBoundaries();

protected:
    virtual void GmLoad();
};

#endif // GAMEMODE_H
