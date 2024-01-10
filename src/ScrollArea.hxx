#ifndef    SCROLL_AREA_HXX
# define   SCROLL_AREA_HXX

# include "ScrollArea.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    ScrollArea::setHorizontalScrollBarPolicy(const BarPolicy& policy) {
      // Protect from concurrent accesses.
      const std::lock_guard guard(m_propsLocker);

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
      const std::lock_guard guard(m_propsLocker);

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
    utils::Sizef
    ScrollArea::getMaximumViewportSize() const noexcept {
      // Assume the locker is already acquired.

      ScrollableWidget* wid = getViewportHandler();

      // Use the viewport to get the maximum size of the attached viewport.
      return wid->getPreferredSize();
    }

    inline
    bool
    ScrollArea::isHSBarVisible(float width,
                               float* remaining) const noexcept
    {
      // The horizontal scroll bar is visible if:
      // 1. the policy allows it.
      // 2. the size of the viewport requires it.
      bool visible = false;

      switch (m_hBarPolicy) {
        case BarPolicy::AlwaysOff:
          visible = false;
          break;
        case BarPolicy::AlwaysOn:
          visible = true;
          break;
        case BarPolicy::AsNeeded:
        default:
          visible = (getMaximumViewportSize().w() > width);
          break;
      }

      if (remaining != nullptr) {
        *remaining = width - getMaximumViewportSize().w();
      }

      return visible;
    }

    inline
    bool
    ScrollArea::isVSBarVisible(float height,
                               float* remaining) const noexcept
    {
      // The vertical scroll bar is visible if:
      // 1. the policy allows it.
      // 2. the size of the viewport requires it.
      bool visible = false;

      switch (m_vBarPolicy) {
        case BarPolicy::AlwaysOff:
          visible = false;
          break;
        case BarPolicy::AlwaysOn:
          visible =  true;
          break;
        case BarPolicy::AsNeeded:
        default:
          visible = (getMaximumViewportSize().h() > height);
          break;
      }

      if (remaining != nullptr) {
        *remaining = height - getMaximumViewportSize().h();
      }

      return visible;
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
