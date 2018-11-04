#ifndef    GRIDLAYOUT_HXX
# define   GRIDLAYOUT_HXX

# include "GridLayout.hh"

namespace sdl {
  namespace graphic {

    inline
    const unsigned&
    GridLayout::getWidth() const noexcept {
      return m_width;
    }

    inline
    const unsigned&
    GridLayout::getHeight() const noexcept {
      return m_height;
    }

    inline
    const float&
    GridLayout::getMargin() const noexcept {
      return m_margin;
    }

    inline
    void
    GridLayout::setGrid(const unsigned& w, const unsigned& h) {
      m_width = w;
      m_height = h;
    }

  }
}

#endif    /* GRIDLAYOUT_HXX */
