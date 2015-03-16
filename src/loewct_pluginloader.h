#ifndef LOEWCT_PLUGINLOADER_H
#define LOEWCT_PLUGINLOADER_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QPluginLoader>
#include "PluginInterface.h"
#include "widget.h"

class loewct_pluginLoader : public QObject
{
  Q_OBJECT
public:
  explicit loewct_pluginLoader(QObject *parent = 0);

signals:

public slots:
  void LoadAllPlugins();
  void LoadPlugin(QString filename);
};

#endif // LOEWCT_PLUGINLOADER_H
