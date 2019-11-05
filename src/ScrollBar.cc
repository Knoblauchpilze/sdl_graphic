
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
    ScrollBar::keyPressEvent(const core::engine::KeyEvent& e) {
      // Some keys are linked to specific scroll bar actions. This handler aims
      // at detecting such keys and requesting the correct action to update the
      // slider.
      // Note that we voluntarily use the key press event in order to be able
      // to react to repeated key events.
      bool update = false;

      switch (e.getRawKey()) {
        case core::engine::RawKey::Up:
        case core::engine::RawKey::Left:
          // Based on the orientation both keys might trigger a step sub.
          update = performAction(Action::SingleStepSub);
          break;
        case core::engine::RawKey::Down:
        case core::engine::RawKey::Right:
          // Based on the orientation both keys might trigger a step add.
          update = performAction(Action::SingleStepAdd);
          break;
        case core::engine::RawKey::PageUp:
          update = performAction(Action::PageStepSub);
          break;
        case core::engine::RawKey::PageDown:
          update = performAction(Action::PageStepAdd);
          break;
        case core::engine::RawKey::Home:
          update = performAction(Action::ToMinimum);
          break;
        case core::engine::RawKey::End:
          update = performAction(Action::ToMaximum);
          break;
        default:
          // Not a key binded to an action, do nothing.
          break;
      }

      if (update) {
        requestRepaint();
      }

      // Use the base handler to provide the return value.
      return core::SdlWidget::keyPressEvent(e);
    }

    bool
    ScrollBar::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // We want to detect click on any of the element of the scroll bar to
      // perform an update of the position of the slider. To do so we will
      // use the position of the button when it was released and compare it
      // to the internal position saved for each element.
      // Also we don't want to handle events where the mouse button release
      // event was actually issued after a drag event.
      if (e.wasDragged()) {
        return core::SdlWidget::mouseButtonReleaseEvent(e);
      }

      bool update = false;

      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      bool isInUpArrow = m_upArrow.box.contains(local);
      bool isInSlider = m_slider.box.contains(local);
      bool isInDownArrow = m_downArrow.box.contains(local);

      if (isInUpArrow) {
        update = performAction(Action::SingleStepSub);
      }

      if (isInDownArrow) {
        update = performAction(Action::SingleStepAdd);
      }

      if (!isInUpArrow && !isInSlider && !isInDownArrow) {
        // Click on an empty region of the area used to move the slider around:
        // we want to trigger a page step in this case. The precise value can
        // be found by determining whether the click was _above_ (i.e. either
        // above or on the left of the slider depending on the orientation or)
        // or _below_ (so below or on the right of the slider).
        bool onTop = true;
        switch (m_orientation) {
          case Orientation::Horizontal:
            onTop = (m_slider.box.getLeftBound() >= local.x());
            break;
          case Orientation::Vertical:
            onTop = (m_slider.box.getTopBound() <= local.y());
            break;
          default:
            log(
              std::string("Cannot perform page step motion for scroll bar, unknown orientation ") + std::to_string(static_cast<int>(m_orientation)),
              utils::Level::Warning
            );
            break;
        }

        update = performAction(onTop ? Action::PageStepSub : Action::PageStepAdd);
      }

      // Request a repaint if needed.
      if (update) {
        requestRepaint();
      }

      // Use the base handler to provide a return value.
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
      // In the case of a drag event we want to move the slider to the position indicated
      // by the mouse. This includes converting the current mouse position to the local
      // coordinate frame and then determining the equivalent value required to have the
      // slider at this location.
      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      // Convert in terms of `slider's reference frame`.
      int desired = getValueFromSliderPos(local);

      // Assign the value and request a repaint if needed.
      bool update = performAction(Action::Move, desired);

      if (update) {
        requestRepaint();
      }

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseDragEvent(e);
    }

    bool
    ScrollBar::mouseWheelEvent(const core::engine::MouseEvent& e) {
      // We want to trigger some page step actions when the wheel is rolled
      // on the scroll bar. We only want to do so if the mouse is inside
      // this widget though as otherwise it means that we currently react to
      // the wheel event on an application wide basis.
      if (!isMouseInside()) {
        return core::SdlWidget::mouseWheelEvent(e);
      }

      utils::Vector2i scroll = e.getScroll();

      // Consolidate the action needed by this scroll event: we will consider
      // that any step towards positive axis corresponds to a page step sub
      // and any scroll towards negative axis corresponds to a page step add.
      // Indeed the common semantic is to associate wheel up events with a
      // positive scroll value which in our case would correspond to a motion
      // of the scroll bar's slider to the up direction and thus a page step
      // sub.
      int pageStepSub = std::max(0, scroll.x()) + std::max(0, scroll.y());
      int pageStepAdd = std::abs(std::min(0, scroll.x())) + std::abs(std::min(0, scroll.y()));

      int total = pageStepAdd - pageStepSub;

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      bool update = false;
      Action action = (total < 0 ? Action::PageStepSub : Action::PageStepAdd);

      if (total != 0) {
        update = performAction(action);
      }

      if (update) {
        requestRepaint();
      }

      // Use base implementation to provide a return value.
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

      // We only want to handle hover over focus: the click is not something that
      // is not really helpful as it is already handled by the mouse button release
      // event.
      setFocusPolicy(core::FocusPolicy(core::focus::Type::Hover));
    }

    bool
    ScrollBar::performAction(const Action& action,
                             int value)
    {
      // Assume that the locker is already acquired.

      // The input action describes the motion to apply to the slider. Depending
      // on the range of the scroll bar and the page step we need to adapt the
      // corresponding modification.
      // A second task of this method is to update the position of the slider so
      // that it is consistent with the actual value it is meant to represent.
      // For example if the slider is at the minimum possible value it should be
      // rendered right below (or on the right) of the top arrwo. As the value
      // increases it should be drawn closer and closer to the down arrow.

      // Handle trivial case of `NoAction`.
      if (action == Action::NoAction) {
        // Consider that `doing nothing` is never successful.
        return false;
      }

      // First thing is to compute the target value pointed to by the slider.
      int targetValue = m_value;

      switch (action) {
        case Action::SingleStepAdd:
          ++targetValue;
          break;
        case Action::SingleStepSub:
          --targetValue;
          break;
        case Action::PageStepAdd:
          targetValue = m_value + m_pageStep;
          break;
        case Action::PageStepSub:
          targetValue = m_value - m_pageStep;
          break;
        case Action::ToMinimum:
          targetValue = m_minimum;
          break;
        case Action::ToMaximum:
          targetValue = m_maximum;
          break;
        case Action::Move:
          targetValue = value;
          break;
        default:
          // Note that this does not include the `NoAction` as it has been handled
          // beforehand.
          error(
            std::string("Cannot perform requested action in scroll bar"),
            std::string("Unknown action value ") + std::to_string(static_cast<int>(action))
          );
          break;
      }

      // Apply this value.
      bool changed = setValuePrivate(targetValue);

      // If the value could not be modified, nothing is left to do.
      if (!changed) {
        return false;
      }

      // The internal value has been changed: we need to update the position of the slider
      // based on the value represented by this scroll bar.
      updateSliderPosFromValue();

      // The position of the slider has been updated.
      return true;
    }

    void
    ScrollBar::updateSliderPosFromValue() {
      // The slider is supposed to represent a single page step in size.
      // We know that the step `0` is right alongside the up arrow and
      // that the last step is right alongisde the down arrow. Everything
      // in between increases linearly between that.
      float perc = (m_minimum == m_maximum ? 0.0f : 1.0f * m_value / (m_maximum - m_minimum));

      float availableSpace = 0.0f;
      switch (m_orientation) {
        case Orientation::Horizontal:
          availableSpace = m_downArrow.box.getLeftBound() - m_upArrow.box.getRightBound() - m_slider.box.w();
          break;
        case Orientation::Vertical:
          availableSpace = m_upArrow.box.getBottomBound() - m_downArrow.box.getTopBound() - m_slider.box.h();
          break;
        default:
          error(
            std::string("Could not update slider position from value"),
            std::string("Unknown scroll bar orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      // The position of the slider is given by dividing fairly the `availableSpace`
      // among all the possible steps.
      utils::Vector2f sliderPos;

      switch (m_orientation) {
        case Orientation::Horizontal:
          sliderPos = utils::Vector2f(m_upArrow.box.getRightBound() + m_slider.box.w() / 2.0f + perc * availableSpace, 0.0f);
          break;
        case Orientation::Vertical:
          sliderPos = utils::Vector2f(0.0f, m_upArrow.box.getBottomBound() - m_slider.box.h() / 2.0f - perc * availableSpace);
        default:
          break;
      }

      m_slider.box.x() = sliderPos.x();
      m_slider.box.y() = sliderPos.y();
    }

    int
    ScrollBar::getValueFromSliderPos(const utils::Vector2f& local) const {
      // We want to compute the value of the scroll bar which would produce a position of the
      // slider equal to the input `local`. To do so we have to determine the available space
      // for the slider knowing that it represents the whole range.
      float availableSpace = 0.0f;
      switch (m_orientation) {
        case Orientation::Horizontal:
          availableSpace = m_downArrow.box.getLeftBound() - m_upArrow.box.getRightBound() - m_slider.box.w();
          break;
        case Orientation::Vertical:
          availableSpace = m_upArrow.box.getBottomBound() - m_downArrow.box.getTopBound() - m_slider.box.h();
          break;
        default:
          error(
            std::string("Could not update slider position from value"),
            std::string("Unknown scroll bar orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      // The percentage of the slider is given by one of the two coordinates depending on the
      // orientation. Note that we don't need to check for invalid orientation as we checked
      // that just before.
      float perc = 0.0f;
      switch (m_orientation) {
        case Orientation::Horizontal:
          perc = local.x() - m_upArrow.box.getRightBound();
          break;
        case Orientation::Vertical:
          // The vertical axis is inverted (meaning that high y values actually corresponds to
          // low range values).
          perc = m_upArrow.box.getBottomBound() - local.y();
        default:
          break;
      }

      perc /= availableSpace;

      // Now compute the desired value for the slider: we know that the `available` space is
      // meant to represent the whole range of this scroll bar so we can deduce the pointed
      // value based on the input position.
      return static_cast<int>(m_minimum + (m_maximum - m_minimum) * perc);
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

      // Finally update the boxes associated to each element with the new textures
      // dimensions: this will allow to correctly render each element.
      utils::Sizef arrow = getEngine().queryTexture(m_upArrow.id);
      utils::Sizef slider = getEngine().queryTexture(m_slider.id);

      switch (m_orientation) {
        case Orientation::Horizontal:
          m_upArrow.box = utils::Boxf(-total.w() / 2.0f + arrow.w() / 2.0f, 0.0f, arrow);
          m_slider.box = utils::Boxf(m_upArrow.box.x() + arrow.w() / 2.0f + slider.w() / 2.0f, 0.0f, slider);
          m_downArrow.box = utils::Boxf(total.w() / 2.0f - arrow.w() / 2.0f, 0.0f, arrow);
          break;
        case Orientation::Vertical:
          m_upArrow.box = utils::Boxf(0.0f, total.h() / 2.0f - arrow.h() / 2.0f, arrow);
          m_slider.box = utils::Boxf(0.0f, m_upArrow.box.y() - arrow.h() / 2.0f - slider.h() / 2.0f, slider);
          m_downArrow.box = utils::Boxf(0.0f, -total.h() / 2.0f + arrow.h() / 2.0f, arrow);
          break;
        default:
          error(
            std::string("Could not perform rendering of scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
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
