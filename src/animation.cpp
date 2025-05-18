#include "animation.hpp"

#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>

#include <nlohmann/json_fwd.hpp>
#include <stdexcept>

#include "SDL3/SDL_log.h"

Animation::Animation(const SheetLayout layout) {
  m_Layout = layout;
  SDL_Log("member layout width: %f, %f", m_Layout.frame_width,
          layout.frame_width);
  m_Srect = {0.0f, 0.0f, m_Layout.frame_width, m_Layout.frame_height};
  m_Drect = {0.0f, 0.0f, m_Layout.frame_width, m_Layout.frame_height};
  m_CurFcnt = 0;
  m_Delay = 150;
  m_FrameDuration = 0;
  m_Flipped = true;
}

Animation::Animation(const nlohmann::json *layout_data) {
  auto get_val = [&](const char *key) {
    if (!layout_data->contains(key)) {
      throw std::runtime_error("Missing config key");
    }
    return layout_data->at(key);
  };
  m_Layout = {
      .row = get_val("row"),
      .col = get_val("col"),
      .frame_count = get_val("frame_count"),
      .frame_width = get_val("frame_width"),
      .frame_height = get_val("frame_height"),
      .padding = get_val("padding"),
      .anim_count = get_val("anim_count"),
  };
  m_Delay = get_val("delay");
  m_Srect = {0.0f, 0.0f, m_Layout.frame_width, m_Layout.frame_height};
  m_Drect = {0.0f, 0.0f, m_Layout.frame_width, m_Layout.frame_height};
  m_CurFcnt = 0;
  m_FrameDuration = 0;
  m_Flipped = true;
}

Animation::~Animation() {}

void Animation::NextFrame(const double delta) {
  if (m_Delay >= m_FrameDuration) {
    m_FrameDuration += static_cast<int>(delta * 1000);
  } else {
    m_Srect.x = (int)(m_CurFcnt % m_Layout.col) * m_Layout.frame_width +
                m_Layout.padding;
    m_Srect.y = (int)(m_CurFcnt / m_Layout.col) * m_Layout.frame_height +
                m_Layout.padding;
    m_CurFcnt = (m_CurFcnt + 1) % (m_Layout.row * m_Layout.col);
    m_FrameDuration = 0;
  }
}

void Animation::ResetAnimation() {
  m_CurFcnt = 0;
}
