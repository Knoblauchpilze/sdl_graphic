#ifndef    SCROLLABLE_WIDGET_HXX
# define   SCROLLABLE_WIDGET_HXX

# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    ScrollableWidget::setupSupport(core::SdlWidget* /*widget*/) {
      // Empty implementation.
    }

    inline
    bool
    ScrollableWidget::hasSupportWidget() noexcept {
      return hasChild(m_supportName);
    }

    inline
    core::SdlWidget*
    ScrollableWidget::getSupportWidget() {
      // Use the base handler to retrieve the support widget if any.
      return getChildAs<core::SdlWidget>(m_supportName);
    }

  }
}

#endif    /* SCROLLABLE_WIDGET_HXX */
