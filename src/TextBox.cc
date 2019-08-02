
# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    TextBox::TextBox(const std::string& name,
                     const std::string& font,
                     const std::string& text,
                     const unsigned& size,
                     SdlWidget* parent,
                     const utils::Sizef& area):
      core::SdlWidget(name, area, parent),
      m_text(text),

      m_fontName(font),
      m_fontSize(size),
      m_font(),

      m_textRole(core::engine::Palette::ColorRole::WindowText),

      m_textChanged(true),

      m_textTex(),

      m_propsLocker()
    {
      // Build the internal state of this box.
      build();
    }

    TextBox::~TextBox() {
      // Clear text.
      clearText();

      // Clear font.
      if (m_font.valid()) {
        getEngine().destroyColoredFont(m_font);
      }
    }

    void
    TextBox::build() {
      // Disable hovering focus: more precisely only allow click focus.
      setFocusPolicy(core::FocusPolicy::ClickFocus);

      // Build a palette which has the same selection color as the base
      // background color.
      core::engine::Palette palette = core::engine::Palette::fromButtonColor(
        core::engine::Color::NamedColor::White
      );

      palette.setColorForRole(core::engine::Palette::ColorRole::Dark, core::engine::Color::NamedColor::White);

      setPalette(palette);
    }

  }
}
