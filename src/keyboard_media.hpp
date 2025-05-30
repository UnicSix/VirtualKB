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

namespace MediaStates {
constexpr std::bitset<media_state_max_size> IDLE        = {0b0000'0000};
constexpr std::bitset<media_state_max_size> TYPING      = {0b0000'0001};
constexpr std::bitset<media_state_max_size> RMB_HOLDING = {0b0000'0010};
constexpr std::bitset<media_state_max_size> LMB_HOLDING = {0b0000'0100};
constexpr std::bitset<media_state_max_size> DRAGGING     = {0b0000'1000};

constexpr std::bitset<media_state_max_size> WHEEL_CLICK = {0b0001'0000};
constexpr std::bitset<media_state_max_size> INTRO       = {0b0010'0000};
constexpr std::bitset<media_state_max_size> OUTRO       = {0b0100'0000};
}  // namespace MediaStates

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
  std::bitset<media_state_max_size>              m_MediaState = {};
  std::filesystem::path                          m_TexResDir;
  Uint8                                          m_CurrentAnimID = 2;
  double                                         m_Duration      = 0.f;
  MouseWindowOffset                              m_Offset        = {};

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
      spdlog::error("Undefined Animation Name: {}", name);
      std::exit(EXIT_FAILURE);
    }
  }
};
