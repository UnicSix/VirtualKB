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
  using json   = nlohmann::json;
  m_Window     = SDL_CreateWindow("Pepe", 132, 132,
                                  SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP |
                                      SDL_WINDOW_TRANSPARENT | SDL_WINDOW_UTILITY);
  if (!m_Window) {
    SDL_Log("Couldn't create SDL window: %s", SDL_GetError());
  }
  SDL_DisplayID          id           = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode* display_mode = SDL_GetCurrentDisplayMode(id);
  const int              width        = display_mode->w;
  const int              height       = display_mode->h;
  SDL_GetDisplays(nullptr);
  // SDL_SetWindowPosition(m_Window, width * 0.9, height * 0.85);
  SDL_Log("Window max size: %d, %d", width, height);
  m_Renderer = SDL_CreateRenderer(m_Window, NULL);
  if (!m_Renderer) {
    SDL_Log("Couldn't create SDL renderer: %s", SDL_GetError());
  }

  // load resources
  m_Textures.fill(nullptr);
  m_TexResDir  = "../res/textures/";
  m_Offset     = {0, 0};
  auto get_val = [&](const char* key, const json* layout_data) {
    if (!layout_data->contains(key)) {
      throw std::runtime_error("Missing config key");
    }
    return layout_data->at(key);
  };
  for (const auto& entry : fs::directory_iterator(m_TexResDir)) {
    const std::string tex_path = entry.path().string();
    if (entry.is_regular_file() && entry.path().extension() == ".png") {
      std::string config_path = tex_path;
      config_path.replace(config_path.rfind("png"), 4, "json");
      SDL_Log("Read Config %s", config_path.c_str());
      if (!fs::exists(config_path)) {
        throw std::runtime_error("Config file not exist");
      }

      std::ifstream layout_config(config_path.c_str());
      json          jf;
      if (!layout_config) {
        throw std::runtime_error("Failed to read config file");
      }
      try {
        jf = json::parse(layout_config);
      } catch (const json::parse_error& err) {
        SDL_Log("JSON parse error %s", err.what());
      }
      try {
        m_Textures.at(
            Name2AnimationID(static_cast<std::string>(get_val("name", &jf)))) =
            IMG_LoadTexture(m_Renderer, tex_path.c_str());
        m_Animations.at(Name2AnimationID(
            static_cast<std::string>(get_val("name", &jf)))) = Animation(&jf);
      } catch (const json::parse_error& err) {
        SDL_Log("Json parse error %s", err.what());
      }
      layout_config.close();
    }
  }

  m_MediaState = MediaStates::IDLE;
  SDL_Log("End keyboard instance creation");
}

void KeyboardMedia::Update(const double delta) {
  float mouse_x = 0, mouse_y = 0;
  int   window_x = 0, window_y = 0;
  if (m_Offset.x == 0 && m_Offset.y == 0) {
    SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
    SDL_GetWindowPosition(m_Window, &window_x, &window_y);
    m_Offset.x = mouse_x - window_x;
    m_Offset.y = mouse_y - window_y;
  }

  // Update states
  switch (m_MediaState) {
    case MediaStates::TYPING:
      m_Duration += delta;
      if (m_Duration >= state_duration) {
        m_Duration = 0;
        m_Animations.at(m_CurrentAnimID).ResetAnimation();
        m_CurrentAnimID = Name2AnimationID("idle");
        m_MediaState    = MediaStates::IDLE;
      }
      break;
    case MediaStates::DRAGGING:
      SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
      SDL_SetWindowPosition(m_Window, mouse_x - m_Offset.x,
                            mouse_y - m_Offset.y);
      m_CurrentAnimID = Name2AnimationID("dragging");
      break;
    case MediaStates::LMB_HOLDING:
      break;
    default:
      m_CurrentAnimID = Name2AnimationID("idle");
      m_Offset        = {0, 0};
      break;
  }

  // Render call
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
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == 1) {
    m_MediaState = MediaStates::DRAGGING;
  } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    m_MediaState = MediaStates::IDLE;
  }
  if (event->type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
    m_MediaState = MediaStates::IDLE;
  }
}

void KeyboardMedia::KeyboardCallback(const std::wstring_view keyname) {
  SDL_Log("Successfully capture the kb callback: \"%ls\"", keyname.data());
  m_MediaState    = MediaStates::TYPING;
  m_CurrentAnimID = Name2AnimationID("typing");
  m_Duration      = 0.f;
}

void KeyboardMedia::MouseCallback(const int mouse_action) {
  if ((SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MOUSE_FOCUS) != 0) {
    return;
  }
  if (mouse_action == 1) {
    m_MediaState = m_MediaState == MediaStates::LMB_HOLDING
                       ? MediaStates::IDLE
                       : MediaStates::LMB_HOLDING;
  }
  if (m_MediaState == MediaStates::LMB_HOLDING) {
    m_CurrentAnimID = Name2AnimationID("lmb holding");
  } else {
    m_CurrentAnimID = Name2AnimationID("idle");
  }
}

KeyboardMedia::~KeyboardMedia() {
  for (SDL_Texture* tex : m_Textures) {
    if (tex) {
      SDL_DestroyTexture(tex);
    }
  }
  SDL_DestroyRenderer(m_Renderer);
  SDL_DestroyWindow(m_Window);
  return;
}
