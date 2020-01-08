#ifndef    BUTTON_HXX
# define   BUTTON_HXX

# include "Button.hh"

namespace sdl {
  namespace graphic {

    inline
    Button::~Button() {
      Guard guard(m_propsLocker);
      clearBorders();
    }

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
    void
    Button::toggle(bool toggled) {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Check whether the type of the button allows for this manipulation.
      if (m_type == button::Type::Toggle) {
        // Un/toggle the button as needed.
        bool change = (m_state != State::Toggled && toggled) || (m_state != State::Released && !toggled);

        if (change) {
          // Update the role of the borders.
          bool old = m_borders.pressed;
          m_borders.pressed = toggled;

          if (old != m_borders.pressed) {
            setBordersChanged();
          }

          // And the state of the buttno.
          m_state = (toggled ? State::Toggled : State::Released);

          // Request a repaint to see the new look of the button.
          requestRepaint();
        }
      }
    }

    inline
    bool
    Button::toggled() {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      return m_state == State::Toggled;
    }

    inline
    void
    Button::updatePrivate(const utils::Boxf& window) {
      // Use the base handler.
      core::SdlWidget::updatePrivate(window);

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Mark the borders as dirty.
      setBordersChanged();
    }

    inline
    bool
    Button::dropEvent(const core::engine::DropEvent& e) {
      // A drop event is generated when the mouse has been dragged.
      // We mostly handle this case in order to correctly release
      // the button when the mouse is pressed inside the button and
      // then dragged elsewhere and released. In this case the events
      // system does not propagate the click to this widget (because
      // it is not under the click's position): this is normal but
      // it prevents use from releasing the button.
      // In any other case though we will receive a `mouseButtonRelease`
      // event so we shouldn't be too greedy and try to only handle
      // the mentionned cases.
      utils::Vector2f localS = mapFromGlobal(e.getStartPosition());
      utils::Vector2f localE = mapFromGlobal(e.getEndPosition());
      utils::Boxf b = LayoutItem::getRenderingArea().toOrigin();

      if (e.getButton() == getClickButton() && !b.contains(localE)) {
        // We are facing a drop event outside of this widget while the
        // initial drag occurred inside it (otherwise we would not be
        // receiving the event): we have to determine whether it is
        // needed to release the borders.
        // This is the case if the button state is not set to `Toggled`
        // (in which case the borders will stay pressed).
        Guard guard(m_propsLocker);

        if (m_state != State::Toggled) {
          // Update the role of the borders.
          m_borders.pressed = false;

          // And reset the state of the button.
          m_state = State::Released;

          // Request a repaint to see the new look of the button.
          requestRepaint();
        }
      }

      // Also check for drop event which are inside this widget but which
      // originated outside of the button: in this case we don't want to
      // do anything. This is already handled in the `mouseButtonRelease`
      // method where we don't do anything precisely so that we can handle
      // things here.
      // In the case the button did not start in this button, we don't
      // want to change anything to the button state. On the other hand
      // if it originated inside this widget we want to make it just like
      // it was a regular mouse button release event.
      if (e.getButton() == getClickButton() &&
          b.contains(localS) &&
          b.contains(localE))
      {
        Guard guard(m_propsLocker);

        updateButtonState();
      }

      return core::SdlWidget::dropEvent(e);
    }

    inline
    bool
    Button::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {
        Guard guard(m_propsLocker);

        // Update the role of the borders if needed (i.e. if the button is not
        // already toggled).
        if (m_state != State::Toggled) {
          m_borders.pressed = !m_borders.pressed;

          // Also update the state to being pressed.
          m_state = State::Pressed;

          // Request a repaint to see the new look of the button.
          requestRepaint();
        }
      }

      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    inline
    bool
    Button::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {

        // We know that the mouse has been released inside this widget. We need
        // to determine whether the event occurred inside the widget.
        // We can't really do anything here but what is certain is that the only
        // case where the mouse can be released without having being pressed in
        // the widget is in the case of a drag and drop event.
        // In which case we will receive the drop event and thus we can act to
        // do something over there. We will ignore dragged events in here in
        // consequence.
        if (!e.wasDragged()) {
          Guard guard(m_propsLocker);
          updateButtonState();
        }
      }

      return core::SdlWidget::mouseButtonReleaseEvent(e);
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
    core::engine::Palette::ColorRole
    Button::getBorderColorRole() noexcept {
      return core::engine::Palette::ColorRole::AlternateBase;
    }

    inline
    core::engine::Palette::ColorRole
    Button::getBorderAlternateColorRole() noexcept {
      return core::engine::Palette::ColorRole::Dark;
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

      utils::Boxf area = LayoutItem::getRenderingArea();

      m_borders.hLightBorder = getEngine().createTexture(
        utils::Sizef(area.w(), m_borders.size),
        getBorderColorRole()
      );
      if (!m_borders.hLightBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Horizontal border not valid")
        );
      }

      m_borders.hDarkBorder = getEngine().createTexture(
        utils::Sizef(area.w(), m_borders.size),
        getBorderAlternateColorRole()
      );
      if (!m_borders.hDarkBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Horizontal border not valid")
        );
      }

      m_borders.vLightBorder = getEngine().createTexture(
        utils::Sizef(m_borders.size, area.h()),
        getBorderColorRole()
      );
      if (!m_borders.vLightBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Vertical border not valid")
        );
      }

      m_borders.vDarkBorder = getEngine().createTexture(
        utils::Sizef(m_borders.size, area.h()),
        getBorderAlternateColorRole()
      );
      if (!m_borders.vDarkBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Vertical border not valid")
        );
      }

      getEngine().fillTexture(m_borders.hLightBorder, getPalette());
      getEngine().fillTexture(m_borders.hDarkBorder, getPalette());
      getEngine().fillTexture(m_borders.vLightBorder, getPalette());
      getEngine().fillTexture(m_borders.vDarkBorder, getPalette());
    }

    inline
    void
    Button::clearBorders() {
      if (m_borders.hLightBorder.valid()) {
        getEngine().destroyTexture(m_borders.hLightBorder);
        m_borders.hLightBorder.invalidate();
      }
      if (m_borders.hDarkBorder.valid()) {
        getEngine().destroyTexture(m_borders.hDarkBorder);
        m_borders.hDarkBorder.invalidate();
      }

      if (m_borders.vLightBorder.valid()) {
        getEngine().destroyTexture(m_borders.vLightBorder);
        m_borders.vLightBorder.invalidate();
      }
      if (m_borders.vDarkBorder.valid()) {
        getEngine().destroyTexture(m_borders.vDarkBorder);
        m_borders.vDarkBorder.invalidate();
      }
    }

    inline
    void
    Button::updateButtonState() {
      // We need to toggle the button if needed.
      switch (m_type) {
        case button::Type::Toggle:
          // Un/toggle the button.
          if (m_state == State::Toggled) {
            m_state = State::Released;
          }
          else {
            m_state = State::Toggled;
          }
          break;
        case button::Type::Regular:
        default:
          // Assume default case is a regular button.
          m_state = State::Released;
          break;
      }

      // Update the role of the borders.
      if (m_state == State::Released) {
        m_borders.pressed = false;

        // Request a repaint to see the new look of the button.
        requestRepaint();
      }
    }

  }
}

#endif    /* BUTTON_HXX */
