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
      // Depending on the type of key pressed by the user we might:
      // - add a new character to the text displayed
      // - move the position of the cursor
      // - remove a character from the text displayed.
      // - do nothing if the key is not handled.
      const bool toReturn = core::SdlWidget::keyReleaseEvent(e);

      // We will handle first the motion of the cursor. It is triggered by using
      // the left and right arrows. The position is updated until no more move is
      // possible in the corresponding direction.
      // TODO: We should handle `Home` and `End` button.
      if (canTriggerCursorMotion(e.getKey())) {
        // Assume left motion and change if needed.
        CursorMotion motion = CursorMotion::Left;
        if (e.getKey() == core::engine::Key::Right || e.getKey() == core::engine::Key::End) {
          motion = CursorMotion::Right;
        }
        const bool fastForward = (
          e.getKey() == core::engine::Key::Home ||
          e.getKey() == core::engine::Key::End
        );

        updateCursorPosition(motion, fastForward);

        // Use the base handler to provide the return value.
        return toReturn;
      }

      // Handle the removal of a character.
      if (e.getKey() == core::engine::Key::BackSpace) {
        removeCharFromText();

        log("Text is now \"" + m_text + "\")", utils::Level::Notice);

        return toReturn;
      }

      // Check whether the key is alphanumeric: if this is not the case we can't do much
      // so we will just trash the event for now.
      if (!e.isAlphaNumeric()) {
        // Use the return value provided by the base handler.
        return toReturn;
      }

      // Add the corresponding char to the internal text.
      addCharToText(e.getChar());

      log("Text is now \"" + m_text + "\")", utils::Level::Notice);

      return toReturn;
    }

    inline
    bool
    TextBox::canTriggerCursorMotion(const core::engine::Key& k) const noexcept {
      return
        k == core::engine::Key::Left ||
        k == core::engine::Key::Right ||
        k == core::engine::Key::Home ||
        k == core::engine::Key::End
      ;
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
    TextBox::updateCursorPosition(const CursorMotion& motion, bool fastForward) {
      // Based on the input direction, try to update the index at which the cursor
      // should be displayed.
      // Detect whether some text is visible in the textbox.
      if (m_text.empty()) {
        // Set the cursor position to `0` to be on the safe side.
        m_cursorIndex = 0;

        return;
      }

      // Depending on the motion direction update the position of the cursor.
      if (motion == CursorMotion::Left) {
        // Check for fast forward.
        if (fastForward) {
          m_cursorIndex = 0;
        }
        else {
          if (m_cursorIndex > 0) {
            --m_cursorIndex;
          }
        }
      }
      else {
        if (fastForward) {
          m_cursorIndex = m_text.size();
        }
        else {
          if (m_cursorIndex < m_text.size()) {
            ++m_cursorIndex;
          }
        }
      }
    }

    inline
    void
    TextBox::addCharToText(const char c) {
      // Insert the char at the position specified by the cursor index.
      m_text.insert(m_text.begin() + m_cursorIndex, c);

      // Update the position of the cursor index so that it stays at the
      // same position.
      ++m_cursorIndex;

      // Mark the text as dirty.
      setTextChanged();
    }

    void
    TextBox::removeCharFromText() {
      // Check whether we can remove anything at all: this is always possible
      // except if:
      // - the string is empty
      // - the cursor index is before the first character of the string.
      // Both conditions can be merged into a single one though because if
      // the text is empty the cursor index will also be set to `0`.
      if (m_cursorIndex == 0) {
        // Not possible to remove anything, do not mark the text as dirty as
        // nothing changed.
        return;
      }

      // Erase the corresponding character.
      m_text.erase(m_text.begin() + m_cursorIndex - 1);

      // The cursor index should be decremented so that it keeps indicating
      // the same character.
      --m_cursorIndex;

      // Also we need to trigger a repaint as the text has changed.
      setTextChanged();
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
    TextBox::isCursorVisible() const noexcept {
      return m_cursorVisible;
    }

    inline
    bool
    TextBox::hasLeftTextPart() const noexcept {
      return m_cursorIndex == 0;
    }

    inline
    bool
    TextBox::hasRightTextPart() const noexcept {
      return m_cursorIndex >= m_text.size();
    }

    inline
    bool
    TextBox::textChanged() const noexcept {
      return m_textChanged;
    }

    inline
    void
    TextBox::setTextChanged() noexcept {
      // Mark the text as dirty.
      m_textChanged = true;

      // Request a repaint.
      requestRepaint();
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

      return utils::Boxf(
        -env.w() / 2.0f + sizeLeft.w() / 2.0f,
        0.0f,
        sizeLeft
      );
    }

    utils::Boxf
    TextBox::computeCursorPosition(const utils::Sizef& env) const noexcept {
      // The cursor should be placed right after the left part of the text. To do se we
      // obviously need to retrieve the dimensions of the left part of the text. If this
      // can't be done it means that the cursor is probably set to be located before the
      // first character displayed in the textbox.
      utils::Sizef sizeLeft;

      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // Retrieve the dimensions of the cursor so that we can create an accurate position.
      // We need to ensure both that the cursor is valid and that it's visible.
      if (!m_cursor.valid()) {
        error(
          std::string("Could not compute cursor position in textbox"),
          std::string("Invalid cursor texture")
        );
      }
      if (!isCursorVisible()) {
        error(
          std::string("Could not compute cursor position in textbox"),
          std::string("Cursor is not visible")
        );
      }

      utils::Sizef sizeCursor = getEngine().queryTexture(m_cursor);

      // Locate the cursor on the right of the left part of the text. If there's no left part
      // of the text (i.e. the cursor is located before the first character) the `sizeLeft`
      // value will be null so we can use it the same way.
      return utils::Boxf(
        -env.w() - 2.0f + sizeLeft.w() + sizeCursor.w() / 2.0f,
        0.0f,
        sizeCursor
      );
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
