#include <stdio.h>
#include <SDL2/SDL.h>
#include "api/api.h"
#include "renderer.h"
#include <unistd.h>

SDL_Window *window;


static double get_scale() {
  return 1.0;
}


static void get_exe_filename(char *buf, int sz) {
  char path[512];
  sprintf(path, "/proc/%d/exe", getpid());
  int len = readlink(path, buf, sz - 1);
  buf[len] = '\0';
}


int main(int argc, char **argv) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  atexit(SDL_Quit);

#ifdef SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR /* Available since 2.0.8 */
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  window = SDL_CreateWindow(
    "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w * 0.8, dm.h * 0.8,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);
  ren_init(window);


  lua_State *L = luaL_newstate();
  luaL_openlibs(L);
  api_load_libs(L);


  lua_newtable(L);
  for (int i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i + 1);
  }
  lua_setglobal(L, "ARGS");

  lua_pushstring(L, "1.11");
  lua_setglobal(L, "VERSION");

  lua_pushstring(L, SDL_GetPlatform());
  lua_setglobal(L, "PLATFORM");

  lua_pushnumber(L, get_scale());
  lua_setglobal(L, "SCALE");

  char exename[2048];
  get_exe_filename(exename, sizeof(exename));
  lua_pushstring(L, exename);
  lua_setglobal(L, "EXEFILE");


  (void) luaL_dostring(L,
    "local core\n"
    "xpcall(function()\n"
    "  SCALE = tonumber(os.getenv(\"LITE_SCALE\")) or SCALE\n"
    "  PATHSEP = package.config:sub(1, 1)\n"
    "  EXEDIR = EXEFILE:match(\"^(.+)[/\\\\].*$\")\n"
    "  package.path = EXEDIR .. '/data/?.lua;' .. package.path\n"
    "  package.path = EXEDIR .. '/data/?/init.lua;' .. package.path\n"
    "  core = require('core')\n"
    "  core.init()\n"
    "  core.run()\n"
    "end, function(err)\n"
    "  print('Error: ' .. tostring(err))\n"
    "  print(debug.traceback(nil, 2))\n"
    "  if core and core.on_error then\n"
    "    pcall(core.on_error, err)\n"
    "  end\n"
    "  os.exit(1)\n"
    "end)");


  lua_close(L);
  SDL_DestroyWindow(window);

  return EXIT_SUCCESS;
}
