#ifndef    SCROLLABLE_WIDGET_HXX
# define   SCROLLABLE_WIDGET_HXX

# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    const core::SdlWidget*
    ScrollableWidget::getItemAt(const utils::Vector2f& pos) const noexcept {
      // Use the base handler to retrieve the item spanning the position.
      const core::SdlWidget* wid = core::SdlWidget::getItemAt(pos);

      // Check whether the widget corresponds to the support widget.
      if (!hasSupportWidget()) {
        return wid;
      }

      core::SdlWidget* support = getSupportWidget();

      if (wid != support) {
        // The best fit is not the support widget, we can return it.
        return wid;
      }

      // The best fit is the support widget: we need to make sure that the
      // area does correspond to the area specified for this widget. We do
      // check it against the area of `this` widget.
      utils::Boxf b = LayoutItem::getRenderingArea();

      if (!b.contains(pos)) {
        // The area is not contained in this widget, consider that we have
        // no valid item at this point.
        return nullptr;
      }

      // The support widget is valid.
      return wid;
    }

    inline
    void
    ScrollableWidget::setupSupport(core::SdlWidget* /*widget*/) {
      // Empty implementation.
    }

    inline
    bool
    ScrollableWidget::hasSupportWidget() const noexcept {
      return hasChild(m_supportName);
    }

    inline
    core::SdlWidget*
    ScrollableWidget::getSupportWidget() const {
      // Use the base handler to retrieve the support widget if any.
      return getChildAs<core::SdlWidget>(m_supportName);
    }

  }
}

#endif    /* SCROLLABLE_WIDGET_HXX */
