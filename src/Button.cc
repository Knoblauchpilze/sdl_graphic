
# include "Button.hh"
# include "LinearLayout.hh"
# include "PictureWidget.hh"
# include "LabelWidget.hh"

namespace sdl {
  namespace graphic {

    Button::Button(const std::string& name,
                   const std::string& text,
                   const std::string& icon,
                   const std::string& font,
                   const button::Type& type,
                   unsigned size,
                   core::SdlWidget* parent,
                   float bordersSize,
                   const utils::Sizef& area,
                   const core::engine::Color& color):
      core::SdlWidget(name, area, parent, color),

      m_propsLocker(),

      m_type(type),

      m_bordersChanged(true),
      m_borders(BordersData{
        utils::Uuid(),
        utils::Uuid(),
        utils::Uuid(),
        utils::Uuid(),

        std::max(0.0f, bordersSize),

        false
      }),
      m_state(State::Released),

      onButtonToggled()
    {
      build(icon, TextData{text, font, size});
    }

    void
    Button::drawContentPrivate(const utils::Uuid& uuid,
                               const utils::Boxf& area)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // Load the borders if needed.
      if (bordersChanged()) {
        loadBorders();

        m_bordersChanged = false;
      }

      // Repaint the borders on the side of the widget. Note that we assume that both
      // areas are similar in size (light and dark ones).
      utils::Boxf thisArea = LayoutItem::getRenderingArea().toOrigin();
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Sizef hSize = getEngine().queryTexture(m_borders.hLightBorder);
      utils::Sizef vSize = getEngine().queryTexture(m_borders.vLightBorder);

      // Determine which borders should be displayed where based on the status of this
      // button. According to whether it is pressed we will alternate the dark and light
      // borders to create a feeling of depth.
      utils::Uuid vl = (m_borders.pressed ? m_borders.vDarkBorder : m_borders.vLightBorder);
      utils::Uuid vr = (m_borders.pressed ? m_borders.vLightBorder : m_borders.vDarkBorder);
      utils::Uuid ht = (m_borders.pressed ? m_borders.vDarkBorder : m_borders.vLightBorder);
      utils::Uuid hb = (m_borders.pressed ? m_borders.vLightBorder : m_borders.vDarkBorder);

      // Compute the position of each border based on its size and the size of this area.
      utils::Boxf vFromL(-thisArea.w() / 2.0f + vSize.w() / 2.0f, 0.0f, vSize);
      utils::Boxf vFromR(thisArea.w() / 2.0f - vSize.w() / 2.0f, 0.0f, vSize);
      utils::Boxf hFromT(0.0f, thisArea.h() / 2.0f - hSize.h() / 2.0f, hSize);
      utils::Boxf hFromB(0.0f, -thisArea.h() / 2.0f + hSize.h() / 2.0f, hSize);

      // Intersect these boxes with the input area to determine which part of the borders
      // should be displayed.
      utils::Boxf dVFromL = vFromL.intersect(area);
      utils::Boxf dVFromR = vFromR.intersect(area);
      utils::Boxf dHFromT = hFromT.intersect(area);
      utils::Boxf dHFromB = hFromB.intersect(area);

      // Determine the source area from the output area.
      utils::Boxf sVFromL = convertToLocal(dVFromL, vFromL);
      utils::Boxf sVFromR = convertToLocal(dVFromR, vFromR);
      utils::Boxf sHFromT = convertToLocal(dHFromT, hFromT);
      utils::Boxf sHFromB = convertToLocal(dHFromB, hFromB);

      // Convert boxes to engine format.
      utils::Boxf sVFromLEngine = convertToEngineFormat(sVFromL, vSize);
      utils::Boxf sVFromREngine = convertToEngineFormat(sVFromR, vSize);
      utils::Boxf sHFromTEngine = convertToEngineFormat(sHFromT, hSize);
      utils::Boxf sHFromBEngine = convertToEngineFormat(sHFromB, hSize);

      utils::Boxf dVFromLEngine = convertToEngineFormat(dVFromL, sizeEnv);
      utils::Boxf dVFromREngine = convertToEngineFormat(dVFromR, sizeEnv);
      utils::Boxf dHFromTEngine = convertToEngineFormat(dHFromT, sizeEnv);
      utils::Boxf dHFromBEngine = convertToEngineFormat(dHFromB, sizeEnv);

      // Draw borders. We want the dark borders to always be displayed on top so
      // that they get most of the area. This also guarantees consistent visual
      // aspect for the button.
      if (m_borders.pressed) {
        if (dVFromR.valid()) {
          getEngine().drawTexture(vr, &sVFromREngine, &uuid, &dVFromREngine);
        }
        if (dHFromB.valid()) {
          getEngine().drawTexture(hb, &sHFromBEngine, &uuid, &dHFromBEngine);
        }
        if (dVFromL.valid()) {
          getEngine().drawTexture(vl, &sVFromLEngine, &uuid, &dVFromLEngine);
        }
        if (dHFromT.valid()) {
          getEngine().drawTexture(ht, &sHFromTEngine, &uuid, &dHFromTEngine);
        }
      }
      else {
        if (dVFromL.valid()) {
          getEngine().drawTexture(vl, &sVFromLEngine, &uuid, &dVFromLEngine);
        }
        if (dHFromT.valid()) {
          getEngine().drawTexture(ht, &sHFromTEngine, &uuid, &dHFromTEngine);
        }
        if (dVFromR.valid()) {
          getEngine().drawTexture(vr, &sVFromREngine, &uuid, &dVFromREngine);
        }
        if (dHFromB.valid()) {
          getEngine().drawTexture(hb, &sHFromBEngine, &uuid, &dHFromBEngine);
        }
      }
    }

    void
    Button::build(const std::string& icon,
                  const TextData& text)
    {
      // Assign a linear layout which will allow positionning text and icon.
      sdl::graphic::LinearLayoutShPtr layout = std::make_shared<sdl::graphic::LinearLayout>(
        "button_layout",
        this,
        LinearLayout::Direction::Horizontal,
        m_borders.size,
        2.0f
      );

      // And assign the layout to this widget.
      setLayout(layout);

      // We need to register the icon and the text associated to this button.
      PictureWidget* pic = nullptr;
      if (!icon.empty()) {
        pic = new PictureWidget(
          std::string("button_icon"),
          icon,
          PictureWidget::Mode::Fit,
          this
        );

        if (pic == nullptr) {
          error(
            std::string("Could not create button \"") + getName() + "\"",
            std::string("Could not allocate icon \"") + icon + "\""
          );
        }

        pic->setMaxSize(getIconMaxDims());
        pic->setFocusPolicy(core::FocusPolicy());
      }

      LabelWidget* label = nullptr;
      if (!text.title.empty()) {
        label = new LabelWidget(
          std::string("button_label"),
          text.title,
          text.font,
          text.size,
          LabelWidget::HorizontalAlignment::Left,
          LabelWidget::VerticalAlignment::Center,
          this
        );

        if (label == nullptr) {
          error(
            std::string("Could not create button \"") + getName() + "\"",
            std::string("Could not allocate text \"") + text.title + "\""
          );
        }

        label->setFocusPolicy(core::FocusPolicy());
        label->setPalette(getPalette());
      }

      // Disable log for various component.
      layout->allowLog(false);
      if (pic != nullptr) {
        pic->allowLog(false);
      }
      if (label != nullptr) {
        label->allowLog(false);
      }

      // Add each item to the layout if needed.
      if (pic != nullptr) {
        layout->addItem(pic);
      }
      if (label != nullptr) {
        layout->addItem(label);
      }
    }

  }
}
