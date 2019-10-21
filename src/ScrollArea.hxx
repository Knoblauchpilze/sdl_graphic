#ifndef    SCROLL_AREA_HXX
# define   SCROLL_AREA_HXX

# include "ScrollArea.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    ScrollArea::setHorizontalScrollBarPolicy(const BarPolicy& policy) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Assign the new display policy if needed.
      if (policy != m_hBarPolicy) {
        m_hBarPolicy = policy;

        // Request a repaint if the horizontal scroll bar is visible.
        if (isHSBarVisible()) {
          requestRepaint();
        }
      }
    }

    inline
    void
    ScrollArea::setVerticalScrollBarPolicy(const BarPolicy& policy) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Assign the new display policy if needed.
      if (policy != m_vBarPolicy) {
        m_vBarPolicy = policy;

        // Request a repaint if the horizontal scroll bar is visible.
        if (isVSBarVisible()) {
          requestRepaint();
        }
      }
    }

    inline
    std::string
    ScrollArea::getViewportName() const noexcept {
      return "viewport";
    }

    inline
    std::string
    ScrollArea::getHBarName() const noexcept {
      return "hbar";
    }

    inline
    std::string
    ScrollArea::getVBarName() const noexcept {
      return "vbar";
    }

    inline
    std::string
    ScrollArea::getCornerWidgetName() const noexcept {
      return "corner_widget";
    }

    inline
    bool
    ScrollArea::isHSBarVisible() const noexcept {
      // The horizontal scroll bar is visible if:
      // 1. the policy allows it.
      // 2. the size of the viewport requires it.
      switch (m_hBarPolicy) {
        case BarPolicy::AlwaysOff:
          return false;
        case BarPolicy::AlwaysOn:
          return true;
        case BarPolicy::AsNeeded:
        default:
          return getMaximumViewportSize().w() > LayoutItem::getRenderingArea().w();
      }
    }

    inline
    bool
    ScrollArea::isVSBarVisible() const noexcept {
      // The vertical scroll bar is visible if:
      // 1. the policy allows it.
      // 2. the size of the viewport requires it.
      switch (m_vBarPolicy) {
        case BarPolicy::AlwaysOff:
          return false;
        case BarPolicy::AlwaysOn:
          return true;
        case BarPolicy::AsNeeded:
        default:
          return getMaximumViewportSize().h() > LayoutItem::getRenderingArea().h();
      }
    }

    GridLayout&
    ScrollArea::getLayout() {
      // Try to retrieve the layout as a `GridLayout`.
      GridLayout* layout = getLayoutAs<GridLayout>();

      // If the conversion failed, this is a problem.
      if (layout == nullptr) {
        error(
          std::string("Cannot retrieve layout for scroll area"),
          std::string("Invalid layout type")
        );
      }

      // Return this layout.
      return *layout;
    }

  }
}

#endif    /* SCROLL_AREA_HXX */
