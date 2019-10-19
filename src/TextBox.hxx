#ifndef    TEXT_BOX_HXX
# define   TEXT_BOX_HXX

# include "TextBox.hh"

namespace sdl {
  namespace graphic {

    inline
    void
    TextBox::setValidator(ValidatorShPtr validator) {
      Guard guard(m_propsLocker);

      // Assign the validator to the internal object. We might reset a previously
      // installed filter by doing so.
      m_validator = validator;
    }

    inline
    std::string
    TextBox::getValue() {
      // Lock this object.
      Guard guard(m_propsLocker);

      // Check whether a validator is set for this object: if this is the case we
      // should apply it on the value displayed, try to fix it if needed and both
      // update the value displayed along with the return value.
      if (m_validator != nullptr) {
        Validator::State s = m_validator->validate(m_text);

        // Check the result of the validation.
        switch (s) {
          case Validator::State::Intermediate:
          case Validator::State::Invalid:
            {
              // If the input is either invalid or intermediate we should try to fix
              // it so that we get a valid value.
              m_validator->fixup(m_text);
              
              // Check whether it is valid.
              std::string text = m_text;
              s = m_validator->validate(text);

              if (s == Validator::State::Valid) {
                // The validator was able to fix the input, let's reflect these changes
                // in the text box.
                m_text = text;

                setTextChanged();
              }
              else {
                // The text was not made valid, continue by keeping this text but by
                // logging something.
                log(
                  std::string("Could not make text \"") + m_text + "\" valid against validator, using it as is",
                  utils::Level::Warning
                );
              }
            }
            break;
          case Validator::State::Valid:
          default:
            // Default behavior is to consider the input valid.
            break;
        }
      }

      // Return the text currently displayed: it is either valid because of
      // the validator or it is not valid and the validator can't fix it so
      // better use it as is.
      return m_text;
    }

    inline
    bool
    TextBox::keyboardGrabbedEvent(const core::engine::Event& e) {
      // Lock the content using the locker provided by the parent class.
      Guard guard(m_propsLocker);

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
      // Lock the content using the locker provided by the parent class.
      Guard guard(m_propsLocker);

      // Update the cursor visible status, considering that as we just lost the
      // keyboard focus the user does not want to perform modifications on the
      // textbox anymore and thus we can hide the cursor.
      updateCursorState(false);

      // Use the base handler method to provide a return value.
      return core::SdlWidget::keyboardReleasedEvent(e);
    }

    inline
    bool
    TextBox::canTriggerCursorMotion(const core::engine::RawKey& k) const noexcept {
      return
        k == core::engine::RawKey::Left ||
        k == core::engine::RawKey::Right ||
        k == core::engine::RawKey::Home ||
        k == core::engine::RawKey::End
      ;
    }

    inline
    void
    TextBox::updateCursorState(const bool visible) {
      bool old = m_cursorVisible;

      // Update the cursor's internal state.
      m_cursorVisible = visible;

      // Request a repaint event if needed.
      if (old != m_cursorVisible) {
        requestRepaint();
      }
    }

