#include "luascript.h"

#include "message.h"

// Convert known functions to something that can be executed in Lua
/*int LuaScript::LUA_logMessage(lua_State *L)
{
    QString s = lua_tostring(L,1);
    win.logMessage(s);

    return 0;
}

int LuaScript::LUA_sendChatMessage(lua_State *L)
{
    QString txt = lua_tostring(L,1);
    QString author = lua_tostring(L,2);
    for (int i=0; i<win.udpPlayers.size(); i++)
    {
        sendChatMessage(win.udpPlayers[i], txt, author, ChatGeneral);
    }

    return 0;
}

LuaScript::LuaScript()
{
}

int LuaScript::LUA_main()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_register(L, "logMessage", LuaScript::LUA_logMessage);
    lua_register(L, "sendGlobalChat", LuaScript::LUA_sendChatMessage);
    luaopen_debug(L); // Debug
    luaL_loadfile(L,"plugins/main.lua");
    lua_call(L,0,0);
    return 0;
}*/
