#pragma once

#include <lua.hpp>

namespace aki {

class Server;

namespace config {

class LuaAPI {
private:
    Server *server;
    lua_State *L;

public:
    LuaAPI(Server *server);
    ~LuaAPI();

    bool init();
    bool load_script(const std::string &path);
    lua_State *get_state() const;
};

} // namespace config
} // namespace aki
