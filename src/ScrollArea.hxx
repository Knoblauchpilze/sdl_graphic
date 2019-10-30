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
        if (isHSBarVisible(LayoutItem::getRenderingArea().w())) {
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
        if (isVSBarVisible(LayoutItem::getRenderingArea().h())) {
          requestRepaint();
        }
      }
    }

    inline
    utils::Sizef
    ScrollArea::getMaximumViewportSize() const noexcept {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      ScrollableWidget* wid = getChildOrNull<ScrollableWidget>(getViewportName());

      // TODO: We should reimplement resize to be able to configure whether the
      // scroll bars are visible based on the size of the context.
      // TODO: Deadlock.

      // Use the viewport to get the maximum size of the attached viewport.
      return wid->getPreferredSize();
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
    ScrollArea::isHSBarVisible(float width) const noexcept {
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
          return getMaximumViewportSize().w() > width;
      }
    }

    inline
    bool
    ScrollArea::isVSBarVisible(float height) const noexcept {
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
          return getMaximumViewportSize().h() > height;
      }
    }

    inline
    GridLayout&
    ScrollArea::getLayout() const {
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

    inline
    ScrollableWidget*
    ScrollArea::getViewportHandler() const {
      // Try to retrieve the layout as a `GridLayout`.
      ScrollableWidget* viewport = getChildAs<ScrollableWidget>(getViewportName());

      // If the conversion failed, this is a problem.
      if (viewport == nullptr) {
        error(
          std::string("Cannot retrieve viewport handler for scroll area"),
          std::string("Invalid viewport data")
        );
      }

      // Return this widget.
      return viewport;
    }

  }
}

#endif    /* SCROLL_AREA_HXX */
