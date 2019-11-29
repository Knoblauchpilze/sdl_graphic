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
    Button::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {
        Guard guard(m_propsLocker);

        // Update the role of the borders.
        m_borders.pressed = !m_borders.pressed;

        // Request a repaint to see the new look of the button.
        requestRepaint();
      }

      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    inline
    bool
    Button::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with the button.
      if (e.getButton() == getClickButton()) {
        Guard guard(m_propsLocker);

        // Update the role of the borders.
        m_borders.pressed = !m_borders.pressed;

        // Request a repaint to see the new look of the button.
        requestRepaint();
      }

      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    inline
    void
    Button::stateUpdatedFromFocus(const core::FocusState& /*state*/,
                                  bool /*gainedFocus*/)
    {
      // Do nothing here. This is to prevent the base class behavior of chaning
      // the color role of the background and thus making obvious that the button
      // is actually composed of several elements. We're okay with that because
      // it only means that we won't get update when the widget gains or loses
      // focus.
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
        utils::Sizef(area.w(), getBorderDims()),
        getBorderColorRole()
      );
      if (!m_borders.hLightBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Horizontal border not valid")
        );
      }

      m_borders.hDarkBorder = getEngine().createTexture(
        utils::Sizef(area.w(), getBorderDims()),
        getBorderAlternateColorRole()
      );
      if (!m_borders.hDarkBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Horizontal border not valid")
        );
      }

      m_borders.vLightBorder = getEngine().createTexture(
        utils::Sizef(getBorderDims(), area.h()),
        getBorderColorRole()
      );
      if (!m_borders.vLightBorder.valid()) {
        error(
          std::string("Unable to create border for button"),
          std::string("Vertical border not valid")
        );
      }

      m_borders.vDarkBorder = getEngine().createTexture(
        utils::Sizef(getBorderDims(), area.h()),
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

  }
}

#endif    /* BUTTON_HXX */
