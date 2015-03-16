#include "animationparser.h"
#include "animation.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonDocument>

AnimationParser::AnimationParser(QString filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) throw "[SEVERE] AnimationParser: Unable to open Json Document";

    QByteArray data = file.readAll();

    // Remove Comments
    while (true)
    {
        int start=data.indexOf("/*");
        if (start == -1)    break;
        int end = data.indexOf("*/");
        if (end < start+2)  break;
        if (end != -1)      data.remove(start, end-start+2);
    }

    QJsonParseError jsonError;
    QJsonArray json = QJsonDocument::fromJson(data,&jsonError).array();
    if (jsonError.error != QJsonParseError::NoError)
        throw QString("[SEVERE] AnimationParser Error: "+jsonError.errorString()+QString(" at %1").arg(jsonError.offset));

    for (QJsonValue animationValue : json)
    {
        if (!animationValue.isObject()) throw "[SEVERE] AnimationParser: Invalid format (not an object)";

        QJsonObject animationObject = animationValue.toObject();
        Animation anim;
        if (animationObject.contains("AnimId")) anim.id = animationObject["AnimId"].toDouble();
        if (animationObject.contains("SId")) anim.name = animationObject["SId"].toString();
        Animation::animations[anim.name] = anim;
    }
}
