
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
      m_maximum(0),
      m_pageStep(0),

      m_value(0),

      m_elementsChanged(true),
      m_upArrowTex(),
      m_downArrowTex(),
      m_sliderTex(),

      onValueChanged()
    {
      // Build this component.
      build();
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
      m_upArrowTex = getEngine().createTexture(getArrowSize(total), core::engine::Palette::ColorRole::AlternateBase);
      if (!m_upArrowTex.valid()) {
        error(
          std::string("Could not create up arrow to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }

      m_downArrowTex = getEngine().createTexture(getArrowSize(total), core::engine::Palette::ColorRole::AlternateBase);
      if (!m_downArrowTex.valid()) {
        error(
          std::string("Could not create down arrow to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }

      m_sliderTex = getEngine().createTexture(getSliderSize(total), core::engine::Palette::ColorRole::Dark);
      if (!m_sliderTex.valid()) {
        error(
          std::string("Could not create slider to represent scroll bar"),
          std::string("Engine returned invalid uuid")
        );
      }
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

      // The arrows and slider are arranged in a linear fashion where the top arrow is
      // positionned first, then comes the slider and finally the bottom arrow. In the
      // case of an horizontal layout this is still the case but the position are set
      // to be increasing along the horizontal axis.
      utils::Sizef sizeBar = LayoutItem::getRenderingArea().toSize();
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Boxf env = utils::Boxf::fromSize(sizeEnv, true);

      utils::Sizef arrow = getArrowSize(sizeBar);
      utils::Sizef slider = getSliderSize(sizeBar);

      utils::Boxf upArrowPos;
      utils::Boxf sliderPos;
      utils::Boxf downArrowPos;

      switch (m_orientation) {
        case Orientation::Horizontal:
          upArrowPos = utils::Boxf(-sizeBar.w() / 2.0f + arrow.w() / 2.0f, 0.0f, arrow);
          sliderPos = utils::Boxf(upArrowPos.x() + upArrowPos.w() / 2.0f + slider.w() / 2.0f, 0.0f, slider);
          downArrowPos = utils::Boxf(sizeBar.w() / 2.0f - arrow.w() / 2.0f, 0.0f, arrow);
          break;
        case Orientation::Vertical:
          upArrowPos = utils::Boxf(0.0f, sizeBar.h() / 2.0f - arrow.w() / 2.0f, arrow);
          sliderPos = utils::Boxf(0.0f, upArrowPos.y() - upArrowPos.h() / 2.0f - slider.h() / 2.0f, slider);
          downArrowPos = utils::Boxf(0.0f, -sizeBar.h() / 2.0f + arrow.w() / 2.0f, arrow);
          break;
        default:
          error(
            std::string("Could not perform rendering of scroll bar"),
            std::string("Unknown orientation ") + std::to_string(static_cast<int>(m_orientation))
          );
          break;
      }

      log("Arrows size is " + arrow.toString() + ", slider " + slider.toString());
      log("Local area is " + sizeBar.toString() + ", env is " + env.toString());
      log("Up arrow pos is " + upArrowPos.toString() + " with color " + getPalette().getColorForRole(core::engine::Palette::ColorRole::AlternateBase).toString());
      log("Slider pos is " + sliderPos.toString() + " with color " + getPalette().getColorForRole(core::engine::Palette::ColorRole::Dark).toString());
      log("Down arrow pos is " + downArrowPos.toString()+ + " with color " + getPalette().getColorForRole(core::engine::Palette::ColorRole::AlternateBase).toString());

      // Draw each element but only render the part which are actually requested
      // given the input area.
      utils::Boxf dstRectForUpArrow = upArrowPos.intersect(area);
      if (dstRectForUpArrow.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForUpArrow, env);

        utils::Boxf srcRect = convertToLocal(dstRectForUpArrow, upArrowPos);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(arrow, true));

        log("Drawing up arrow from " + srcRect.toString() + " (engine: " + srcRectEngine.toString() + ") to " + dstRectForUpArrow.toString() + " (engine: " + dstRectEngine.toString() + ")");

        // Draw the texture.
        getEngine().drawTexture(m_upArrowTex, &srcRectEngine, &uuid, &dstRectEngine);
      }

      utils::Boxf dstRectForSlider = sliderPos.intersect(area);
      if (dstRectForSlider.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForSlider, env);

        utils::Boxf srcRect = convertToLocal(dstRectForSlider, sliderPos);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(slider, true));

        log("Drawing slider from " + srcRect.toString() + " (engine: " + srcRectEngine.toString() + ") to " + dstRectForSlider.toString() + " (engine: " + dstRectEngine.toString() + ")");

        // Draw the texture.
        getEngine().drawTexture(m_sliderTex, &srcRectEngine, &uuid, &dstRectEngine);
      }

      utils::Boxf dstRectForDownArrow = downArrowPos.intersect(area);
      if (dstRectForDownArrow.valid()) {
        // Convert data to engine format and compute the source area to render.
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectForDownArrow, env);

        utils::Boxf srcRect = convertToLocal(dstRectForDownArrow, downArrowPos);
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(arrow, true));

        log("Drawing slider from " + srcRect.toString() + " (engine: " + srcRectEngine.toString() + ") to " + dstRectForDownArrow.toString() + " (engine: " + dstRectEngine.toString() + ")");

        // Draw the texture.
        getEngine().drawTexture(m_downArrowTex, &srcRectEngine, &uuid, &dstRectEngine);
      }


      // TODO: Implementation.
    }

  }
}
