#include "keyboard_media.hpp"

#include <SDL3_image/SDL_image.h>

#include <cstdlib>
#include <string>
#include <string_view>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "animation.hpp"

KeyboardMedia::KeyboardMedia() {
  namespace fs = std::filesystem;
  m_Window = SDL_CreateWindow("Pepe", 132, 132,
                              SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP |
                              SDL_WINDOW_TRANSPARENT );
  if (!m_Window) {
    SDL_Log("Couldn't create SDL window: %s", SDL_GetError());
  }
  SDL_SetWindowPosition(m_Window, 1700, 900);
  m_Renderer = SDL_CreateRenderer(m_Window, NULL);
  if (!m_Renderer) {
    SDL_Log("Couldn't create SDL renderer: %s", SDL_GetError());
  }
  m_TexResDir = "../res/textures/";
  SheetLayout anim1 = {.row = 1,
                       .col = 1,
                       .frame_count = 1,
                       .frame_width = 132.f,
                       .frame_height = 132.f,
                       .padding = 0,
                       .anim_count = 1};
  m_Textures.reserve(2);
  m_Animations.reserve(2);
  m_Animations.emplace_back(Animation(anim1));
  m_Animations.emplace_back(Animation(anim1));
  SDL_Log("animation width, height: %f, %f",
          m_Animations[0].m_Layout.frame_width,
          m_Animations[0].m_Layout.frame_height);
  for (const auto& entry : fs::directory_iterator(m_TexResDir)) {
    const std::string texPath = entry.path().string();
    if (entry.is_regular_file() && entry.path().extension() == ".png") {
      SDL_Log("Loading image %s to texture ", texPath.c_str());
      m_Textures.emplace_back(IMG_LoadTexture(m_Renderer, texPath.c_str()));
    }
    SDL_Log("File in dir: %s", texPath.c_str());
  }

  for (auto& tex : m_Textures) {
    if (!tex) {
      SDL_Log("Empty texture");
    }
  }
  SDL_Log("End keyboard instance createion");
}
void KeyboardMedia::Update(const double delta) {
  if (m_IsKbPressed) {
    m_Duration += delta;
    if (m_Duration >= state_duration) {
      m_Duration = 0;
      m_CurrentAnimID = 0;
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
