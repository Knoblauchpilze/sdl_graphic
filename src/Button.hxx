#ifndef    BUTTON_HXX
# define   BUTTON_HXX

# include "Button.hh"

namespace sdl {
  namespace graphic {

    inline
    Button::~Button() {}

    inline
    const core::SdlWidget*
    Button::getItemAt(const utils::Vector2f& pos) const noexcept {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      const utils::Vector2f local = mapFromGlobal(pos);

      // Check whether the position is inside this widget: if this is not
      // the case we consider that no widget should be returned as we don't
      // want to account for children widget spanning outside of the area
      // of this button.
      utils::Boxf b = LayoutItem::getRenderingArea().toOrigin();

      if (!b.contains(local)) {
        // The area is not contained in this widget, consider that we have
        // no valid item at this point.
        return nullptr;
      }

      // Instead of the returned element we will assume that the best fit
      // is `this` widget: this will redirect part of the events that would
      // have landed to the children directly to this widget. We will handle
      // them first and propagate them if needed to the children.
      return this;
    }

    inline
    bool
    Button::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {
        Guard guard(m_propsLocker);

        setBordersChanged();
      }

      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    inline
    bool
    Button::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {
        Guard guard(m_propsLocker);

        setBordersChanged();
      }

      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    inline
    float
    Button::getBorderDims() noexcept {
      return 10.0f;
    }

    inline
    utils::Sizef
    Button::getIconMaxDims() noexcept {
      return utils::Sizef(100.0f, 100.0f);
    }

    inline
    core::engine::mouse::Button
    Button::getClickButton() noexcept {
      return core::engine::mouse::Button::Left;
    }

    inline
    bool
    Button::bordersChanged() const noexcept {
      return m_bordersChanged;
    }

    inline
    void
    Button::setBordersChanged() {
      m_bordersChanged = true;

      requestRepaint();
    }

    inline
    void
    Button::loadBorders() {
      // Clear existing borders.
      clearBorders();

      // TODO: Create borders.
    }

    inline
    void
    Button::clearBorders() {
      // TODO: Clear borders.
    }

  }
}

#endif    /* BUTTON_HXX */
