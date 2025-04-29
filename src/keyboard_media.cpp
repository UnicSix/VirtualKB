#include "keyboard_media.hpp"

#include <SDL3_image/SDL_image.h>
#include <fmt/base.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "animation.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

KeyboardMedia::KeyboardMedia() {
  namespace fs = std::filesystem;
  using json = nlohmann::json;
  m_Window = SDL_CreateWindow("Pepe", 132, 132,
                              SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP |
                                  SDL_WINDOW_TRANSPARENT);
  if (!m_Window) {
    SDL_Log("Couldn't create SDL window: %s", SDL_GetError());
  }
  int width, height;
  SDL_DisplayID id = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode* display_mode = SDL_GetCurrentDisplayMode(id);
  width = display_mode->w;
  height = display_mode->h;
  SDL_GetDisplays(0);
  SDL_SetWindowPosition(m_Window, width * 0.9, height * 0.85);
  SDL_Log("Window max size: %d, %d", width, height);
  m_Renderer = SDL_CreateRenderer(m_Window, NULL);
  if (!m_Renderer) {
    SDL_Log("Couldn't create SDL renderer: %s", SDL_GetError());
  }

  m_TexResDir = "../res/textures/";
  m_Textures.reserve(4);
  m_Animations.reserve(4);
  for (const auto& entry : fs::directory_iterator(m_TexResDir)) {
    const std::string tex_path = entry.path().string();
    if (entry.is_regular_file() && entry.path().extension() == ".png") {
      SDL_Log("Loading image %s to texture ", tex_path.c_str());
      m_Textures.emplace_back(IMG_LoadTexture(m_Renderer, tex_path.c_str()));

      std::string config_path = tex_path;
      config_path.replace(config_path.rfind("png"), 4, "json");
      SDL_Log("Read Config %s", config_path.c_str());
      if (!fs::exists(config_path)) {
        throw std::runtime_error("Config file not exist");
      }
      std::ifstream layout_config(config_path.c_str());
      if (!layout_config) {
        throw std::runtime_error("Failed to read config file");
      }
      try {
        json jf = json::parse(layout_config);
        m_Animations.emplace_back(Animation(&jf));
      } catch (const json::parse_error& err) {
        SDL_Log("JSON parse error %s", err.what());
      }
      layout_config.close();
    }
  }

  SDL_Log("End keyboard instance createion");
}

void KeyboardMedia::Update(const double delta) {
  float mouse_x = 0, mouse_y = 0;
  int window_x = 0, window_y = 0;
  if (m_Offset.x == 0 && m_Offset.y == 0) {
    SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
    SDL_GetWindowPosition(m_Window, &window_x, &window_y);
    m_Offset.x = mouse_x - window_x;
    m_Offset.y = mouse_y - window_y;
  }
  if (m_IsMouseHeld) {
    SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
    SDL_SetWindowPosition(m_Window, mouse_x - m_Offset.x, mouse_y - m_Offset.y);
  } else {
    m_Offset = {0, 0};
  }
  if (m_IsKbPressed) {
    m_Duration += delta;
    if (m_Duration >= state_duration) {
      m_Duration = 0;
      m_CurrentAnimID = 3;
      m_IsKbPressed = false;
    }
  }
  SDL_RenderClear(m_Renderer);
  if (!SDL_RenderTextureRotated(m_Renderer, m_Textures.at(m_CurrentAnimID),
                                &m_Animations.at(m_CurrentAnimID).m_Srect,
                                &m_Animations.at(m_CurrentAnimID).m_Drect, 0.0f,
                                NULL, SDL_FLIP_HORIZONTAL)) {
    SDL_Log("Failed to render texture: %s", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_RenderPresent(m_Renderer);
  m_Animations.at(m_CurrentAnimID).NextFrame(delta);
}

void KeyboardMedia::OnEvent(const SDL_Event* event) {
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    m_IsMouseHeld = true;
  } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    m_IsMouseHeld = false;
  }
}

void KeyboardMedia::KeyboardCallback(const std::wstring_view keyname) {
  SDL_Log("Successfully capture the kb callback: \"%ls\"", keyname.data());
  m_IsKbPressed = true;
  m_CurrentAnimID = 1;
  m_Duration = 0.f;
  return;
}

KeyboardMedia::~KeyboardMedia() {
  for (SDL_Texture* tex : m_Textures) {
    if (tex) {
      SDL_DestroyTexture(tex);
    }
  }
  m_Textures.clear();
  SDL_DestroyRenderer(m_Renderer);
  SDL_DestroyWindow(m_Window);
  return;
}
