#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include <sdl_core/SdlException.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    const sdl::core::Layout::Direction&
    LinearLayout::getDirection() const noexcept {
      return m_direction;
    }

    inline
    const float&
    LinearLayout::getMargin() const noexcept {
      return m_margin;
    }

    inline
    const float&
    LinearLayout::getComponentMargin() const noexcept {
      return m_componentMargin;
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeAvailableSize(const sdl::utils::Boxf& totalArea) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(
          totalArea.w() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin,
          totalArea.h() - 2.0f * m_margin
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(
          totalArea.w() - 2.0f * m_margin,
          totalArea.h() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

    inline
    sdl::utils::Sizef
    LinearLayout::computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                          const unsigned& widgetsCount) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return sdl::utils::Sizef(
          area.w() / widgetsCount,
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return sdl::utils::Sizef(
          area.w(),
          area.h() / widgetsCount
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
