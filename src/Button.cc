
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
                   unsigned size,
                   core::SdlWidget* parent,
                   const utils::Sizef& area):
      core::SdlWidget(name, area, parent, core::engine::Color::NamedColor::Yellow),

      m_propsLocker(),

      m_bordersChanged(true),
      m_borders(BordersData{
        utils::Uuid(),
        utils::Uuid(),
        utils::Uuid(),
        utils::Uuid(),
        false
      })
    {
      build(icon, TextData{text, font, size});
    }

    void
    Button::drawContentPrivate(const utils::Uuid& uuid,
                               const utils::Boxf& /*area*/)
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
      // TODO: Handle the area.
      utils::Boxf thisArea = LayoutItem::getRenderingArea().toOrigin();
      utils::Sizef hSize = getEngine().queryTexture(m_borders.hLightBorder);
      utils::Sizef vSize = getEngine().queryTexture(m_borders.vLightBorder);

      utils::Uuid vl = (m_borders.pressed ? m_borders.vDarkBorder : m_borders.vLightBorder);
      utils::Uuid vr = (m_borders.pressed ? m_borders.vLightBorder : m_borders.vDarkBorder);
      utils::Uuid ht = (m_borders.pressed ? m_borders.vDarkBorder : m_borders.vLightBorder);
      utils::Uuid hb = (m_borders.pressed ? m_borders.vLightBorder : m_borders.vDarkBorder);

      // Vertical borders.
      utils::Boxf vFromL(-thisArea.w() / 2.0f + vSize.w() / 2.0f, 0.0f, vSize);
      utils::Boxf vFromLEngine = convertToEngineFormat(vFromL, thisArea);

      utils::Boxf vFromR(thisArea.w() / 2.0f - vSize.w() / 2.0f, 0.0f, vSize);
      utils::Boxf vFromREngine = convertToEngineFormat(vFromR, thisArea);

      getEngine().drawTexture(vl, nullptr, &uuid, &vFromLEngine);
      getEngine().drawTexture(vr, nullptr, &uuid, &vFromREngine);

      // Horizontal borders.
      utils::Boxf hFromT(0.0f, thisArea.h() / 2.0f - hSize.h() / 2.0f, hSize);
      utils::Boxf hFromTEngine = convertToEngineFormat(hFromT, thisArea);

      utils::Boxf hFromB(0.0f, -thisArea.h() / 2.0f + hSize.h() / 2.0f, hSize);
      utils::Boxf hFromBEngine = convertToEngineFormat(hFromB, thisArea);

      getEngine().drawTexture(ht, nullptr, &uuid, &hFromTEngine);
      getEngine().drawTexture(hb, nullptr, &uuid, &hFromBEngine);
    }

    void
    Button::stateUpdatedFromFocus(const core::FocusState& /*state*/,
                                  bool /*gainedFocus*/)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // TODO: Should propagate the info to the children somehow.
      // Note that doing nothing here prevents the repaint of this
      // element which makes no update on hovering and thus kinda
      // solve the problem.
      // TODO: Should emit click.
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
        getBorderDims(),
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