    inline
    void
    TextBox::updateCursorPosition(const CursorMotion& motion,
                                  const CursorMotionMode& mode)
    {
      // Based on the input direction, try to update the index at which the cursor
      // should be displayed.
      // Detect whether some text is visible in the textbox.
      if (m_text.empty()) {
        // Set the cursor position to `0` to be on the safe side.
        updateCursorToPosition(0u);

        return;
      }

      // Depending on the motion direction update the position of the cursor.
      if (motion == CursorMotion::Left) {
        // Based on the cursor motion mode we should handle the motion differently.
        // Also we handle the case where the cursor is already on the very left of
        // the text here so that we don't have to handle it for each cursor motion
        // mode.
        if (m_cursorIndex == 0) {
          return;
        }

        switch (mode) {
          case CursorMotionMode::SingleChar:
            updateCursorToPosition(m_cursorIndex - 1u);
            break;
          case CursorMotionMode::ToEnd:
            updateCursorToPosition(0u);
            break;
          case CursorMotionMode::ToWord:
            {
              // Try to find the previous occurrence of a ' ' character. If we can
              // find one we will move to reach it, otherwise we move to the start
              // of the string.
              // Also we want to skip all the ' ' characters to directly reach the
              // next word in case we're standing right on the verge of a space gap.
              std::size_t id = m_cursorIndex - 1u;
              bool gap = (m_text[id] == ' ');

              // Loop until either we reach a space or (if we started at a space)
              // until we reach a real character.
              while (id > 0u && (gap || (!gap && m_text[id] != ' '))) {
                --id;
                if (gap && m_text[id] != ' ') {
                  gap = false;
                }
              }

              // We don't want to move to whatever position we reached unless we did
              // reach the beginning of the string. Indeed in any other case we don't
              // want to move past the space character.
              updateCursorToPosition(id == 0u ? id : id + 1u);
            }
            break;
          case CursorMotionMode::ToWordOrSpace:
            {
              // This mode is similar to the `ToWord` one except we consider a space
              // sequence to also be a `word`.
              std::size_t id = m_cursorIndex - 1u;
              bool gap = (m_text[id] == ' ');

              // Loop until either we reach a space or (if we started at a space)
              // until we reach a real character.
              while (id > 0u && ((gap && m_text[id] == ' ') || (!gap && m_text[id] != ' '))) {
                --id;
              }

              // We don't want to move to whatever position we reached unless we did
              // reach the beginning of the string. Indeed in any other case we don't
              // want to move past the space character.
              updateCursorToPosition(id == 0u ? id : id + 1u);
            }
            break;
          default:
            log("Could not move cursor given mode " + std::to_string(static_cast<int>(mode)), utils::Level::Warning);
            break;
        }
      }
      else {
        // Based on the cursor motion mode we should handle the motion differently.
        // Also we handle the case where the cursor is already on the very right of
        // the text here so that we don't have to handle it for each cursor motion
        // mode.
        if (m_cursorIndex >= m_text.size()) {
          return;
        }

        switch (mode) {
          case CursorMotionMode::SingleChar:
            updateCursorToPosition(m_cursorIndex + 1u);
            break;
          case CursorMotionMode::ToEnd:
            updateCursorToPosition(m_text.size());
            break;
          case CursorMotionMode::ToWord:
            {
              // Try to find the next occurrence of a ' ' character. If we can
              // find one we will move to reach it, otherwise we move to the
              // end of the string.
              // Also we want to skip all the ' ' characters to directly reach the
              // next word in case we're standing right on the verge of a space gap.
              std::size_t id = m_cursorIndex;
              bool gap = (m_text[id] == ' ');

              // Loop until either we reach a space or (if we started at a space)
              // until we reach a real character.
              while (id < m_text.size() && (gap || (!gap && m_text[id] != ' '))) {
                ++id;
                if (gap && m_text[id] != ' ') {
                  gap = false;
                }
              }

              // We want to move to whatever position we reached.
              updateCursorToPosition(id);
            }
            break;
          case CursorMotionMode::ToWordOrSpace:
            {
              // Try to find the next occurrence of a ' ' character. If we can
              // find one we will move to reach it, otherwise we move to the
              // end of the string.
              // Also we want to skip all the ' ' characters to directly reach the
              // next word in case we're standing right on the verge of a space gap.
              std::size_t id = m_cursorIndex;
              bool gap = (m_text[id] == ' ');

              // Loop until either we reach a space or (if we started at a space)
              // until we reach a real character.
              while (id < m_text.size() && ((gap && m_text[id] == ' ') || (!gap && m_text[id] != ' '))) {
                ++id;
              }

              // We want to move to whatever position we reached.
              updateCursorToPosition(id);
            }
            break;
          default:
            log("Could not move cursor given mode " + std::to_string(static_cast<int>(mode)), utils::Level::Warning);
            break;
        }
      }
    }

    inline
    void
    TextBox::updateCursorToPosition(const unsigned pos) {
      const unsigned old = m_cursorIndex;

      // Clamp the position when assigning to the internal value. This formula has
      // the advantage of taking care of empty text displayed.
      m_cursorIndex = std::min(static_cast<unsigned>(m_text.size()), pos);

      // Indicate that the text has changed if needed.
      if (old != m_cursorIndex) {
        setTextChanged();
        setCursorChanged();
      }
    }

