#pragma once
#include <SDL3_image/SDL_image.h>

#include <string>

#include "SDL3/SDL_rect.h"

typedef struct SheetLayout {
  int row;
  int col;
  int frame_count;
  float frame_width;
  float frame_height;  // To init SDL_FRect
  int padding;         // The gap between frames
  int anim_count;
} SheetLayout;

class Animation {
 public:
  Animation();
  Animation(const SheetLayout layout);
  ~Animation();

  void NextFrame(const double delta);
  bool LoadAnimationConfig(std::string_view &configPath);
  bool GenAnimationConfig(std::string_view &configPath);
  bool Play(const std::string anim, const double delta);

  // assuming all frames' size are the same
  SDL_FRect m_Srect;
  SDL_FRect m_Drect;
  SheetLayout m_Layout;
  int m_CurFcnt;
  int m_Delay;          // ms
  int m_FrameDuration;  // indicate the duration of the current frame
  bool m_Flipped;
};
