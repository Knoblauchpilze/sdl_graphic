#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

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
    LinearLayout::getComponentMargin() const noexcept {
      return m_componentMargin;
    }

    inline
    utils::Sizef
    LinearLayout::computeAvailableSize(const utils::Boxf& totalArea) const noexcept {
      // We assume the layout is not empty.

      // Handle horizontal layout.
      if (getDirection() == Direction::Horizontal) {
        return totalArea.toSize() - getMargin() - utils::Sizef((getItemsCount() - 1.0f) * m_componentMargin, 0.0f);
      }

      // Assume the layout is vertical.
      return totalArea.toSize() - getMargin() - utils::Sizef(0.0f, (getItemsCount() - 1.0f) * m_componentMargin);
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
      error(std::string("Unknown direction when updating layout (direction: ") + std::to_string(static_cast<int>(getDirection())) + ")");

      // Make compiler quiet about reaching end of non void function.
      return utils::Sizef();
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
