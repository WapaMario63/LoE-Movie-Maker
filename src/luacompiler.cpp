/*#include "luacompiler.h"

extern "C"{
#include "LuaSrc/lua.h"
#include "LuaSrc/lauxlib.h"

#include "LuaSrc/ldo.h"
#include "LuaSrc/lfunc.h"
#include "LuaSrc/lmem.h"
#include "LuaSrc/lobject.h"
#include "LuaSrc/lopcodes.h"
#include "LuaSrc/lstring.h"
#include "LuaSrc/lundump.h"
}

#include <QList>

QList<const char*> files;

void begin()
{
    files.clear();
}

void add_file(const char *filename)
{
    files.push_back(filename);
}

#define setPtValue(L,obj,x) \
    { TValue *i_o = (obj); \
      i_o->value_.gc=cast(GCObject *, (x)); i_o->tt_=LUA_TPROTO; \
      checkliveness(G(L),i_o); }

#define toProto(L,i) (clvalue(L->top+(i))->l.p)

int writer(lua_State* L, const void* p, size_t size, void* u)
{
    return (fwrite(p,size,1,(FILE*)u)!=1) && (size !=0);
}

int compile(const char *destFile, bool stripping)
{
    lua_State *L;
    L=luaL_newstate();

    QList<const char*>::iterator it;
    for (it=files.begin(); it!=files.end(); ++it)
    {
        luaL_loadfile(L, *it);
    }

    int n = files.size();
    int i, pc;
    Proto *f = luaF_newproto(L);
    setPtValue(L,L->top,f); incr_top(L);
    f->source = luaS_newliteral(L,"compiled scripts");
    f->maxstacksize = 1;
    pc = 2*n+1;
    f->code = luaM_newvector(L,pc,Instruction);
    f->sizecode = pc;
    f->p = luaM_newvector(L,n,Proto*);
    f->sizep = n;
    pc = 0;
    for ( i=0; i<n; ++i)
    {
        f->p[i] = toProto(L,i-n-1);
        f->code[pc++] = CREATE_ABx(OP_CLOSURE,0,i);
        f->code[pc++] = CREATE_ABC(OP_CALL,0,1,1);
    }
    f->code[pc++] = CREATE_ABC(OP_RETURN,0,1,0);
    FILE *D = fopen(destFile,"wb");
    lua_lock(L);
    luaU_dump(L,f,writer,D,stripping);
    lua_unlock(L);
    fclose(D);
    return 0;
}

// Bind
int ORE_Lua_Begin(lua_State *L)
{
    begin();
    return 0;
}

int ORE_Lua_AddFile(lua_State *L)
{
    add_file(lua_tostring(L,1));
    return 0;
}

int ORE_Lua_Compile(lua_State *L)
{
    compile(lua_tostring(L,1), lua_toboolean(L,2));
    return 0;
}

void RegisterCompile(lua_State *L)
{
    lua_register(L,"Begin",ORE_Lua_Begin);
    lua_register(L,"AddFile",ORE_Lua_AddFile);
    lua_register(L,"Compile",ORE_Lua_Compile);
}*/
