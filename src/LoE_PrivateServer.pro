#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T21:58:54
#
#-------------------------------------------------

QT       += core gui network widgets multimedia

TARGET = LoE_Movie_Maker
TEMPLATE = app
VERSION = 0.0.1

win32:RC_FILE = loeIcon.rc

CONFIG += console

SOURCES += main.cpp \
        widget.cpp \
	tcp.cpp \
	udp.cpp \
	messages.cpp \
        Utils/utils.cpp \
	pingTimeout.cpp \
	character.cpp \
	scene.cpp \
        Utils/dataType.cpp \
	sync.cpp \
    receiveMessage.cpp \
    sendMessage.cpp \
    quest.cpp \
    Utils/serialize.cpp \
    items.cpp \
    receiveack.cpp \
    receivechatmessage.cpp \
    mob.cpp \
    mobsparser.cpp \
    mobstats.cpp \
    settings_widget.cpp \
    skill.cpp \
    skillparser.cpp \
    animation.cpp \
    animationparser.cpp \
    settings.cpp \
    luascript.cpp \
    luacompiler.cpp \
    lib.cpp \
    form.cpp \
    modmanager.cpp \
    shops.cpp \
    permissions.cpp \
    gamemode.cpp \
    CustomContentManager.cpp \
    moderation.cpp \
    demo.cpp \
    NpcManager.cpp \
    console.cpp \
    consoleCommandHandler.cpp \
    Utils/Thread.cpp

HEADERS  += widget.h \
	character.h \
	message.h \
        Utils/utils.h \
	scene.h \
        Utils/dataType.h \
	sync.h \
    quest.h \
    Utils/serialize.h \
    items.h \
    sendMessage.h \
    receiveack.h \
    receiveChatMessage.h \
    mob.h \
    sceneEntity.h \
    mobzone.h \
    mobsparser.h \
    mobstats.h \
    packetloss.h \
    receiveMessage.h \
    PluginInterface.h \
    settings_widget.h \
    skill.h \
    skillparser.h \
    statsComponent.h \
    animation.h \
    animationparser.h \
    settings.h \
    luascript.h \
    luacompiler.h \
    lib.h \
    form.h \
    modmanager.h \
    shops.h \
    permissions.h \
    gamemode.h \
    CustomContentManager.h \
    moderation.h \
    demo.h \
    NpcManager.h \
    console.h \
    loewct.h \
    Utils/Thread.h

FORMS    += widget.ui \
    settings_widget.ui \
    form.ui \
    modmanager.ui \
    demo.ui \
    NpcManager.ui

# include coreservices (required for timestamps) only on mac
macx {
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreServices.framework/
    LIBS += -framework CoreServices
}

CONFIG += c++11

RESOURCES += resources.qrc