    inline
    void
    TextBox::addCharToText(char c) {
      // Insert the char at the position specified by the cursor index.
      m_text.insert(m_text.begin() + m_cursorIndex, c);

      // Update the position of the cursor index so that it stays at the
      // same position.
      ++m_cursorIndex;

      // Mark the text as dirty.
      setTextChanged();

      if (m_validator) {
        Validator::State s = m_validator->validate(m_text);
        if (s == Validator::State::Valid) {
          log("Textbox content \"" + m_text + "\" is valid", utils::Level::Info);
        }
        else if (s == Validator::State::Intermediate) {
          log("Textbox content \"" + m_text + "\" is intermediate", utils::Level::Warning);
        }
        else {
          log("Textbox content \"" + m_text + "\" is invalid", utils::Level::Error);
        }
      }
    }

    inline
    void
    TextBox::startSelection() noexcept {
      // Set the selection has started.
      m_selectionStarted = true;

      // Register the current cursor's index in order to perform the selection.
      m_selectionStart = m_cursorIndex;
    }

    inline
    void
    TextBox::stopSelection() noexcept {
      // Detect cases where the selection was not active.
      if (!m_selectionStarted) {
        log(
          std::string("Stopping selection while none has been started"),
          utils::Level::Warning
        );

        return;
      }

      // Stop the selection.
      m_selectionStarted = false;

      // Request a repaint if the selection contained at least one character:
      // indeed we can go on and request a text changed because the text is
      // no longer selected.
      if (m_selectionStart != m_cursorIndex) {
        setTextChanged();
        setCursorChanged();
      }
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

        // We need to render each part of the text: this include the left part of
        // the text, the selected part and the right part of it.
        // The left part corresponds to the part of the internal text which is both
        // before the cursor and the start of the selection part.
        // The selection part corresponds to the area defined between the cursor's
        // position and the start of the selection.
        // The right part corresponds to the part of the internal text which is both
        // after the cursor and the start of the selection part.
        // Depending on the combination of values for the cursor's position and the
        // selection start some part might be empty.

        // Render each part.
        if (hasLeftTextPart()) {
          // Check for empty text and display a debug to help understand the problem.
          if (getLeftText().empty()) {
            log(
              "Trying to render left text ranging from " + std::to_string(m_selectionStart) + " to " +
              std::to_string(m_cursorIndex) + " which lead to empty text",
              utils::Level::Error
            );
          }

          m_leftText = getEngine().createTextureFromText(getLeftText(), m_font, m_textRole);
        }

        if (hasSelectedTextPart()) {
          // Check for empty text and display a debug to help understand the problem.
          if (getSelectedText().empty()) {
            log(
              "Trying to render selected text ranging from " + std::to_string(m_selectionStart) + " to " +
              std::to_string(m_cursorIndex) + " which lead to empty text",
              utils::Level::Error
            );
          }

          // The role of the selected text is always `HighlightedText`.
          m_selectedText = getEngine().createTextureFromText(
            getSelectedText(),
            m_font,
            core::engine::Palette::ColorRole::HighlightedText
          );

          // Also create the selection background based on the size of the selected text.
          // In order to obtain a fully usable texture we will perform the fill operation
          // for this texture here: this is a one-time operation which should be performed
          // before rendering the texture for the first time. As this method is called by
          // the main thread (through the `drawContentPrivate` interface) we CAN do that.
          utils::Sizef sizeText = getEngine().queryTexture(m_selectedText);
          m_selectionBackground = getEngine().createTexture(
            sizeText,
            core::engine::Palette::ColorRole::Highlight
          );

          if (!m_selectionBackground.valid()) {
            error(
              std::string("Could not create selection background texture"),
              std::string("Engine returned invalid uuid")
            );
          }

          getEngine().fillTexture(m_selectionBackground, getPalette());
        }

        if (hasRightTextPart()) {
          // Check for empty text and display a debug to help understand the problem.
          if (getRightText().empty()) {
            log(
              "Trying to render right text ranging from " + std::to_string(m_selectionStart) + " to " +
              std::to_string(m_cursorIndex) + " which lead to empty text",
              utils::Level::Error
            );
          }

          m_rightText = getEngine().createTextureFromText(getRightText(), m_font, m_textRole);
        }
      }
    }

