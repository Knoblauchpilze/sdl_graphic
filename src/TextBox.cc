
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

      // Clear cursor.
      clearCursor();

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

      // Load the cursor if it is visible (otherwise no need to do so).
      if (isCursorVisible()) {
        loadCursor();
      }

      // Render each part of the text displayed in this text box: depending on
      // the actual content and position of the cursor some parts might be left
      // empty and thus should not be rendered.
      // We maintain a position where each part of the text should be displayed
      // and we perform the intersection with the input `area` which indicates
      // the rectangle to update.
      utils::Sizef sizeEnv = getEngine().queryTexture(uuid);
      utils::Boxf env = utils::Boxf::fromSize(sizeEnv, true);

      // Render the left part of the text if it is valid.
      if (m_leftText.valid() && hasLeftTextPart()) {
        // Determine the position of the left part of the text.
        utils::Boxf dstRect = computeLeftTextPosition(sizeEnv);

        // Determine whether some part of the left text should be repainted.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          utils::Sizef sizeLeft = getEngine().queryTexture(m_leftText);

          // Use the dedicated handler to perform the repaint.
          drawPartOnCanvas(m_leftText, dstRectToUpdate, sizeLeft, dstRect, uuid, env);
        }
      }

      // Render the cursor if needed (i.e. if the keyboard focus is active).
      if (m_cursor.valid() && isCursorVisible()) {
        // Determine the position of the cursor.
        utils::Boxf dstRect = computeCursorPosition(sizeEnv);

        // Determine whether some part of the cursor should be repainted.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          utils::Sizef sizeCursor = getEngine().queryTexture(m_cursor);

          // Use the dedicated handler to perform the repaint.
          drawPartOnCanvas(m_cursor, dstRectToUpdate, sizeCursor, dstRect, uuid, env);
        }
      }

      // Render the right part of the text if it is valid.
      if (m_rightText.valid() && hasRightTextPart()) {
        // Determine the position of the right part of the text.
        utils::Boxf dstRect = computeRightTextPosition(sizeEnv);

        // Determine whether some part of the right text should be repainted.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          utils::Sizef sizeRight = getEngine().queryTexture(m_rightText);

          // Use the dedicated handler to perform the repaint.
          drawPartOnCanvas(m_rightText, dstRectToUpdate, sizeRight, dstRect, uuid, env);
        }
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

    void
    TextBox::drawPartOnCanvas(const utils::Uuid& text,
                              const utils::Boxf& localDst,
                              const utils::Sizef& textSize,
                              const utils::Boxf& toRepaint,
                              const utils::Uuid& canvas,
                              const utils::Boxf& env)
    {
      // We want to render the area of the input `text` which should be drawn at
      // the position `localDst` on the parent if it were to be drawn.
      // In order to find the actual area to repaint in `text` coordinate frame
      // we can use the provided `toRepaint` which represents in parent frame
      // (which is, in the same coordinate frame than the `localDst`) the area
      // which actually needs to be repainted.
      //
      // The `env` represents the area of the parent canvas and allows to figure
      // how the `localDst` should be expressed (it basically transforms the area
      // into a top left corner based one).

      // First retrieve the source area to repaint from the `text` texture.
      utils::Boxf srcRect = convertToLocal(localDst, toRepaint);

      // Convert both the source and destination areas to engine format.
      utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(textSize, true));
      utils::Boxf dstRectEngine = convertToEngineFormat(localDst, env);

      // Draw the `text` onto the `canvas` at last.
      getEngine().drawTexture(text, &srcRectEngine, &canvas, &dstRectEngine);
    }

  }
}
