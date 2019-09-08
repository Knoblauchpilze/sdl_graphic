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

  }
}

#endif    /* TEXT_BOX_HXX */
