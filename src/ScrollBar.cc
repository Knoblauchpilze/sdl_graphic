
# include "ScrollBar.hh"

namespace sdl {
  namespace graphic {

    ScrollBar::ScrollBar(const std::string& name,
                         const Orientation& orientation,
                         const core::engine::Color& color,
                         core::SdlWidget* parent,
                         const utils::Sizef& area):
      core::SdlWidget(name, area, parent, color),

      m_propsLocker(),

      m_orientation(orientation),

      m_minimum(0),
      m_maximum(2),
      m_pageStep(1),

      m_value(0),

      m_elementsChanged(true),
      m_upArrow({utils::Uuid(), utils::Boxf(), getArrowColorRole(false), true}),
      m_slider({utils::Uuid(), utils::Boxf(), getSliderColorRole(false), true}),
      m_downArrow({utils::Uuid(), utils::Boxf(), getArrowColorRole(false), true}),

      onValueChanged()
    {
      // Build this component.
      build();
    }

    void
    ScrollBar::drawContentPrivate(const utils::Uuid& uuid,
                                  const utils::Boxf& area)
    {
      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      // Load the elements: this should happen only if the geometry of the scroll bar
      // has changed since last draw operation. This can either mean that the way the
      // elements has changed or that one of the rendering properties to use to draw
      // the elements has been updated.
      if (elementsChanged()) {
        loadElements();

        // The elements have been updated.
        m_elementsChanged = false;
      }

      // Fill up the textures with the relevant color.
      fillElements();

      // The arrows and slider are arranged in a linear fashion where the top arrow is
      // positionned first, then comes the slider and finally the bottom arrow. In the
      // case of an horizontal layout this is still the case but the position are set
      // to be increasing along the horizontal axis.
      utils::Sizef sizeBar = LayoutItem::getRenderingArea().toSize();
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Boxf env = utils::Boxf::fromSize(sizeEnv, true);

      // Query the size of the elements by using the textures directly. We assume that
      // both arrows always have the same size and only query the `up` arrow.
      utils::Sizef arrow = getEngine().queryTexture(m_upArrow.id);
      utils::Sizef slider = getEngine().queryTexture(m_slider.id);

      switch (m_orientation) {
        case Orientation::Horizontal:
          m_upArrow.box = utils::Boxf(-sizeBar.w() / 2.0f + arrow.w() / 2.0f, 0.0f, arrow);
          m_slider.box = utils::Boxf(m_upArrow.box.x() + arrow.w() / 2.0f + slider.w() / 2.0f, 0.0f, slider);
          m_downArrow.box = utils::Boxf(sizeBar.w() / 2.0f - arrow.w() / 2.0f, 0.0f, arrow);
          break;
        case Orientation::Vertical:
          m_upArrow.box = utils::Boxf(0.0f, sizeBar.h() / 2.0f - arrow.h() / 2.0f, arrow);
          m_slider.box = utils::Boxf(0.0f, m_upArrow.box.y() - arrow.h() / 2.0f - slider.h() / 2.0f, slider);
          m_downArrow.box = utils::Boxf(0.0f, -sizeBar.h() / 2.0f + arrow.h() / 2.0f, arrow);
          break;
        default:
          error(
            std::string("Could not perform rendering of scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      // Draw each element but only render the part which are actually requested
      // given the input area.
      utils::Boxf dstRectForUpArrow = m_upArrow.box.intersect(area);
      if (dstRectForUpArrow.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForUpArrow, env);

        utils::Boxf srcRect = convertToLocal(dstRectForUpArrow, m_upArrow.box);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(arrow, true));

        // Draw the texture.
        getEngine().drawTexture(m_upArrow.id, &srcRectEngine, &uuid, &dstRectEngine);
      }

      utils::Boxf dstRectForSlider = m_slider.box.intersect(area);
      if (dstRectForSlider.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForSlider, env);

        utils::Boxf srcRect = convertToLocal(dstRectForSlider, m_slider.box);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(slider, true));

        // Draw the texture.
        getEngine().drawTexture(m_slider.id, &srcRectEngine, &uuid, &dstRectEngine);
      }

      utils::Boxf dstRectForDownArrow = m_downArrow.box.intersect(area);
      if (dstRectForDownArrow.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForDownArrow, env);

        utils::Boxf srcRect = convertToLocal(dstRectForDownArrow, m_downArrow.box);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(arrow, true));

