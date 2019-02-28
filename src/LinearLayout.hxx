#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

# include <sdl_core/SdlException.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    const LinearLayout::Direction&
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
    float
    LinearLayout::computeHintedSize(const std::vector<unsigned>& widgets,
                                    const Direction& direction) const noexcept
    {
      float hintedSize = 0.0f;

      for (unsigned index = 0u ; index < widgets.size() ; ++index) {
        switch(direction) {
          case Direction::Horizontal:
            hintedSize += m_items[widgets[index]]->getSizeHint().w();
            break;
          case Direction::Vertical:
            hintedSize += m_items[widgets[index]]->getSizeHint().h();
            break;
          default:
            throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
            break;
        }
      }

      return hintedSize;
    }

    inline
    float
    LinearLayout::computeAvailableSize(const sdl::core::Boxf& totalArea,
                                       const Direction& direction) const noexcept
    {
      if (direction == Direction::Horizontal) {
        return totalArea.w() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin;
      }
      else if (direction == Direction::Vertical) {
        return totalArea.h() - 2.0f * m_margin - (m_items.size() - 1) * m_componentMargin;
      }
      throw sdl::core::SdlException(std::string("Unknown direction when updating linear layout"));
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
