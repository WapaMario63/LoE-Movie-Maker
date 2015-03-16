#include "lib.h"

#include <QApplication>
#include <QTranslator>

#include "chat_widget.h"
#include "settings_widget.h"

/*
lib::lib()
{
}

lib::~lib()
{
}

void lib::main()
{
    int argc=0;

    QApplication a(argc,(char**)0);
    Widget win;
    chat_widget cwin;
    settings_widget swin;

    // Windows DLL hell fix
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    a.addLibraryPath("platforms");

    QString locale = QLocale::system().name().section('_', 0, 0);

    // We are now making this user friendly for other language speakers!
    // Like me with spanish. [yay intensifies]
    // -> (6/10/2014) About that, there is going to be a whole work in adding 'tr' to all log messages.
    // -> Thing is, doing this causes errors when doing it outside widget.cpp. Shit isn't it?
    QTranslator translator;
    if (!translator.load("languages/"+locale))
      {
        qDebug() << "Unable to load language translation";
        win.logMessage("[INFO] No language translations found or unable to load them.");
      }
    else
    {
      qDebug() << "Language translation loaded";
      win.logMessage("[INFO] Loaded language translation: "+locale);
    }
    a.installTranslator(&translator);
}
*/
