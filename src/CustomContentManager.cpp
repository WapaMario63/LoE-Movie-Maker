#include "CustomContentManager.h"

#include <QDir>
#include <QFile>

void ContentManager::sendCustomContent()
{
    QDir content("custom");
    content.cd("BY_SERVER");


}
