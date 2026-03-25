#include "aki/config/lua_api.h"
#include "aki/server.h"
#include "aki/utils/log.h"

namespace aki {

namespace config {

LuaAPI::LuaAPI(Server *server) :
    server(server),
    L(nullptr) {
}

LuaAPI::~LuaAPI() {
    if (L) {
        lua_close(L);
    }
}

bool LuaAPI::init() {
    L = luaL_newstate();
    if (!L) {
        utils::Log::error("Failed to create Lua state");
        return false;
    }

    luaL_openlibs(L);

    // 注册API函数
    // 这里将注册各种API函数到Lua环境

    utils::Log::info("Lua API initialized");
    return true;
}

bool LuaAPI::load_script(const std::string &path) {
    if (!L) {
        return false;
    }

    int status = luaL_dofile(L, path.c_str());
    if (status) {
        utils::Log::error("Failed to load Lua script: " + std::string(lua_tostring(L, -1)));
        lua_pop(L, 1);
        return false;
    }

    utils::Log::info("Lua script loaded: " + path);
    return true;
}

lua_State *LuaAPI::get_state() const {
    return L;
}

} // namespace config
} // namespace aki
