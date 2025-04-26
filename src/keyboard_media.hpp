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
  void KeyboardCallback(const std::wstring_view keyname);

 private:
  SDL_Window* m_Window = NULL;
  SDL_Renderer* m_Renderer = NULL;
  std::vector<SDL_Texture*> m_Textures;
  std::vector<Animation> m_Animations;
  std::filesystem::path m_TexResDir;
  Uint8 m_CurrentAnimID;
  double m_Duration = 0.f;
  bool m_IsKbPressed = false;
};
