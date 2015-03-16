#ifndef MOB_H
#define MOB_H

#include "sceneEntity.h"
#include "statsComponent.h"
#include <QString>

class Mobzone;

class Mob : public SceneEntity, public StatsComponent
{
public:
    enum mobType : unsigned
    {
        birch_dryad,
        bunny,
        cockatrice,
        dragon,
        hornet,
        husky_diamond_dog,
        lantern_monster,
        timberwolf
    };
public:
    explicit Mob(Mobzone* zone);
    void setType(QString ModelName); ///< Don't change the SceneEntity model named directly
    virtual void kill() override; ///< Kills the mob. He'll respawn
    virtual void respawn() override; ///< Resets the mob
    virtual void takeDamage(unsigned amount) override; ///< Remove health, update the client, may kill the mob

private:
    static UVector getRandomPos(Mobzone* zone); ///< Returns a random position in this zone.
public:
    mobType type;
private:
    Mobzone* spawnZone;
    Mobzone* currentZone;
};

#endif // MOB_H
