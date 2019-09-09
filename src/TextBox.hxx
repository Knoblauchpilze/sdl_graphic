#ifndef    TEXT_BOX_HXX
# define   TEXT_BOX_HXX

# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    inline
    bool
    TextBox::keyboardGrabbedEvent(const core::engine::Event& e) {
      // Update the cursor visible status, considering that as we just grabbed the
      // keyboard focus we are ready to make some modifications on the textbox and
      // thus we should display the cursor.
      updateCursorState(true);

      // Use the base handler method to provide a return value.
      return core::SdlWidget::keyboardGrabbedEvent(e);
    }

    inline
    bool
    TextBox::keyboardReleasedEvent(const core::engine::Event& e) {
      // Update the cursor visible status, considering that as we just lost the
      // keyboard focus the user does not want to perform modifications on the
      // textbox anymore and thus we can hide the cursor.
      updateCursorState(false);

      // Use the base handler method to provide a return value.
      return core::SdlWidget::keyboardReleasedEvent(e);
    }

    inline
    bool
    TextBox::keyReleaseEvent(const core::engine::KeyEvent& e) {
      // TODO: Handle this.
      log("Should handle key " + std::to_string(e.getKey()) + " released", utils::Level::Warning);
      return core::SdlWidget::keyReleaseEvent(e);
    }

    inline
    void
    TextBox::updateCursorState(const bool visible) {
      // Update the cursor's internal state.
      m_cursorVisible = visible;

      // Request a repaint.
      // TODO: Implement this probably through some addition texture displayed at the
      // end of the text's texture.
      log(std::string("Should make cursor ") + (visible ? "visible" : "hidden"), utils::Level::Warning);
    }

    inline
    void
    TextBox::loadText() {
      // Clear existing text if any.
      clearText();

      // Load the text
      if (!m_text.empty()) {
        if (!m_font.valid()) {
          // Load the font.
          m_font = getEngine().createColoredFont(m_fontName, getPalette(), m_fontSize);

          if (!m_font.valid()) {
            error(
              std::string("Cannot create text \"") + m_text + "\"",
              std::string("Invalid null font")
            );
          }
        }

        // We need to render both the left and right part of the text. The left part
        // corresponds to the part of the internal `m_text` before the `m_cursorIndex`
        // value while the right part corresponds to the part that is after that.
        // Depending on the value of the `m_cursorIndex` one of the part might be
        // empty.

        // The left part is not empty if the `m_cursorIndex` is greater than `0`.
        if (m_cursorIndex > 0u) {
          m_leftText = getEngine().createTextureFromText(m_text.substr(0u, m_cursorIndex), m_font, m_textRole);
        }

        // The right part is not empty as long as the `m_cursorIndex` is smaller than
        // `m_text.size()`.
        if (m_cursorIndex < m_text.size()) {
          m_rightText = getEngine().createTextureFromText(m_text.substr(m_cursorIndex), m_font, m_textRole);
        }
      }
    }

    inline
    void
    TextBox::clearText() {
      if (m_leftText.valid()) {
        getEngine().destroyTexture(m_leftText);
        m_leftText.invalidate();
      }

      if (m_rightText.valid()) {
        getEngine().destroyTexture(m_rightText);
        m_rightText.invalidate();
      }
    }

    inline
    bool
    TextBox::textChanged() const noexcept {
      return m_textChanged;
    }

    inline
    void
    TextBox::setTextChanged() noexcept {
      m_textChanged = true;
    }

    inline
    utils::Boxf
    TextBox::computeLeftTextPosition(const utils::Sizef& env) const noexcept {
      // The left part of the text is always on the left part of the widget. It cannot
      // be offseted by the cursor because the whole point of the `left part` of the
      // text is to be on the left of the cursor.
      // We assume that this method should not be called if the `m_leftText` texture is
      // not valid, otherwise we're not able to compute a valid position.
      if (!m_leftText.valid()) {
        error(
          std::string("Could not compute position of the left part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }

      utils::Sizef sizeLeft = getEngine().queryTexture(m_leftText);

      return utils::Boxf(-env.w() / 2.0f + sizeLeft.w() / 2.0f, 0.0f, sizeLeft);
    }

    utils::Boxf
    TextBox::computeCursorPosition(const utils::Sizef& env) const noexcept {
      // We assume here that the keyboard focus state is checked elsewhere. In other terms
      // we're not determining whether this position is actually meaningful given the whole
      // state of this text box.

      // The cursor should be placed right after the left part of the text. To do se we
      // obviously need to retrieve the dimensions of the left part of the text. If this
      // can't be done we're screwed.
      if (!m_leftText.valid()) {
        error(
          std::string("Could not compute cursor position located at index ") + std::to_string(m_cursorIndex),
          std::string("Invalid text texture")
        );
      }

      utils::Sizef sizeLeft = getEngine().queryTexture(m_leftText);

      // Retrieve the dimensions of the cursor so that we can create an accurate position.
      if (!m_cursor.valid()) {
        error(
          std::string("Could not compute cursor position in textbox"),
          std::string("Invalid cursor texture")
        );
      }

      utils::Sizef sizeCursor = getEngine().queryTexture(m_cursor);

      // Use the cursor index to determine whether the cursor can be placed after the left
      // part of the text.
      if (m_cursorIndex == 0) {
        // The cursor position is completely on the left of the widget.
        return utils::Boxf(-env.w() / 2.0f + sizeCursor.w() / 2.0f, 0.0f, sizeCursor);
      }

      // Locate the cursor on the right of the left part of the text.
      return utils::Boxf(-env.w() - 2.0f + sizeLeft.w() + sizeCursor.w() / 2.0f, 0.0f, sizeCursor);
    }

    utils::Boxf
    TextBox::computeRightTextPosition(const utils::Sizef& env) const noexcept {
      // The right part always comes after the left part and the cursor. Unlike the other
      // two it is actually not a failure if the left part of the text does not have a
      // valid texture identifier.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // It is a problem if the cursor is not valid though.
      if (!m_cursor.valid()) {
        error(
          std::string("Could not compute position of the right part of the text in textbox"),
          std::string("Invalid cursor texture")
        );
      }

      utils::Sizef sizeCursor = getEngine().queryTexture(m_cursor);

      // It is also a problem if the right part is not valid.
      if (!m_rightText.valid()) {
        error(
          std::string("Could not compute position of the right part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }

      utils::Sizef sizeRight = getEngine().queryTexture(m_cursor);

      // Locate the right part of the text after the left and cursor part.
      return utils::Boxf(-env.w() / 2.0f + sizeLeft.w() + sizeCursor.w() + sizeRight.w() / 2.0f, 0.0f, sizeRight);
    }

  }
}

#endif    /* TEXT_BOX_HXX */