        // Draw the texture.
        getEngine().drawTexture(m_downArrow.id, &srcRectEngine, &uuid, &dstRectEngine);
      }
    }

    void
    ScrollBar::stateUpdatedFromFocus(const core::FocusState& state,
                                     bool /*gainedFocus*/)
    {
      // Basically what we want here is react on complete loss of focus to deactivate the
      // highlight on any element that was highlighted until then.
      // Any other scenario is not interesting to us and we should do nothing. Note also
      // that the call to the base class behavior is purposefully deactivated in order not
      // to have the area where nothing is displayed (i.e the area of the scroll bar where
      // the slider is not) to be updated when the focus is received.
      if (!state.hasFocus()) {
        core::engine::Palette::ColorRole arrowRole = getArrowColorRole(false);
        core::engine::Palette::ColorRole sliderRole = getSliderColorRole(false);

        bool update = false;

        if (m_upArrow.role != arrowRole) {
          m_upArrow.role = arrowRole;
          m_upArrow.roleUpdated = true;
          update = true;
        }

        if (m_slider.role != sliderRole) {
          m_slider.role = sliderRole;
          m_slider.roleUpdated = true;
          update = true;
        }

        if (m_downArrow.role != arrowRole) {
          m_downArrow.role = arrowRole;
          m_downArrow.roleUpdated = true;
          update = true;
        }

        if (update) {
          requestRepaint();
        }
      }
    }

    bool
    ScrollBar::keyReleaseEvent(const core::engine::KeyEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::keyReleaseEvent(e);
    }

    bool
    ScrollBar::mouseButtonPressEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseButtonPressEvent(e);
    }

    bool
    ScrollBar::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    bool
    ScrollBar::mouseMoveEvent(const core::engine::MouseEvent& e) {
      // Given the events system we know that these events are only produced when
      // the mouse is actually inside the widget. This is very convenient because
      // it means that we can use it to detect and update the elements composing
      // the scroll bar in order to react to user input.
      // Basically we want to change the color of the slider and motion arrows if
      // the mouse hovers over them: this will help the user understand what they
      // can do with the scroll bar.

      // Convert the mouse position to local coordinate.
      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      // Check whether the mouse is hovering over any of the elements representing
      // the scroll bar: if this is the case we should request a repaint with the
      // new info.
      bool update = false;

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      core::engine::Palette::ColorRole r = getArrowColorRole(m_upArrow.box.contains(local));
      if (m_upArrow.role != r) {
        m_upArrow.role = r;
        m_upArrow.roleUpdated = true;
        update = true;
      }

      r = getSliderColorRole(m_slider.box.contains(local));
      if (m_slider.role != r) {
        m_slider.role = r;
        m_slider.roleUpdated = true;
        update = true;
      }

      r = getArrowColorRole(m_downArrow.box.contains(local));
      if (m_downArrow.role != r) {
        m_downArrow.role = r;
        m_downArrow.roleUpdated = true;
        update = true;
      }

      // Request an update if any of the element have been modified.
      if (update) {
        requestRepaint();
      }

      // Perform the base class behavior.
      return core::SdlWidget::mouseMoveEvent(e);
    }

    bool
    ScrollBar::mouseDragEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseDragEvent(e);
    }

    bool
    ScrollBar::mouseWheelEvent(const core::engine::MouseEvent& e) {
      // TODO: Implementation.
      return core::SdlWidget::mouseWheelEvent(e);
    }

    void
    ScrollBar::build() {
      // Assign a max size to the scroll bar: depending on the orientation we don't want to
      // grow beyond a certain width or height, typically 100 pixels.
      // We also need to assign a minimum size: indeed we consider that the two arrows to use
      // to move the scroll bar need a minimum size of 20 pixels each.
      utils::Sizef minSize;
      utils::Sizef maxSize = utils::Sizef::max();

      switch (m_orientation) {
        case Orientation::Horizontal:
          minSize.w() = 2.0f * minArrowSize();
          maxSize.h() = maxDimsAlongSlider();
          break;
        case Orientation::Vertical:
          minSize.h() = 2.0f * minArrowSize();
          maxSize.w() = maxDimsAlongSlider();
          break;
        default:
          error(
            std::string("Cannot determine maximum size when building scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      setMinSize(minSize);
      setMaxSize(maxSize);
    }

    void
    ScrollBar::loadElements() {
      // Clear any existing elements.
      clearElements();

      utils::Sizef total = LayoutItem::getRenderingArea().toSize();

      // Retrieve the dimensions of this scroll bar so that we can determine
      // the size of each element.
      m_upArrow.id = getEngine().createTexture(getArrowSize(total), m_upArrow.role);
      if (!m_upArrow.id.valid()) {
        error(
          std::string("Could not create up arrow to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }

      m_slider.id = getEngine().createTexture(getSliderSize(total), m_slider.role);
      if (!m_slider.id.valid()) {
        error(
          std::string("Could not create slider to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }

      m_downArrow.id = getEngine().createTexture(getArrowSize(total), m_downArrow.role);
      if (!m_downArrow.id.valid()) {
        error(
          std::string("Could not create down arrow to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }
    }

    void
    ScrollBar::fillElements() {
      // Fill the textures with the relevant color if needed.
      if (m_upArrow.roleUpdated) {
        getEngine().setTextureRole(m_upArrow.id, m_upArrow.role);
        getEngine().fillTexture(m_upArrow.id, getPalette());
        m_upArrow.roleUpdated = false;
      }

      if (m_slider.roleUpdated) {
        getEngine().setTextureRole(m_slider.id, m_slider.role);
        getEngine().fillTexture(m_slider.id, getPalette());
        m_slider.roleUpdated = false;
      }

      if (m_downArrow.roleUpdated) {
        getEngine().setTextureRole(m_downArrow.id, m_downArrow.role);
        getEngine().fillTexture(m_downArrow.id, getPalette());
        m_downArrow.roleUpdated = false;
      }
    }

  }
}
