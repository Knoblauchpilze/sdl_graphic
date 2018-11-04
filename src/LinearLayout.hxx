#ifndef    LINEARLAYOUT_HXX
# define   LINEARLAYOUT_HXX

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
    void
    LinearLayout::handleHorizontalLayout(const sdl::core::Boxf& totalArea,
                                         float& cw,
                                         float& ch) const
    {
      cw = (totalArea.w() - 2.0f * m_margin - ((m_items.size() - 1) * m_componentMargin)) / (1.0f * m_items.size());
      ch = totalArea.h() - 2.0f * m_margin;
    }

    inline
    void
    LinearLayout::handleVerticalLayout(const sdl::core::Boxf& totalArea,
                                       float& cw,
                                       float& ch) const
    {
      cw = totalArea.w() - 2.0f * m_margin;
      ch = (totalArea.h() - 2.0f * m_margin - ((m_items.size() - 1) * m_componentMargin)) / (1.0f * m_items.size());
    }

  }
}

#endif    /* LINEARLAYOUT_HXX */
