#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "SDL3/SDL_render.h"
#include "animation.hpp"
constexpr double state_duration = 0.75;
class KeyboardMedia {
 public:
  KeyboardMedia();
  ~KeyboardMedia();
  void Update(const double delta);
  void OnEvent(const SDL_Event *event);
  void KeyboardCallback(const std::wstring_view keyname);

private:
  struct MouseWindowOffset {
    float x;
    float y;
  };

 private:
  SDL_Window* m_Window = NULL;
  SDL_Renderer* m_Renderer = NULL;
  std::vector<SDL_Texture*> m_Textures;
  std::vector<Animation> m_Animations;
  std::filesystem::path m_TexResDir;
  Uint8 m_CurrentAnimID = 0;
  double m_Duration = 0.f;
  bool m_IsKbPressed = false;
  bool m_IsMouseHeld = false;
  MouseWindowOffset m_Offset;

};
