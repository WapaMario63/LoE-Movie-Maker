#ifndef RECEIVECHATMESSAGE_H
#define RECEIVECHATMESSAGE_H

/**
  This file handles the raw chat RPC messages we receive.
  In other words, it handles all the chat commands

  It's a lot cleaner, so yeah.
**/

#include <QByteArray>

#include "settings.h"

class Player;

void receiveChatMessage(QByteArray msg, Player* player); // Normal Chat
void receiveModChatMessage(QByteArray msg, Player* player, bool isMod); // Moderator Chat

#endif // RECEIVECHATMESSAGE_H
