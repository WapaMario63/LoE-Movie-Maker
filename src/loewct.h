#ifndef LOEWCT
#define LOEWCT

/*
 * Includes for Library version
 */

#define IS_APPLICATION
/*#define IS_SHARED_LIBRARY
  #define IS_STATIC_LIBRARY*/

#ifdef IS_SHARED_LIBRARY || IS_STATIC_LIBRARY
#include <UI/form.h>
#include <Server/console.h>
#include <Server/widget.h>
#include <Player/character.h>
#include <Player/quest.h>
#include <Messages/message.h>
#include <Player/scene.h>
#include <Utils/dataType.h>
#include <Utils/serialize.h>
#endif

/***
 * Macros that are made to save time on things that are typed in lots of areas in the code.
 */

/*
 * Since this is a for loop, you need to add the '{ }'
 * after it and put the code that interacts with all
 * players. don't forget to use VAR_ALL_PLAYERS.
 */
#define FUNCTION_SELECT_ALL_PLAYERS for (int PLR=0; PLR<win.udpPlayers.size(); PLR++)
/*
 * Used to check if there is actually a player set, used mostly in the input handler.
 */
#define isPlayerNotSet() if (cmd.cmdPlayer == nullptr || cmd.cmdPlayer->IP == "") \
                         { cmd.logInfoMsg("No Player Set. Type 'setPlayer [id]|[IP:port]' to set one."); \
                           return; }

/*
 * Can only be used inside FUNCTION_SELECT_ALL_FLAYERS
 */
#define VAR_ALL_PLAYERS win.udpPlayers[PLR]

#endif // LOEWCT

