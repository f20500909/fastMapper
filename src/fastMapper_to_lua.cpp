#include <lua.hpp>
#include "fastMapper.hpp"



//luaL_Reg结构体的第一个字段为字符串，在注册时用于通知Lua该函数的名字。
//第一个字段为C函数指针。
//结构体数组中的最后一个元素的两个字段均为NULL，用于提示Lua注册函数已经到达数组的末尾。

extern "C" int run(lua_State *L) {
    unsigned out_height;
    unsigned out_width;
    unsigned symmetry;
    unsigned N;
    int channels;
    int log;
    string input_data;
    string output_data;
    string type;
    bool res =false;

    int cnt = 1;
    out_height = lua_tonumber(L, cnt++);
    out_width = lua_tonumber(L, cnt++);
    symmetry = lua_tonumber(L, cnt++);
    N = lua_tonumber(L, cnt++);
    channels = lua_tonumber(L, cnt++);
    log = lua_tonumber(L, cnt++);
    input_data = std::string(lua_tostring(L, cnt++));
    output_data = std::string(lua_tostring(L, cnt++));
    type = std::string(lua_tostring(L, cnt++));

    std::cout<<out_width<<endl;
    std::cout<<type<<endl;

    res = single_run(out_height, out_width, symmetry, N, channels, log, input_data, output_data, type);

    lua_pushnumber(L, res);//将平均数压如栈，供lua获取

    return 1;//返回返回值个数，通知lua应该在栈里取几个值作为返回结果
}


static luaL_Reg cMethods[] = {
        {"run", run},
        {NULL, NULL}
};


//该C库的唯一入口函数。其函数签名等同于上面的注册函数。见如下几点说明：
//1. 我们可以将该函数简单的理解为模块的工厂函数。
//2. 其函数名必须为luaopen_xxx，其中xxx表示library名称。Lua代码require "xxx"需要与之对应。
//3. 在luaL_register的调用中，其第一个字符串参数为模块名"xxx"，第二个参数为待注册函数的数组。
//4. 需要强调的是，所有需要用到"xxx"的代码，不论C还是Lua，都必须保持一致，这是Lua的约定，
//   否则将无法调用。
extern "C" __declspec(dllexport)
int luaopen_fastmapper_dll(lua_State *L) {
    luaL_register(L, "fastmapper_dll", cMethods);
    return 1;
}