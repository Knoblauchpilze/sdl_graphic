#ifndef    CHECKBOX_HXX
# define   CHECKBOX_HXX

# include "Checkbox.hh"

namespace sdl {
  namespace graphic {

    inline
    Checkbox::~Checkbox() {
      Guard guard(m_propsLocker);
      clearBox();
    }

    inline
    const core::SdlWidget*
    Checkbox::getItemAt(const utils::Vector2f& pos) const noexcept {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // We want to prevent children from receiving the requests that
      // are usually associated to the `getItemAt` call. To do so we
      // will check whether the input position is inside the bounds of
      // this item in which case we return this element without asking
      // the children.
      // Otherwise we consider that no element of this item's hierarchy
      // can span the position.
      const utils::Vector2f local = mapFromGlobal(pos);

      utils::Boxf b = LayoutItem::getRenderingArea().toOrigin();

      if (!b.contains(local)) {
        // Not contained in this item: no valid item (as we don't want
        // to consider the children).
        return nullptr;
      }

      // The position fits in this item, consider it valid.
      return this;
    }

    inline
    bool
    Checkbox::toggled() {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      return m_toggled;
    }

    inline
    void
    Checkbox::toggle(bool toggled) {
      // Un/toggle the checkbox as needed.
      bool change = (m_toggled != toggled);

      if (change) {
        Guard guard(m_propsLocker);

        m_toggled = toggled;

        // Request a repaint to see the new look
        // of the checkbox.
        setBoxChanged();
      }
    }

    inline
    void
    Checkbox::updatePrivate(const utils::Boxf& window) {
      // Use the base handler.
      core::SdlWidget::updatePrivate(window);

      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      setBoxChanged();
    }

    inline
    bool
    Checkbox::dropEvent(const core::engine::DropEvent& e) {
      // A drop event is generated when the mouse has been dragged.
      // Just like in the case of a button it might happen that the
      // mouse is pressed outside this checkbox and released in it.
      // In this case we don't want to change the status of this
      // checkbox.
      // In the case the starting position was inside this widget
      // we will update the state.
      utils::Vector2f localS = mapFromGlobal(e.getStartPosition());
      utils::Vector2f localE = mapFromGlobal(e.getEndPosition());
      utils::Boxf b = LayoutItem::getRenderingArea().toOrigin();

      if (e.getButton() == getClickButton() &&
          b.contains(localS) &&
          b.contains(localE))
      {
        // The drop event started and ended in this widget we can
        // safely toggle this checkbox.
        Guard guard(m_propsLocker);

        m_toggled = !m_toggled;
        setBoxChanged();
      }

      return core::SdlWidget::dropEvent(e);
    }

    inline
    bool
    Checkbox::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Check whether the button is the one to use to interact with this checkbox.
      if (e.getButton() == getClickButton()) {
        // We are certain that the mouse has been released inside the widget but we
        // need to make certain that it started also inside this widget. The only
        // case where this can happen is when the mouse has been dragged, in which
        // case we will receive also a `dropEvent`. We can better handle things over
        // there so we will just discard this case here.
        // What's left to process is the case where the use just toggled the checkbox.
        if (!e.wasDragged()) {
          Guard guard(m_propsLocker);

          m_toggled = !m_toggled;
          setBoxChanged();
        }
      }

      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    inline
    float
    Checkbox::getBorderDims() noexcept {
      return 10.0f;
    }

    inline
    utils::Sizef
    Checkbox::getSelectionBoxMaxDims() noexcept {
      return utils::Sizef(100.0f, 100.0f);
    }

    inline
    core::engine::Color
    Checkbox::getContrastedColorFromRef(const core::engine::Color& c) noexcept {
      // We will try to assign a white color by default unless the input
      // color has a brightness less than `0.5` in which case we will use
      // a black color.
      if (c.brightness() < 0.5f) {
        return core::engine::Color::NamedColor::White;
      }

      return core::engine::Color::NamedColor::Black;
    }

    inline
    utils::Sizef
    Checkbox::getTogglingElementSize(const utils::Sizef& rectangle) noexcept {
      // Decrease the size of the toggling element by `5` if possible.
      return utils::Sizef(
        std::max(rectangle.w() - 2.0f * 5.0f, 1.0f),
        std::max(rectangle.h() - 2.0f * 5.0f, 1.0f)
      );
    }

    inline
    core::engine::mouse::Button
    Checkbox::getClickButton() noexcept {
      return core::engine::mouse::Button::Left;
    }

    inline
    bool
    Checkbox::boxChanged() const noexcept {
      return m_boxChanged;
    }

    inline
    void
    Checkbox::setBoxChanged() {
      m_boxChanged = true;

      requestRepaint();
    }

    inline
    void
    Checkbox::loadBox() {
      // Clear existing selection box' data.
      clearBox();

      // We need to create a box with the dimensions specified
      // by the virtual layout item. Whether we need to add the
      // selection mark depends on the state of this box.
      // We will create two brushes to represent the two elems
      // of the visual representation of the box.
      utils::Sizef boxSz = m_boxItem->getRenderingArea().toSize();

      core::engine::Color c = getPalette().getBackgroundColor();
      core::engine::Color sbc = getContrastedColorFromRef(c);
      core::engine::Color sic = getContrastedColorFromRef(sbc);

      // Create the background.
      core::engine::BrushShPtr bBackground = std::make_shared<core::engine::Brush>(
        std::string("bg_brush_for_") + getName(),
        false
      );
      bBackground->setClearColor(sbc);
      bBackground->create(boxSz, true);

      m_emptyBox = getEngine().createTextureFromBrush(bBackground);

      // Create the toggling element.
      core::engine::BrushShPtr bForeground = std::make_shared<core::engine::Brush>(
        std::string("fg_brush_for_") + getName(),
        false
      );
      bForeground->setClearColor(sic);
      bForeground->create(getTogglingElementSize(boxSz), true);

      m_selectionItem = getEngine().createTextureFromBrush(bForeground);

      if (!m_emptyBox.valid()) {
        error(
          std::string("Could not load checkbox visual"),
          std::string("Invalid empty box texture")
        );
      }
      if (!m_selectionItem.valid()) {
        error(
          std::string("Could not load checkbox visual"),
          std::string("Invalid toggled box texture")
        );
      }
    }

    inline
    void
    Checkbox::clearBox() {
      if (m_emptyBox.valid()) {
        getEngine().destroyTexture(m_emptyBox);
        m_emptyBox.invalidate();
      }
      if (m_selectionItem.valid()) {
        getEngine().destroyTexture(m_selectionItem);
        m_selectionItem.invalidate();
      }
    }

  }
}

#endif    /* CHECKBOX_HXX */