    inline
    void
    TextBox::loadCursor() {
      // Clear existing cursor if any.
      clearCursor();

      // Load the font.
      loadFont();

      // The cursor is actually represented with a '|' character.
      // Its role is determine by whether it is displayed on top
      // of the selection background: indeed as the background is
      // quite dark, the base cursor's role does not contrast well
      // with it so we usually want to choose another role.
      core::engine::Palette::ColorRole role = (
        selectionStarted() && m_cursorIndex < m_selectionStart ?
        core::engine::Palette::ColorRole::HighlightedText :
        m_textRole
      );

      m_cursor = getEngine().createTextureFromText(std::string("|"), m_font, role);
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

      if (m_selectedText.valid()) {
        getEngine().destroyTexture(m_selectedText);
        m_selectedText.invalidate();
      }

      if (m_selectionBackground.valid()) {
        getEngine().destroyTexture(m_selectionBackground);
        m_selectionBackground.invalidate();
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
    TextBox::selectionStarted() const noexcept {
      return m_selectionStarted;
    }

    inline
    bool
    TextBox::hasLeftTextPart() const noexcept {
      // Basically we have to verify that both the cursor's index and the selection start
      // are greater than `0` which means that a part of the text should still be displayed
      // normally.
      // If no selection is started only the cursor's position is relevant.
      const unsigned lowerBound = (
        selectionStarted() ?
        std::min(m_cursorIndex, m_selectionStart) :
        m_cursorIndex
      );

      // A left part exist if the lower bound is larger than `0`.
      return lowerBound > 0u;
    }

    inline
    std::string
    TextBox::getLeftText() const noexcept {
      // The left part of the text is the part that is not covered neither by the cursor nor
      // by the selection.

      // Assume the left part is only the part left to the cursor's current index.
      unsigned upperBound = m_cursorIndex;

      // If a selection is running, take the minimum between the cursor's index and the
      // selection start: this is the left part of the text.
      if (selectionStarted()) {
        upperBound = std::min(m_cursorIndex, m_selectionStart);
      }

      return m_text.substr(0u, upperBound);
    }

    inline
    bool
    TextBox::hasSelectedTextPart() const noexcept {
      // A selected text part exist if the selection is active and if the interval defined
      // by `[min(cursor position, selection start), max(cursor position, selection start)]`
      // is valid.

      if (!selectionStarted()) {
        return false;
      }

      const unsigned lowerBound = std::min(m_cursorIndex, m_selectionStart);
      const unsigned upperBound = std::max(m_cursorIndex, m_selectionStart);

      return lowerBound != upperBound;
    }

    inline
    std::string
    TextBox::getSelectedText() const noexcept {
      // The selected part of the text is the part that spans the interbal defined by the
      // cursor's current position and the selection start. If no selection is started,
      // the selected part is empty.

      // If no selection is active, no selected text part.
      if (!selectionStarted()) {
        return std::string();
      }

      // If a selection is active define a valid interval between `m_cursorIndex` and the
      // `m_selectionStart`: this corresponds to the selected part.
      unsigned lowerBound = std::min(m_selectionStart, m_cursorIndex);
      unsigned upperBound = std::max(m_selectionStart, m_cursorIndex);

      return m_text.substr(lowerBound, upperBound - lowerBound);
    }

    inline
    bool
    TextBox::hasRightTextPart() const noexcept {
      // Basically we have to verify that both the cursor's index and the selection start
      // are smaller than the length of the internal text which means that a part of the
      // text should still be displayed normally.
      // If no selection is started only the cursor's position is relevant.
      const unsigned upperBound = (
        selectionStarted() ?
        std::max(m_cursorIndex, m_selectionStart) :
        m_cursorIndex
      );

      // A right part exist if the upper bound is smaller than the internal text's size.
      return upperBound < m_text.size();
    }

    inline
    std::string
    TextBox::getRightText() const noexcept {
      // The right part of the text is the part that is not covered neither by the cursor nor
      // by the selection.

      // Assume the right part is only the part right to the cursor's current index.
      unsigned lowerBound = m_cursorIndex;

      // If a selection is running, take the minimum between the cursor's index and the
      // selection start: this is the left part of the text.
      if (selectionStarted()) {
        lowerBound = std::max(m_cursorIndex, m_selectionStart);
      }

      return m_text.substr(lowerBound);
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
    bool
    TextBox::cursorChanged() const noexcept {
      return m_cursorChanged;
    }

    inline
    void
    TextBox::setCursorChanged() noexcept {
      // Follow a similar behavior to `setTextChanged`.
      m_cursorChanged = true;

      requestRepaint();
    }

    inline
    utils::Boxf
    TextBox::computeSelectedBackgroundPosition(const utils::Sizef& env) const noexcept {
      // The selected part of the text is located at the same position as the selected text.
      return computeSelectedTextPosition(env);
    }

  }
}

#endif    /* TEXT_BOX_HXX */
