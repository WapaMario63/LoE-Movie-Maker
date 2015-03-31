#-------------------------------------------------
#
# Project created by QtCreator 2013-06-21T21:58:54
#
#-------------------------------------------------

QT       += core gui network widgets multimedia

TARGET = LoE_Movie_Maker
TEMPLATE = app

win32:RC_FILE = loeIcon.rc

CONFIG += console

SOURCES += main.cpp \
        widget.cpp \
	tcp.cpp \
	udp.cpp \
	messages.cpp \
	utils.cpp \
	pingTimeout.cpp \
	character.cpp \
	scene.cpp \
	dataType.cpp \
	sync.cpp \
    receiveMessage.cpp \
    sendMessage.cpp \
    serverCommands.cpp \
    quest.cpp \
    serialize.cpp \
    items.cpp \
    loewct_main.cpp \
    receiveack.cpp \
    receivechatmessage.cpp \
    mob.cpp \
    mobsparser.cpp \
    mobstats.cpp \
    loewct_pluginloader.cpp \
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
    gamemode.cpp

HEADERS  += widget.h \
	character.h \
	message.h \
	utils.h \
	scene.h \
	dataType.h \
	sync.h \
    quest.h \
    serialize.h \
    items.h \
    sendMessage.h \
    loewct_main.h \
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
    loewct_pluginloader.h \
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
    gamemode.h

FORMS    += widget.ui \
    settings_widget.ui \
    form.ui \
    modmanager.ui

# include coreservices (required for timestamps) only on mac
macx {
    QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreServices.framework/
    LIBS += -framework CoreServices
}

CONFIG += c++11

RESOURCES += \
    resources.qrc
