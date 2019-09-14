
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

      m_selectionStart(m_cursorIndex),
      m_selectionStarted(false),

      m_fontName(font),
      m_fontSize(size),
      m_font(),

      m_textRole(core::engine::Palette::ColorRole::WindowText),

      m_textChanged(true),

      m_leftText(),
      m_cursor(),
      m_rightText(),

      m_selectionBackground(),

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

    bool
    TextBox::keyReleaseEvent(const core::engine::KeyEvent& e) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // TODO: Because the repeat events are processed as `KeyPress` we don't handle
      // them in the text box. Maybe we want to modify this ?
      // TODO: We might also want to allow selection with the mouse ?
      // TODO: Both problems could be partly resolved if we were to refactor the events
      // system by creating a `MouseClick` and a `KeyPressed` both regrouping the mouse
      // button down and mouse button release and same for the key. This would allow more
      // complex events to be built like `Drag` and would overall allow for easier semantic
      // than the current `keyPressed` and `keyReleased`.

      // Depending on the type of key pressed by the user we might:
      // - add a new character to the text displayed.
      // - move the position of the cursor.
      // - remove a character from the text displayed.
      // - stop processing selection.
      // - do nothing if the key is not handled.
      const bool toReturn = core::SdlWidget::keyReleaseEvent(e);

      // We will handle first the motion of the cursor. It is triggered by using
      // the left and right arrows. The position is updated until no more move is
      // possible in the corresponding direction.
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

        // Before updating the cursor position we need to detect when the user
        // starts a selection: this is triggered by using the shift modifier and
        // then moving the cursor. We only want to start a selection if we're not
        // already in the process of selected some text.
        if (e.getModifiers().shiftEnabled() && !selectionStarted()) {
          startSelection();
        }

        // We should also stop the selection in case a motion key is pressed while
        // the shift modifier is not pressed. This actually cancels the update of
        // the cursor position.
        if (!e.getModifiers().shiftEnabled() && selectionStarted()) {
          stopSelection();
        }
        else {
          updateCursorPosition(motion, fastForward);
        }

        // Use the base handler to provide the return value.
        return toReturn;
      }

      // Handle the removal of a character.
      if (e.getKey() == core::engine::Key::BackSpace || e.getKey() == core::engine::Key::Delete) {
        // Perform the character removal.
        removeCharFromText(e.getKey() == core::engine::Key::Delete);

        // Handle the end of the selection if needed: we only want to handle it after
        // performing the deletion of the character(s) because that's how most of the
        // other tools handle it.
        if (!e.getModifiers().shiftEnabled() && selectionStarted()) {
          stopSelection();
        }

        // Return the value provided by the base handler.
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

      // Render the selected part of the text if it is valid.
      if (m_selectionBackground.valid() && hasSelectedTextPart()) {
        // Determine the position of the selected part of the text.
        utils::Boxf dstRect = computeSelectedTextPosition(sizeEnv);

        // Determine whether some part of the selected text should be repainted.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          utils::Sizef sizeSelectedBg = getEngine().queryTexture(m_selectionBackground);

          // TODO: We should probably prevent the filling of the texture at each repaint.
          log("Should fill selection bg with " + getPalette().getColorForRole(getEngine().getTextureRole(m_selectionBackground)).toString());
          getEngine().fillTexture(m_selectionBackground, getPalette(), nullptr);

          // Use the dedicated handler to perform the repaint.
          drawPartOnCanvas(m_selectionBackground, dstRectToUpdate, sizeSelectedBg, dstRect, uuid, env);
        }
      }

      // Render the selected part of the text if it is valid.
      if (m_selectedText.valid() && hasSelectedTextPart()) {
        // Determine the position of the selected part of the text.
        utils::Boxf dstRect = computeSelectedTextPosition(sizeEnv);

        // Determine whether some part of the selected text should be repainted.
        utils::Boxf dstRectToUpdate = dstRect.intersect(area);

        if (dstRectToUpdate.valid()) {
          utils::Sizef sizeSelected = getEngine().queryTexture(m_selectedText);

          // Use the dedicated handler to perform the repaint.
          drawPartOnCanvas(m_selectedText, dstRectToUpdate, sizeSelected, dstRect, uuid, env);
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
    TextBox::removeCharFromText(bool forward) {
      // Check whether we can remove anything at all. Depending on the value
      // of the `forward` boolean the conditions are not always the same and
      // some configuration might be valid in one case and not in another.
      // In the case of a `forward` suppression, we need to make sure that
      // the cursor is not at the end of the string. On the other hand a non
      // `forward` suppression is only possible when the cursor in not at the
      // beginning of the string.
      // In both cases, the text should not be empty, otherwise we can't do
      // much removal.
      // Another aspect to consider is whether a selection is started when
      // entering this function. In this case it means that we might need to
      // remove more than one char from the internal text.

      // Check whether the text is empty.
      if (m_text.empty()) {
        // Nothing to be done.
        return;
      }

      // Check trivial cases where the removal is not possible.
      // Such cases are described below:
      // - no selection is active, a forward removal is asked but the cursor
      //   is at the end of the text.
      // - no selection is active, a backward removal is asked but the
      //   cursor is at the beginning of the text.
      // - the selection is active but no text is actually selected.
      if (selectionStarted()) {
        if (m_cursorIndex == m_selectionStart) {
          return;
        }
      }
      else {
        // Handle cases where no selection is active.
        if (forward && m_cursorIndex >= m_text.size()) {
          return;
        }
        if (!forward && m_cursorIndex == 0) {
          return;
        }
      }

      // Compute the index of the character(s) to remove:
      // - in the case of a `forward` suppression we want to remove the character
      //   which is right in front of the current `m_cursorIndex`.
      // - in the case of a `backward` suppression we want to remove a character
      //   right behind the `m_cursorIndex`.
      // - in the case of an active selection we want to remove all the characters
      //   between the `m_selectionStart` and the `m_cursorIndex`.
      //
      // In order to provide some kind of generic behqvior we will rely on providing
      // two iterators representing the character to erase. This allows to seamlessly
      // handle both the selection and the single character deletion.
      unsigned toRemoveBegin = 0u;
      unsigned toRemoveEnd = 0u;

      if (selectionStarted()) {
        toRemoveBegin = std::min(m_cursorIndex, m_selectionStart);
        toRemoveEnd = std::max(m_cursorIndex, m_selectionStart);
      }
      else {
        if (forward) {
          toRemoveBegin = m_cursorIndex;
          toRemoveEnd = m_cursorIndex + 1u;
        }
        else {
          toRemoveBegin = m_cursorIndex - 1u;
          toRemoveEnd = m_cursorIndex;
        }
      }

      // Erase the corresponding character.
      m_text.erase(m_text.begin() + toRemoveBegin, m_text.begin() + toRemoveEnd);

      // Now we need to update the cursor position so that it stays at the same
      // position no matter the deletion.
      // The first big variation is whether we removed a whole block of text by
      // using a selection or if we just removed a single character. In this
      // mode the `forward` does not mean much because the entire selection is
      // deleted no matter its value. We actually want to move the cursor to the
      // start of the selection.
      // In case of a forward deletion we don't actually modify anything before
      // the cursor's position so there's nothing more to do.
      // In the case of a backward suppression we need to decrement the cursor's
      // position in order to keep indicating the same position.
      if (selectionStarted()) {
        // Put back the cursor after the first character before the deleted part.
        m_cursorIndex = toRemoveBegin;
      }
      else if (!forward) {
        --m_cursorIndex;
      }

      // Also we need to trigger a repaint as the text has changed.
      setTextChanged();
    }

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
    TextBox::computeSelectedTextPosition(const utils::Sizef& env) const noexcept {
      // The selected part of the text is on the right of the left part of the text,
      // and might be before or after the cursor depending on the current position of
      // the cursor compared to the selection start.

      // Retrieve the size of the left part of the text if any.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // Retrieve the size of the cursor text: this is only the case if the current cursor's
      // position is smaller than the starting index of the selection and if the cursor is
      // visible.
      utils::Sizef sizeCursor;
      if (selectionStarted() && m_cursorIndex < m_selectionStart) {
        // Check whether the cursor is valid.
        if (!m_cursor.valid() && isCursorVisible()) {
          error(
            std::string("Could not compute selected text position in textbox"),
            std::string("Invalid cursor texture")
          );
        }

        if (isCursorVisible()) {
          sizeCursor = getEngine().queryTexture(m_cursor);
        }
      }

      // Retrieve the size of the selected text.
      if (!m_selectedText.valid()) {
        error(
          std::string("Could not compute position of the selected part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }
      utils::Sizef sizeSelected = getEngine().queryTexture(m_selectedText);

      return utils::Boxf(
        -env.w() / 2.0f + sizeLeft.w() + sizeCursor.w() + sizeSelected.w() / 2.0f,
        0.0f,
        sizeSelected
      );
    }

    utils::Boxf
    TextBox::computeSelectedBackgroundPosition(const utils::Sizef& env) const noexcept {
      // The selected part of the text is on the right of the left part of the text,
      // and might be before or after the cursor depending on the current position of
      // the cursor compared to the selection start.

      // Retrieve the size of the left part of the text if any.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // Retrieve the size of the cursor text: this is only the case if the current cursor's
      // position is smaller than the starting index of the selection and if the cursor is
      // visible.
      utils::Sizef sizeCursor;
      if (selectionStarted() && m_cursorIndex < m_selectionStart) {
        // Check whether the cursor is valid.
        if (!m_cursor.valid() && isCursorVisible()) {
          error(
            std::string("Could not compute selected text position in textbox"),
            std::string("Invalid cursor texture")
          );
        }

        if (isCursorVisible()) {
          sizeCursor = getEngine().queryTexture(m_cursor);
        }
      }

      // Retrieve the size of the selected text.
      if (!m_selectedText.valid()) {
        error(
          std::string("Could not compute position of the selected part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }
      utils::Sizef sizeSelectedBg = getEngine().queryTexture(m_selectionBackground);

      return utils::Boxf(
        -env.w() / 2.0f + sizeLeft.w() + sizeCursor.w() + sizeSelectedBg.w() / 2.0f,
        0.0f,
        sizeSelectedBg
      );
    }

    utils::Boxf
    TextBox::computeCursorPosition(const utils::Sizef& env) const noexcept {
      // The cursor should be placed after the left part of the text, and after the selected
      // part of the text if the current cursor's index is larger than the selection start.
      // To provide a valid position we need to access the size of the left part of the text
      // if any and also the size of the selected part if any.

      // Retrieve the size of the left part of the text if any.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // Retrieve the size of the selected text: this is only the case if the current cursor's
      // position is larger than the starting index of the selection.
      utils::Sizef sizeSelected;
      if (selectionStarted() && m_cursorIndex > m_selectionStart) {
        // Check whether the selected text is valid.
        if (!m_selectedText.valid()) {
          error(
            std::string("Could not compute cursor position in textbox"),
            std::string("Invalid selected text texture")
          );
        }

        sizeSelected = getEngine().queryTexture(m_selectedText);
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
        -env.w() / 2.0f + sizeLeft.w() + sizeSelected.w() + sizeCursor.w() / 2.0f,
        0.0f,
        sizeCursor
      );
    }

    utils::Boxf
    TextBox::computeRightTextPosition(const utils::Sizef& env) const noexcept {
      // The right part always comes after the left part, the selected part and the cursor.
      
      // Retrieve the size of the left part if any.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
      }

      // Retrieve the size of the selected part if any.
      utils::Sizef sizeSelected;
      if (m_selectedText.valid()) {
        sizeSelected = getEngine().queryTexture(m_selectedText);
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
      return utils::Boxf(-env.w() / 2.0f + sizeLeft.w() + sizeSelected.w() + sizeCursor.w() + sizeRight.w() / 2.0f, 0.0f, sizeRight);
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
