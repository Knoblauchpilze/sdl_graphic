#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include "LinearLayout.hh"
# include <sdl_core/LayoutException.hh>

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
    utils::Sizef
    LinearLayout::computeAvailableSize(const utils::Boxf& totalArea) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return utils::Sizef(
          totalArea.w() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin,
          totalArea.h() - 2.0f * m_margin
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::Sizef(
          totalArea.w() - 2.0f * m_margin,
          totalArea.h() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin
        );
      }
      throw sdl::core::LayoutException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

    inline
    utils::Sizef
    LinearLayout::computeDefaultWidgetBox(const utils::Sizef& area,
                                          const unsigned& widgetsCount) const {
      // We assume the layout is not empty.
      if (getDirection() == Direction::Horizontal) {
        return utils::Sizef(
          area.w() / widgetsCount,
          area.h()
        );
      }
      else if (getDirection() == Direction::Vertical) {
        return utils::Sizef(
          area.w(),
          area.h() / widgetsCount
        );
      }
      throw sdl::core::LayoutException(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
