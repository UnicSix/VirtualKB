#include <SDL3/SDL_stdinc.h>
#include <spdlog/spdlog.h>

#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "SDL3/SDL_render.h"
#include "animation.hpp"

constexpr double  state_duration       = 0.75;
constexpr uint8_t sheet_file_count     = 5;
constexpr int     media_state_max_size = 8;

enum class MediaStates {
  IDLE,
  TYPING,
  RMB_HOLDING,
  LMB_HOLDING,
  DRAGGING,
  WHEEL_CLICK,
  INTRO,
  OUTRO,
};

class KeyboardMedia {
 public:
  KeyboardMedia();
  ~KeyboardMedia();
  void        Update(const double delta);
  void        OnEvent(const SDL_Event* event);
  void        KeyboardCallback(const std::wstring_view keyname);
  void        MouseCallback(const int mouse_action);
  SheetLayout LoadAnimationConfig(std::string_view& configPath);

 private:
  struct MouseWindowOffset {
    float x;
    float y;
  };

 private:
  SDL_Window*                                    m_Window   = NULL;
  SDL_Renderer*                                  m_Renderer = NULL;
  std::array<SDL_Texture*, media_state_max_size> m_Textures;
  std::array<Animation, media_state_max_size>    m_Animations;
  MediaStates           m_MediaState = MediaStates::IDLE;
  std::filesystem::path m_TexResDir;
  Uint8                 m_CurrentAnimID = 2;
  double                m_Duration      = 0.f;
  MouseWindowOffset     m_Offset        = {};

 public:
  inline Uint8 Name2AnimationID(const std::string_view name) {
    if (name.compare("intro") == 0) {
      return 0;
    } else if (name.compare("outro") == 0) {
      return 1;
    } else if (name.compare("idle") == 0) {
      return 2;
    } else if (name.compare("dragging") == 0) {
      return 3;
    } else if (name.compare("lmb held") == 0) {
      return 4;
    } else if (name.compare("wheel clicked") == 0) {
      return 5;
    } else if (name.compare("typing") == 0) {
      return 6;
    } else {
      spdlog::error("Undefined Animation Name: {}, replace outro", name);
      return 1;
    }
  }
};
