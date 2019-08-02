#ifndef    TEXT_BOX_HXX
# define   TEXT_BOX_HXX

# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    inline
    bool
    TextBox::keyReleaseEvent(const core::engine::KeyEvent& e) {
      // TODO: Handle this.
      log("Should handle key " + std::to_string(e.getKey()) + " released", utils::Level::Warning);
      return core::SdlWidget::keyReleaseEvent(e);
    }

    inline
    bool
    TextBox::updateStateFromFocus(const core::engine::FocusEvent::Reason& reason,
                                  const bool gainedFocus)
    {
      // First apply the base class handler to determine we should update the
      // text's role at all.
      const bool updated = core::SdlWidget::updateStateFromFocus(reason, gainedFocus);

      // If the base class has handled the focus reason, we can display the cursor
      // at the end of the text or hide it if the focus was lost.
      if (updated) {
        updateCursorState(gainedFocus);
      }

      // Return the value provided by the base handler.
      return updated;
    }

    inline
    void
    TextBox::updateCursorState(const bool visible) {
      // TODO: Implement this probably through some addition texture displayed at the
      // end of the text's texture.
      log(std::string("Should make cursor ") + (visible ? "visible" : "hidden"), utils::Level::Warning);
    }

    inline
    void
    TextBox::loadText() const {
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

        m_textTex = getEngine().createTextureFromText(m_text, m_font, m_textRole);
      }
    }

    inline
    void
    TextBox::clearText() const {
      if (m_textTex.valid()) {
        getEngine().destroyTexture(m_textTex);
        m_textTex.invalidate();
      }
    }

    inline
    bool
    TextBox::textChanged() const noexcept {
      return m_textChanged;
    }

    inline
    void
    TextBox::setTextChanged() const noexcept {
      m_textChanged = true;
    }

  }
}

#endif    /* TEXT_BOX_HXX */
