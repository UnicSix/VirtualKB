#include <cstdio>
#include <iostream>
#include <string_view>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_timer.h"
#include "SDL3/SDL_video.h"
#include "spdlog/spdlog.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "input_handler.hpp"
#include "keyboard_media.hpp"

static KeyboardMedia kb;

HHOOK InputHandler::kb_hook = NULL;
HHOOK InputHandler::mouse_hook = NULL;
std::function<void(LPWSTR)> InputHandler::keyboard_callback = nullptr;
std::function<void(int)> InputHandler::mouse_callback = nullptr;

static Uint64 now = 0, last = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("KB Test", "1.0", "Unic Six");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!InputHandler::Initialize(
          [](const std::wstring_view keyname) { kb.KeyboardCallback(keyname); },
          [](const int mouse_action) { kb.MouseCallback(mouse_action); })) {
    SDL_Log("Failed to initialize hook");
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT ||
      (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
       event->button.button == 2)) {
    SDL_Log("Quiting Program");
    return SDL_APP_SUCCESS;
  }
  kb.OnEvent(event);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  now = SDL_GetTicks();
  SDL_Delay(10);
  kb.Update(static_cast<double>(now - last) / 1000.f);
  last = now;

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
