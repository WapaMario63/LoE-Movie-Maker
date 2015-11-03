#include <QtWidgets/QApplication>
#include "widget.h"
#include <QTranslator>
#include <QDir>
//#include "chat_widget.h"
#include "settings_widget.h"
#include "luascript.h"
//#include "LuaSrc/lua.hpp"
#include <iostream>
#include "lib.h"
#include "form.h"
#include <demo.h>
#include <QMessageBox>

//#if IS_APP
int argc=0;

QApplication a(argc,(char**)0);
Widget win;
//chat_widget cwin;
settings_widget swin;
Form lwin;
Demo dem;
QTextStream cout(stdout);
QTextStream cin(stdin);
QTextStream cerr(stderr);

int main(int, char**)
{
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
        qDebug() << "[QDebug] No language translations found or unable to load them.";
        win.logMessage("[INFO] No language translations found or unable to load them.");
      }
    else
    {
        qDebug() << "[QDebug] Language translation loaded";
        win.logMessage("[INFO] Loaded language translation: "+locale);
    }
    a.installTranslator(&translator);

    //cwin.show();
    //win.show();
    a.processEvents();
    lwin.show();
    lwin.searchClientPath();
    win.startServer();

    return a.exec(); // win's dtor will quick_exit (we won't run the atexits)
}
/*#if IS_LIB

Widget win;

void Widget::useLibraryMode()
{
    lib LIB;
    LIB.main();
}
#endif
*/
