
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
      m_cursorIndex(0u),
      m_cursorVisible(false),

      m_fontName(font),
      m_fontSize(size),
      m_font(),

      m_textRole(core::engine::Palette::ColorRole::WindowText),

      m_textChanged(true),

      m_leftText(),
      m_cursor(),
      m_rightText(),

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
    TextBox::drawContentPrivate(const utils::Uuid& uuid,
                                const utils::Boxf& area)
    {
      // Lock the content using the locker provided by the parent class.
      Guard guard(m_propsLocker);

      // Load the text: this should happen only if the text has changed since
      // last draw operation. This can either mean that the text itself has
      // been modified or that one of the rendering properties to use to draw
      // the text has been updated.
      if (textChanged()) {
        // Load the text.
        loadText();

        // The text has been updated.
        m_textChanged = false;
      }

      // Render each part of the text displayed in this text box: depending on
      // the actual content and position of the cursor some parts might be left
      // empty and thus should not be rendered.
      // We maintain a position where each part of the text should be displayed
      // and we perform the intersection with the input `area` which indicates
      // the rectangle to update.
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);

      // Render the left part of the text if it is valid.
      if (m_leftText.valid() && hasLeftTextPart()) {
        // Use the base handler to determine the position of the text and to
        // determine whether it's valid.
        utils::Boxf dstRect = computeLeftTextPosition(sizeEnv);

        // Compute the intersection between the input `area` and this `dstRect`
        // area. If both overlaps it means that part of the text is visible.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          // TODO: Implement repaint.
        }
      }

      // Render the cursor if needed (i.e. if the keyboard focus is active).
      if (m_cursor.valid() && isCursorVisible()) {
        // TODO: Implement repaint.
      }

      // Render the right part of the text if it is valid.
      if (m_rightText.valid() && hasRightTextPart()) {
        // TODO: Implement repaint.
      }

      // TODO: Implement.
      // This could be useful: https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_39.html
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
