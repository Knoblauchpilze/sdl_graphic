
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

      m_bordersChanged(true)
   {
     build(icon, TextData{text, font, size});
   }

    void
    Button::drawContentPrivate(const utils::Uuid& /*uuid*/,
                               const utils::Boxf& /*area*/)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // Load the borders if needed.
      if (bordersChanged()) {
        loadBorders();

        m_bordersChanged = false;
      }
    }

    void
    Button::stateUpdatedFromFocus(const core::FocusState& /*state*/,
                                  bool /*gainedFocus*/)
    {
      // Acquire the lock on the attributes of this widget.
      Guard guard(m_propsLocker);

      // TODO: Should propagate the info to the children somehow.
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