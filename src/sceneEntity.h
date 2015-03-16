#ifndef SCENEENTITY_H
#define SCENEENTITY_H


#include "dataType.h"
#include <QString>

struct SceneEntity
{
public:
    SceneEntity();

public:
    // Infos
    QString modelName;
    quint16 id;
    quint16 netviewId;

    // Pos
    QString sceneName;
    UVector pos;
    UQuaternion rot;
};


#endif // SCENEENTITY_H
