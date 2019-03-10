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
    utils::maths::Sizef
    LinearLayout::computeAvailableSize(const utils::maths::Boxf& totalArea) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return utils::maths::Sizef(
          totalArea.w() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin,
          totalArea.h() - 2.0f * m_margin
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::maths::Sizef(
          totalArea.w() - 2.0f * m_margin,
          totalArea.h() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

    inline
    utils::maths::Sizef
    LinearLayout::computeDefaultWidgetBox(const utils::maths::Sizef& area,
                                          const unsigned& widgetsCount) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return utils::maths::Sizef(
          area.w() / widgetsCount,
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::maths::Sizef(
          area.w(),
          area.h() / widgetsCount
        );
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
