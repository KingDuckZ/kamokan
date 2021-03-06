function (load_and_strip_lua_script file_path out_prefix)
	file(READ ${file_path} retval)
	string(REGEX REPLACE "[ \t]*=[ \t]*" "=" retval "${retval}")
	string(REGEX REPLACE ",[ \t]*" "," retval "${retval}")
	string(REGEX REPLACE "(^|\n)[ \t]+" "\\1" retval "${retval}")
	string(LENGTH "${retval}" retval_length)
	set(${out_prefix}_pastie "${retval}" PARENT_SCOPE)
	set(${out_prefix}_pastie_length "${retval_length}" PARENT_SCOPE)
	unset(retval)
	unset(retval_length)
endfunction()

load_and_strip_lua_script(${SOURCE_DIR}/retrieve_pastie.lua retrieve)
load_and_strip_lua_script(${SOURCE_DIR}/save_pastie.lua save)
load_and_strip_lua_script(${SOURCE_DIR}/add_highlighted_pastie.lua add_highlighted)

set(lua_scripts_for_redis_content "//File autogenerated by cmake, changes will be lost
#include <cstddef>
namespace kamokan {
extern const char g_save_script[] = R\"lua(${save_pastie})lua\";
extern const char g_load_script[] = R\"lua(${retrieve_pastie})lua\";
extern const char g_add_highlighted_script[] = R\"lua(${add_highlighted_pastie})lua\";
extern const std::size_t g_save_script_size = ${save_pastie_length};
extern const std::size_t g_load_script_size = ${retrieve_pastie_length};
extern const std::size_t g_add_highlighted_script_size = ${add_highlighted_pastie_length};
} //namespace kamokan
")

file(WRITE
	include/lua_scripts_for_redis.cpp
	"${lua_scripts_for_redis_content}"
)

unset(lua_scripts_for_redis_content)
unset(save_pastie_length)
unset(retrieve_pastie_length)
unset(add_highlighted_pastie_length)
unset(save_pastie)
unset(retrieve_pastie)
unset(add_highlighted_pastie)
