#pragma once
#include <SDL3_image/SDL_image.h>
#include <nlohmann/json.hpp>

#include <string>
#include <unordered_set>

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
  Animation(const nlohmann::json *layout_data);
  ~Animation();

  void NextFrame(const double delta);
  void ResetAnimation();
  bool Play(const std::string anim, const double delta);

  SDL_FRect m_Srect;
  SDL_FRect m_Drect;
  SheetLayout m_Layout;
  int m_CurFcnt;
  int m_Delay;          // ms
  int m_FrameDuration;  // indicate the duration of the current frame
  bool m_Flipped;
};
