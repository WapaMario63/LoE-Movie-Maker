#include "loewct_pluginloader.h"

loewct_pluginLoader::loewct_pluginLoader(QObject *parent) : QObject(parent)
{
}

void loewct_pluginLoader::LoadAllPlugins()
{
    QDir path(QDir::currentPath() + "/plugins");

    // Display current path and switch to the plugin dir
    win.logMessage("[INFO] Current Plugin directory:"+ path.currentPath());

    // Try to load the plugins
    foreach (QString filename, path.entryList(QDir::Files))
      {
        LoadPlugin(path.absolutePath() +"/"+ filename);
      }
}

void loewct_pluginLoader::LoadPlugin(QString filename)
{
    win.logMessage("[INFO] Loading: "+ filename);
    QPluginLoader loader(filename);
    QObject *possiblePlugin = loader.instance();

    if(possiblePlugin)
      {
        PluginInterface *plugin = qobject_cast<PluginInterface*>( possiblePlugin );
        if (plugin)
          {
            win.logMessage("[INFO] Loaded: "+ plugin->pluginName());

            // Tell the plugin to load it's contents
            plugin->LoadPluginContent();
          }
      }
    else
      {
        win.logMessage("[SEVERE] Error loading plugin. NOT A PLUGIN: "+ filename +" = "+ loader.errorString());
      }
}
