#ifndef    SELECTOR_WIDGET_HXX
# define   SELECTOR_WIDGET_HXX

# include "SelectorWidget.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    SelectorWidget::setActiveWidget(const std::string& name) {
      getLayout().setActiveItem(name);
    }

    inline
    void
    SelectorWidget::setActiveWidget(const int& index) {
      getLayout().setActiveItem(index);
    }

    inline
    void
    SelectorWidget::switchToNext() {
      getLayout().switchToNext();
    }

    inline
    bool
    SelectorWidget::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // We need to switch the active widget if the internal `m_switchOnLeftClick`
      // property is active. And of course if at least two widgets are inserted in
      // this obejct: 0 widget is not interesting for obvious reasons but even one
      // widget is not enough to perform a switch.
      // In addition to that we have to verify that the location of the click is
      // inside of this widget.

      // Determine whether the position of the click is inside the widget.
      if (isInsideWidget(e.getMousePosition()) && switchOnClick() && getChildrenCount() > 1) {
        // Switch to the next widget.
        switchToNext();
      }

      // Use base handler to determine whether the event was recognized.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    inline
    bool
    SelectorWidget::switchOnClick() const noexcept {
      return m_switchOnLeftClick;
    }

    inline
    SelectorLayout&
    SelectorWidget::getLayout() {
      // Try to retrieve the layout as a `SelectorLayout`.
      SelectorLayout* layout = getLayoutAs<SelectorLayout>();

      // If the conversion failed, this is a problem.
      if (layout == nullptr) {
        error(
          std::string("Cannot retrieve layout for selector widget"),
          std::string("Invalid layout type")
        );
      }

      // Return this layout.
      return *layout;
    }

  }
}

#endif    /* SELECTOR_WIDGET_HXX */
