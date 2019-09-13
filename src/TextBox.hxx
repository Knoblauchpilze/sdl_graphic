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
      if (canTriggerCursorMotion(e.getKey())) {
        // Assume left motion and change if needed.
        // TODO: Because the repeat events are processed as `KeyPress` we don't handle
        // them in the text box. Maybe we want to modify this ?
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
      if (e.getKey() == core::engine::Key::BackSpace || e.getKey() == core::engine::Key::Delete) {
        removeCharFromText(e.getKey() == core::engine::Key::Delete);

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
      // Lock this object.
      Guard guard(m_propsLocker);

      // Update the cursor's internal state.
      m_cursorVisible = visible;

      // Request a repaint event.
      requestRepaint();
    }

    inline
    void
    TextBox::updateCursorPosition(const CursorMotion& motion, bool fastForward) {
      // Lock this object.
      Guard guard(m_propsLocker);

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

      // Request a repaint as we modified the position of the cursor.
      setTextChanged();
    }

    inline
    void
    TextBox::addCharToText(const char c) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // Insert the char at the position specified by the cursor index.
      m_text.insert(m_text.begin() + m_cursorIndex, c);

      // Update the position of the cursor index so that it stays at the
      // same position.
      ++m_cursorIndex;

      // Mark the text as dirty.
      setTextChanged();
    }

    inline
    void
    TextBox::removeCharFromText(bool forward) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // Check whether we can remove anything at all. Depending on the value
      // of the `forward` boolean the conditions are not always the same and
      // some configuration might be valid in one case and not in another.
      // In the case of a `forward` suppression, we need to make sure that
      // the cursor is not at the end of the string. On the other hand a non
      // `forward` suppression is only possible when the cursor in not at the
      // beginning of the string.
      // In both cases, the text should not be empty, otherwise we can't do
      // much removal.

      // Check whether the text is empty.
      if (m_text.empty()) {
        // Nothing to be done.
        return;
      }

      // Check trivial cases where the removal is not possible.
      if (forward && m_cursorIndex >= m_text.size()) {
        return;
      }
      if (!forward && m_cursorIndex == 0) {
        return;
      }

      // Compute the index of the character to remove:
      // - in the case of a `forward` suppression we want to remove the character
      //   which is right in front of the current `m_cursorIndex`.
      // - in the case of a `backward` suppression we want to remove a character
      //   right behind the `m_cursorIndex`.
      unsigned toRemove = 0u;

      if (forward) {
        toRemove = m_cursorIndex;
      }
      else {
        toRemove = m_cursorIndex - 1u;
      }

      // Erase the corresponding character.
      m_text.erase(m_text.begin() + toRemove);

      // Now we need to update the cursor position so that it stays at the same
      // position no matter the deletion. In case of a forward deletion we don't
      // actually modify anything before the cursor's position so there's nothing
      // more to do.
      // In the case of a backward suppression we need to decrement the cursor's
      // position in order to keep indicating the same position.
      if (!forward) {
        --m_cursorIndex;
      }

      // Also we need to trigger a repaint as the text has changed.
      setTextChanged();
    }

    inline
    void
    TextBox::loadFont() {
      // Only load the font if it has not yet been done.
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
    }

    inline
    void
    TextBox::loadText() {
      // Clear existing text if any.
      clearText();

      // Load the text.
      if (!m_text.empty()) {
        // Load the font.
        loadFont();

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
    TextBox::loadCursor() {
      // Clear existing cursor if any.
      clearCursor();

      // Load the cursor if needed.
      if (!m_cursor.valid()) {
        // Load the font.
        loadFont();

        // The cursor is actually represented with a '|' character.
        m_cursor = getEngine().createTextureFromText(std::string("|"), m_font, m_textRole);
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
    void
    TextBox::clearCursor() {
      if (m_cursor.valid()) {
        getEngine().destroyTexture(m_cursor);
        m_cursor.invalidate();
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
      return m_cursorIndex != 0u;
    }

    inline
    bool
    TextBox::hasRightTextPart() const noexcept {
      return m_cursorIndex < m_text.size();
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

    inline
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
        -env.w() / 2.0f + sizeLeft.w() + sizeCursor.w() / 2.0f,
        0.0f,
        sizeCursor
      );
    }

    inline
    utils::Boxf
    TextBox::computeRightTextPosition(const utils::Sizef& env) const noexcept {
      // The right part always comes after the left part and the cursor. Unlike the other
      // two it is actually not a failure if the left part of the text does not have a
      // valid texture identifier.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // If the cursor is not valid it might mean that we never actually displayed it. We
      // can check that before failing. Also, no matter whether the texture is valid we
      // only want to use it if the cursor is visible.
      utils::Sizef sizeCursor;

      if (isCursorVisible()) {
        if (!m_cursor.valid()) {
          error(
            std::string("Could not compute position of the right part of the text in textbox"),
            std::string("Invalid cursor texture")
          );
        }

        if (m_cursor.valid()) {
          sizeCursor = getEngine().queryTexture(m_cursor);
        }
      }

      // It is also a problem if the right part is not valid.
      if (!m_rightText.valid()) {
        error(
          std::string("Could not compute position of the right part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }

      utils::Sizef sizeRight = getEngine().queryTexture(m_rightText);

      // Locate the right part of the text after the left and cursor part.
      return utils::Boxf(-env.w() / 2.0f + sizeLeft.w() + sizeCursor.w() + sizeRight.w() / 2.0f, 0.0f, sizeRight);
    }

  }
}

#endif    /* TEXT_BOX_HXX */
