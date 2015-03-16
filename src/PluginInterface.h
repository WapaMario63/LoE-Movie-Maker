#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#define loewct_plugininterface "me.WapaMario63.LoE_PrivateServer.PluginInterface"

#include <QString>

class PluginInterface
{
public:
  /*
  * Returns the name of the Plugin
  */
  virtual QString pluginName() const = 0;

  /*
  * Loads Plugin Content
  */
  virtual void LoadPluginContent() const = 0;
};

Q_DECLARE_INTERFACE( PluginInterface, loewct_plugininterface)

#endif // PLUGININTERFACE_H
