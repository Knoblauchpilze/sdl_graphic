
# include "Slider.hh"
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    Slider::Slider(const std::string& name,
                   float /*value*/,
                   const utils::Vector2f& range,
                   int steps,
                   const std::string& font,
                   unsigned size,
                   SdlWidget* parent,
                   const utils::Sizef& area):
      core::SdlWidget(name,
                      area,
                      parent,
                      sdl::core::engine::Color::NamedColor::Purple),

      m_propsLocker(),

      m_data(RangeData{
        range,
        steps,
        5,

        0u, // TODO: Should use the input `value` after converting it to a number of steps.
        utils::Boxf(),
        utils::Boxf()
      }),

      m_sliderChanged(true),
      m_sliderItem(nullptr),
      m_rulerLine(),
      m_mobileArea(),

      onValueChanged()
    {
      build(font, size);
    }

    void
    Slider::drawContentPrivate(const utils::Uuid& uuid,
                               const utils::Boxf& area)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // Load the slider's visuals if needed.
      if (sliderChanged()) {
        loadSlider();

        m_sliderChanged = false;
      }

      // Repaint mobile parts to their specified position. We need to
      // only consider the input `area` to be repainted: if the visuals
      // do not intersect with it we don't need to repaint it.
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Sizef rlSize = getEngine().queryTexture(m_rulerLine);
      utils::Sizef maSize = getEngine().queryTexture(m_mobileArea);

      // Compute the position of the selection box based on the result from the
      // layout and handle the case where it does not intersect the input area.
      utils::Boxf whereToRL(
        m_sliderItem->getRenderingArea().getCenter(),
        rlSize
      );
      utils::Boxf whereToMA = m_data.maBox;

      utils::Boxf intersectRLWhere = whereToRL.intersect(area);
      utils::Boxf intersectMAWhere = whereToMA.intersect(area);
      utils::Boxf fromRL = convertToLocal(intersectRLWhere, whereToRL);
      utils::Boxf fromMA = convertToLocal(intersectMAWhere, whereToMA);
      utils::Boxf sRLEngine = convertToEngineFormat(fromRL, rlSize);
      utils::Boxf sMAEngine = convertToEngineFormat(fromMA, maSize);
      utils::Boxf dRLEngine = convertToEngineFormat(intersectRLWhere, sizeEnv);
      utils::Boxf dMAEngine = convertToEngineFormat(intersectMAWhere, sizeEnv);

      if (sRLEngine.valid() && dRLEngine.valid()) {
        getEngine().drawTexture(m_rulerLine, &sRLEngine, &uuid, &dRLEngine);
      }
      if (sMAEngine.valid() && dMAEngine.valid()) {
        getEngine().drawTexture(m_mobileArea, &sMAEngine, &uuid, &dMAEngine);
      }
    }

    bool
    Slider::keyPressEvent(const core::engine::KeyEvent& e) {
      // Specific keys are binded to modification of the slider's value.
      // Note that we voluntarily use the key press event in order to be able
      // to react to repeated key events.
      bool update = false;

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      switch (e.getRawKey()) {
        case core::engine::RawKey::Down:
        case core::engine::RawKey::Left:
          // Based on the orientation both keys might trigger a step sub.
          update = performAction(Action::SingleStepSub);
          break;
        case core::engine::RawKey::Up:
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
    Slider::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // We want to detect click on any of the element of the slider to
      // perform an update of the position of the slider. To do so we
      // will use the position of the button when it was released and
      // compare it to the internal position saved for each element.
      // Also we don't want to handle events where the mouse button
      // release event was actually issued after a drag event.
      if (e.wasDragged()) {
        return core::SdlWidget::mouseButtonReleaseEvent(e);
      }

      if (e.getButton() != getInteractionButton()) {
        return core::SdlWidget::mouseButtonReleaseEvent(e);
      }

      bool update = false;

      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      bool increase = m_data.maBox.x() <= local.x();
      bool decrease = m_data.maBox.x() > local.x();

      if (increase) {
        update = performAction(Action::SingleStepAdd);
      }

      if (decrease) {
        update = performAction(Action::SingleStepSub);
      }

      // Request a repaint if needed.
      if (update) {
        requestRepaint();
      }

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseButtonReleaseEvent(e);
    }

    bool
    Slider::mouseDoubleClickEvent(const core::engine::MouseEvent& e) {
      // We will try to perform a motion of some fraction of the page step
      // which allows to move faster when double clicking on elements like
      // the arrows.
      if (e.getButton() != getInteractionButton()) {
        return core::SdlWidget::mouseDoubleClickEvent(e);
      }

      bool update = false;

      utils::Vector2f local = mapFromGlobal(e.getMousePosition());

      // Acquire the lock on the data contained in this widget.
      Guard guard(m_propsLocker);

      bool increase = m_data.maBox.x() <= local.x();
      bool decrease = m_data.maBox.x() > local.x();

      // Note that as a click on the up arrow actually means *decrementing*
      // the value of the slider.
      if (increase) {
        update = performAction(Action::Move, m_data.value + getDoubleClickAdvance(m_data.pageStep));
      }

      if (decrease) {
        update = performAction(Action::Move, m_data.value - getDoubleClickAdvance(m_data.pageStep));
      }

      // Request a repaint if needed.
      if (update) {
        requestRepaint();
      }

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseDoubleClickEvent(e);
    }

    bool
    Slider::mouseDragEvent(const core::engine::MouseEvent& e) {
      // In the case of a drag event we want to move the slider to the positionindicated
      // by the mouse. This includes converting the current mouse position to the local
      // coordinate frame and then determining the equivalent value required to have the
      // slider at this location.
      if (!e.getButtons().isSet(getInteractionButton())) {
        return core::SdlWidget::mouseDragEvent(e);
      }

      // Retrieve the coordinate to follow as described in the input event.
      utils::Vector2f dragStart = mapFromGlobal(e.getInitMousePosition(getInteractionButton()));

      if (!m_data.box.contains(dragStart)) {
        // The drag event did not originated from our widget, do not start
        // a scrolling operation.
        return core::SdlWidget::mouseDragEvent(e);
      }

      // We checked that the start position of the event is inside the scroll bar: this
      // means that we won't start scrolling when the user starts to drag the mouse out
      // of the scroll bar and then bring it onto it, leading to possibly weird behavior
      // where the content associated to the scroll bar is moved abruptly.

      // Retrieve the current mouse position.
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

    void
    Slider::build(const std::string& font,
                  unsigned size)
    {
      // Assign a linear layout which will allow positionning the slider's
      // elements along the label widget representing the current value.
      sdl::graphic::LinearLayoutShPtr layout = std::make_shared<sdl::graphic::LinearLayout>(
        "slider_layout",
        this,
        LinearLayout::Direction::Horizontal,
        getGlobalMargins(),
        2.0f
      );

      setLayout(layout);

      // Create the virtual item holding the space used to represent the
      // slider's element such as the ruler line and the mobile area.
      // This item will be inserted in the layout and used to size these
      // components with adequate dimensions.
      m_sliderItem = std::make_shared<VirtualLayoutItem>(std::string("vitem_for_slider"));
      if (m_sliderItem == nullptr) {
        error(
          std::string("Could not create slider \"") + getName() + "\"",
          std::string("Could not allocate slider's elements")
        );
      }

      // Create the label indicating the current slider's value and set
      // it to display the provided value.
      LabelWidget* label = new LabelWidget(
        getValueLabelName(),
        std::to_string(getValueFromRangeData(m_data)),
        font,
        size,
        LabelWidget::HorizontalAlignment::Left,
        LabelWidget::VerticalAlignment::Center,
        this
      );
      if (label == nullptr) {
        error(
          std::string("Could not create slider \"") + getName() + "\"",
          std::string("Could not allocate slider's value label")
        );
      }

      // Configure components.
      m_sliderItem->setManageWidth(true);
      m_sliderItem->setManageHeight(true);

      label->setMaxSize(
        utils::Sizef(
          getValueLabelMaxWidth(),
          std::numeric_limits<float>::max()
        )
      );
      label->setPalette(getPalette());

      // Build up the layout with the slider's box and the value label.
      layout->addItem(m_sliderItem.get());
      layout->addItem(label);
    }

    void
    Slider::loadSlider() {
      // Clear existing slider's elements.
      clearSlider();

      // We want to create both the ruler line and the mobile area
      // to represent the slider. Note that the internal layout
      // item can be used to fetch the desired size of each one.
      // The ruler line will have a width equal to the total size
      // of the slider's elements, while its height is determined
      // by the internal `getRulerLineHeight` method.
      // The mobile area on the other hand will take up all the
      // vertical space available and an horizontal space specified
      // by the `getMobileAreaWidth` method.
      m_data.box = m_sliderItem->getRenderingArea();
      utils::Sizef elemsSz = m_data.box.toSize();


      // Create colors that contrast with the background.
      core::engine::Color c = getPalette().getBackgroundColor();
      core::engine::Color rlc = getPalette().getLightColor();
      core::engine::Color mac = getPalette().getDarkColor();

      // Create the ruler line
      core::engine::BrushShPtr bRulerLine = std::make_shared<core::engine::Brush>(
        std::string("rl_brush_for_") + getName(),
        false
      );
      bRulerLine->setClearColor(rlc);
      bRulerLine->create(
        utils::Sizef(elemsSz.w(), getRulerLineHeight()),
        true
      );

      m_rulerLine = getEngine().createTextureFromBrush(bRulerLine);

      // Create the mobile area.
      core::engine::BrushShPtr bMobileArea = std::make_shared<core::engine::Brush>(
        std::string("ma_brush_for_") + getName(),
        false
      );

      m_data.maBox.w() = getMobileAreaWidth();
      m_data.maBox.h() = elemsSz.h();

      bMobileArea->setClearColor(mac);
      bMobileArea->create(
        m_data.maBox.toSize(),
        true
      );

      m_mobileArea = getEngine().createTextureFromBrush(bMobileArea);

      if (!m_rulerLine.valid()) {
        error(
          std::string("Could not load slider's visuals"),
          std::string("Invalid ruler line texture")
        );
      }
      if (!m_mobileArea.valid()) {
        error(
          std::string("Could not load slider's visuals"),
          std::string("Invalid mobile area texture")
        );
      }
    }

    void
    Slider::updateSliderPosFromValue() {
      // Unlike some other scrollable components the slider only allows discrete
      // intervals to be taken by the mobile area.
      // Given the vaule we can determine precisely the desired position though.
      // For the sake of explanation below is an example where the number of
      // steps needed by the slider is `4`:
      //      _     _     _     _
      //     | |   | |   | |   | |
      //     +-+---+-+---+-+---+-+
      //     |_|   |_|   |_|   |_|
      //
      //     |___|_____|_____|___|
      //       0    1     2    3
      //
      // We can see that each step is assigned a precise position which can be
      // computed from its index. The only two special cases are the extremities
      // which have a slightly smaller available space.
      // In the case of the determination of the position though one can note
      // that it's actually not an issue. The position grows linearly with the
      // number of steps.

      // Handle cases where the slider is composed of a single step first: this
      // is very easy and allows to discard potential division by `0` afterwards.
      // We also handle the first step separately as it is a bit different as it
      // is shorter. No matter the size of each step the first value is always
      // completely on the left of the available space.
      if (m_data.steps < 2 || m_data.value == 0) {
        m_data.maBox.x() =  m_data.box.getLeftBound() + m_data.maBox.w() / 2.0f;
        m_data.maBox.y() =  0.0f;

        return;
      }

      // Compute the total size occupied by each step.
      float interval = m_data.box.w() / (m_data.steps - 1);

      // The mobile area is offseted:
      //  - by the first interval (which is half an interval wide).
      //  - by (value - 1) complete intervals.
      //  - by a single half interval.
      // So all in all it is offseted by `value` complete intervals.
      // The only case where we need to adjust something if when the
      // value reaches the last interval where we want to offset it
      // by half the size of the mobile area (so that the entire area
      // stays visible).
      m_data.maBox.x() = m_data.box.getLeftBound() + m_data.value * interval;
      m_data.maBox.y() = 0.0f;

      if (m_data.value == m_data.steps - 1) {
        m_data.maBox.x() -= m_data.maBox.w() / 2.0f;
      }
    }

    int
    Slider::getValueFromSliderPos(const utils::Vector2f& local) const {
      // We need to compute the value that would lead to the input position
      // of the mobile area. To do so, we know the range of the slider and
      // also the number of steps that it is divided into.
      // We can extract the closest step from the input position and use it
      // as the value for the slider.

      // In case the position is not contained in the slider's elements
      // general box we will assume the current value is retrieved.
      if (!m_data.box.contains(local)) {
        return m_data.value;
      }

      // Also in the case of a single step we can directly return it.
      if (m_data.steps < 2) {
        return 0;
      }

      // Convert the input position to local coordinate frame (i.e. the frame
      // of the `m_data.box` area).
      utils::Vector2f maLocal = convertToLocal(local, m_data.box);

      // The local position is inside the slider's admissible area: try to
      // determine the value which would lead to a similar value.
      float availableSpace = m_data.box.w();

      // The percentage of the slider is given by one of the two coordinates
      // depending on the orientation. Note that we don't need to check for
      // invalid orientation as we checked that just before.
      float perc = maLocal.x() + m_data.box.w() / 2.0f;
      perc /= availableSpace;

      // The slider's available area is divided as shown below (as an example
      // the number of steps is set to `4`):
      //      _     _     _     _
      //     | |   | |   | |   | |
      //     +-+---+-+---+-+---+-+
      //     |_|   |_|   |_|   |_|
      //
      //     |___|_____|_____|___|
      //       0    1     2    3
      //
      // Depending on the position of the mouse we will assign each interval to
      // a specific step: the ones at the extremities are a little bit smaller
      // but we don't care (actually it is because we don't account for the cases
      // where the mouse is outside of the available area but otherwise the ones
      // at the extremities would have a similar size).
      //
      // Each step occupies `1 / m_data.steps` of the total space, except for the
      // two at the extremities which occupy `1 / m_data.steps` together.
      float interval = 1.0f / (m_data.steps - 1);
      float extInter = interval / 2.0f;

      // Handle first step (as it is shorter than the others).
      if (perc < extInter) {
        return 0;
      }

      // Decrease the percentage to account for the `0` case that we already handled.
      float fStep = (perc - extInter) / interval;

      return 1u + static_cast<int>(std::floor(fStep));
    }

  }
}
