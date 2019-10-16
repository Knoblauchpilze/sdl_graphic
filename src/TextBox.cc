
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
      m_cursorChanged(true),

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

      m_propsLocker(),

      m_validator(nullptr)
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
    TextBox::keyPressEvent(const core::engine::KeyEvent& e) {
      // Lock this object.
      Guard guard(m_propsLocker);

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
      if (canTriggerCursorMotion(e.getRawKey())) {
        // Assume left motion and change if needed.
        CursorMotion motion = CursorMotion::Left;
        if (e.getRawKey() == core::engine::RawKey::Right || e.getRawKey() == core::engine::RawKey::End) {
          motion = CursorMotion::Right;
        }
        CursorMotionMode mode = CursorMotionMode::SingleChar;
        if (core::engine::ctrlEnabled(e.getModifiers())) {
          mode = CursorMotionMode::ToWord;
        }
        if (e.getRawKey() == core::engine::RawKey::Home || e.getRawKey() == core::engine::RawKey::End) {
          mode = CursorMotionMode::ToEnd;
        }

        // Before updating the cursor position we need to detect when the user
        // starts a selection: this is triggered by using the shift modifier and
        // then moving the cursor. We only want to start a selection if we're not
        // already in the process of selected some text.
        if (core::engine::shiftEnabled(e.getModifiers()) && !selectionStarted()) {
          startSelection();
        }

        // We should also stop the selection in case a motion key is pressed while
        // the shift modifier is not pressed. This actually cancels the update of
        // the cursor position.
        if (!core::engine::shiftEnabled(e.getModifiers()) && selectionStarted()) {
          stopSelection();
        }
        else {
          updateCursorPosition(motion, mode);
        }

        // Use the base handler to provide the return value.
        return toReturn;
      }

      // Handle the removal of a character.
      if (e.getRawKey() == core::engine::RawKey::BackSpace || e.getRawKey() == core::engine::RawKey::Delete) {
        // Perform the character removal.
        removeCharFromText(e.getRawKey() == core::engine::RawKey::Delete);

        // Handle the end of the selection if needed: we only want to handle it after
        // performing the deletion of the character(s) because that's how most of the
        // other tools handle it.
        if (!core::engine::shiftEnabled(e.getModifiers()) && selectionStarted()) {
          stopSelection();
        }

        // Return the value provided by the base handler.
        return toReturn;
      }

      // Check whether the key is authorized: in a first approach we will rely on
      // the fact that the character should be printable. If this is not the case
      // there's no point in trying to display it.
      // The validator is used later on when the user wants to retrieve the value
      // that is stored in this textbox.
      if (!e.isPrintable()) {
        // Use the return value provided by the base handler.
        return toReturn;
      }

      // We should stop the selection if any and remove the characters selected so far.
      if (selectionStarted()) {
        // Given that we have a selection active it does not really matter whether we use
        // the `forward` suppression. For good measure though we will do as if we pressed
        // the `Delete` key.
        removeCharFromText(true);

        // Stop the selection.
        stopSelection();
      }

      // Add the corresponding char to the internal text.
      addCharToText(e.getChar());

      return toReturn;
    }

    bool
    TextBox::mouseButtonReleaseEvent(const core::engine::MouseEvent& e) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // The goal here is to move the cursor between the characters which are closest
      // of the location of the click. A summary of the algorithm consists in moving
      // through he text displayed on the screen and to select the character whose
      // position is the last that is smallest than the location of the click.
      // Such an action resets any active selection but does not actually change any
      // characters displayed.
      // We only want to apply this behavior when the textbox already has the keyboard
      // focus: this means that we should first call the base method to handle the
      // focus and then perform the necessary modifications of the position of the
      // cursor.
      bool toReturn = core::SdlWidget::mouseButtonReleaseEvent(e);

      // Also we have to take care of the special case of the first button release
      // event issued after a drag event. Indeed the events system provides such an
      // event in the case someone is not tracking drag events but still wants to
      // receive the notification about clicks.
      // As for this class, the button release is used to move the cursor to the
      // desired position and stop the selection which is not what we want in the
      // case of a drag event. So we need to prevent the rest of this function from
      // being executed in this case.
      if (e.wasDragged()) {
        return toReturn;
      }

      // Get the local position of the click.
      utils::Vector2f localClick = mapFromGlobal(e.getMousePosition());

      // Determine the index of the character closest to the click position.
      unsigned idChar = closestCharacterFrom(localClick);

      // Stop selection if any.
      if (selectionStarted()) {
        stopSelection();
      }

      // Move the cursor to the specified index.
      updateCursorToPosition(idChar);

      // Use the base handler to provide the return value.
      return toReturn;
    }

    bool
    TextBox::mouseDoubleClickEvent(const core::engine::MouseEvent& e) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // Perform a selection of the entirety of the text inserted in the textbox. We will
      // also move the cursor to the end of the displayed text.
      updateCursorPosition(CursorMotion::Left, CursorMotionMode::ToWordOrSpace);
      startSelection();
      updateCursorPosition(CursorMotion::Right, CursorMotionMode::ToWordOrSpace);

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseDoubleClickEvent(e);
    }

    bool
    TextBox::mouseDragEvent(const core::engine::MouseEvent& e) {
      // Lock this object.
      Guard guard(m_propsLocker);

      // We only want to react if the drag event includes at least the left mouse
      // button: this is the button triggering the selection behavior.
      core::engine::mouse::Button sensitive = core::engine::mouse::Button::Left;

      if (!e.getButtons().isSet(sensitive)) {
        // We are not interested in this event.
        return core::SdlWidget::mouseDragEvent(e);
      }

      // The left mouse button is part of the drag event. We need to perform the
      // selection of the text spanned by the area covered by the drag event.
      // Also we want to update the cursor's position to always be as close of
      // the mouse's cursor as possible. In order to do all that we need to first
      // determine the indices of the characters which are closer of both the
      // initial position where the drag started and the current position of the
      // mouse: this will represent the desired selection area.
      utils::Vector2f start = mapFromGlobal(e.getInitMousePosition(sensitive));
      utils::Vector2f cur = mapFromGlobal(e.getMousePosition());

      // Determine the character closest to each position.
      unsigned idStart = closestCharacterFrom(start);
      unsigned idCur = closestCharacterFrom(cur);

      // We now need to update the selected text based on the above values. As
      // a drag event is something rather long, we might not update the text
      // selected each time so we should be careful not to request too many
      // repaints.
      if (!selectionStarted()) {
        // The selection has not started: we need to ensure that the cursor is
        // at the character targeted by the drag event before starting the
        // selection.
        updateCursorToPosition(idStart);

        // Now start the selection.
        startSelection();

        // And move the cursor to the current position.
        updateCursorToPosition(idCur);
      }
      else {
        // The selection already started: we can verify that he starting index
        // is still relevant compared to the local start of the selection.
        if (m_selectionStart != idStart) {
          // This is weird.
          log(
            std::string("Drag event references beginning at character ") + std::to_string(idStart) +
            " but internal registered value was " + std::to_string(m_selectionStart),
            utils::Level::Warning
          );

          m_selectionStart = idStart;
        }

        // Move the cursor to the desired index.
        updateCursorToPosition(idCur);
      }

      // Also set the cursor to visible if it is not already the case.
      updateCursorState(true);

      // Use the base handler to provide a return value.
      return core::SdlWidget::mouseDragEvent(e);
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
      if (cursorChanged()) {
        loadCursor();

        m_cursorChanged = false;
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
        drawPartOnCanvas(m_leftText, computeLeftTextPosition(sizeEnv), uuid, env, area);
      }

      // Render the selected part of the text if it is valid.
      if (m_selectionBackground.valid() && hasSelectedTextPart()) {
        drawPartOnCanvas(m_selectionBackground, computeSelectedTextPosition(sizeEnv), uuid, env, area);
      }

      // Render the selected part of the text if it is valid.
      if (m_selectedText.valid() && hasSelectedTextPart()) {
        drawPartOnCanvas(m_selectedText, computeSelectedTextPosition(sizeEnv), uuid, env, area);
      }

      // Render the cursor if needed (i.e. if the keyboard focus is active).
      if (m_cursor.valid() && isCursorVisible()) {
        drawPartOnCanvas(m_cursor, computeCursorPosition(sizeEnv), uuid, env, area);
      }

      // Render the right part of the text if it is valid.
      if (m_rightText.valid() && hasRightTextPart()) {
        drawPartOnCanvas(m_rightText, computeRightTextPosition(sizeEnv), uuid, env, area);
      }
    }

    void
    TextBox::build() {
      // Disable hovering focus: more precisely only allow click focus.
      setFocusPolicy(core::FocusPolicy(core::focus::Type::Click));

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
        updateCursorToPosition(toRemoveBegin);
      }
      else if (!forward) {
        updateCursorToPosition(m_cursorIndex - 1u);
      }

      // Also we need to trigger a repaint as the text has changed.
      setTextChanged();
    }

    unsigned
    TextBox::closestCharacterFrom(const utils::Vector2f& pos) const noexcept {
      // We want to determine the character that fits the following description:
      //  - any string smaller than the one including this character does not reach
      //    the input `pos`.
      //  - any string larger than the one terminating at this character has its
      //    last character completely beyond the input `pos`.
      // In order to determine this index, we will use the dedicated `SDL TTF API`
      // method which allows to compute the dimension of a string as if it was
      // rendered for display. We will loop through the text displayed in this box
      // and pick the character fitting the above conditions.
      // Note that to provide the most exact detection of the character we actually
      // account for intra-character selection, meaning that if the user clicks on
      // the left half of a character, the cursor will be positionned before this
      // character while if the cursor is on the right half of the character upon
      // clicking we will position the cursor after the character.

      // Handle the case where the font is not valid.
      if (!m_font.valid()) {
        log(
          std::string("Could not find closest character from position ") + pos.toString() + ", font not loaded",
          utils::Level::Warning
        );

        return 0u;
      }

      // Start at the beginning of the text displayed in this box.
      unsigned id = 0u;
      bool valid = false;
      utils::Sizef textSize;

      utils::Sizef area = core::LayoutItem::getRenderingArea().toSize();

      while (!valid && id <= m_text.size()) {
        // Render the string containing the characters until `id` and check whether
        // the click is now on the left side of the rendered string.
        textSize = getEngine().getTextSize(m_text.substr(0u, id), m_font, false);

        // Check whether the size of the text is now encompassing the input position.
        if (-area.w() / 2.0f + textSize.w() >= pos.x()) {
          // We found the character we wanted.
          valid = true;
        }
        else {
          // The string composed of the text up to the `id`-th character is not enough
          // to span the input `pos`, continue accumulating characters.
          ++id;
        }
      }

      // We determined the character which allows to move from left to right of the
      // cursor. We know need to determine whether the cursor should be placed on
      // the left or on the right of the character.
      // This is done by computing the size of the text without the last character
      // and determining if the cursor lies in the first half of it or on the second
      // half.
      // The only special cases are when the cursor is either too far left or too far
      // right which means that we should actually clamp it to be on the first or
      // last character of the text displayed.
      if (!valid || id == 0) {
        return id;
      }

      utils::Sizef sizeWithoutLast = getEngine().getTextSize(m_text.substr(0u, id - 1), m_font, false);

      const float delta = textSize.w() - sizeWithoutLast.w();
      const float offset = pos.x() + area.w() / 2.0f - sizeWithoutLast.w();

      if (offset <= delta / 2.0f) {
        --id;
      }

      // Return the found id.
      return id;
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
      // The selected part of the text is on the right of the left part of the text
      // but before the rigth part. This means that if the text is selected until
      // the end of the string there's actually no right part.

      // Retrieve the size of the left part of the text if any.
      utils::Sizef sizeLeft;
      if (m_leftText.valid()) {
        sizeLeft = getEngine().queryTexture(m_leftText);
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
        -env.w() / 2.0f + sizeLeft.w() + sizeSelected.w() / 2.0f,
        0.0f,
        sizeSelected
      );
    }

    utils::Boxf
    TextBox::computeCursorPosition(const utils::Sizef& env) const noexcept {
      // The cursor should be placed at the location specified by the `m_cursorIndex`.
      // In order to determine the position we have to rely on the engine method allowing
      // to determine the length of a rendered text: this will allow to precisely position
      // the cursor after the targeted character.
      // We assume that the cursor is visible when calling this method. We also verify
      // that the associated texture is valid because we have to use its dimensions to
      // position it accurately.
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
      if (!m_font.valid()) {
        error(
          std::string("Could not compute cursor position in textbox"),
          std::string("Font is not valid")
        );
      }

      // Query the size of the text up to the `m_cursorIndex`-nth character: this will
      // provide an offset to localize the cursor's texture on this textbox.
      utils::Sizef text = getEngine().getTextSize(m_text.substr(0u, m_cursorIndex), m_font, false);

      // The cursor should be positionned right after that.
      utils::Sizef sizeCursor = getEngine().getTextSize("|", m_font, true);

      return utils::Boxf(
        -env.w() / 2.0f + text.w() + sizeCursor.w() / 2.0f,
        0.0f,
        sizeCursor
      );
    }

    utils::Boxf
    TextBox::computeRightTextPosition(const utils::Sizef& env) const noexcept {
      // The right part always comes after the left part and the selected part.
      // If the selected part encompasses the last characters of the string we
      // have effectively no right part.

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

      // It is also a problem if the right part is not valid.
      if (!m_rightText.valid()) {
        error(
          std::string("Could not compute position of the right part of the text in textbox"),
          std::string("Invalid text texture")
        );
      }
      utils::Sizef sizeRight = getEngine().queryTexture(m_rightText);

      // Locate the right part of the text after the left and cursor part.
      return utils::Boxf(-env.w() / 2.0f + sizeLeft.w() + sizeSelected.w() + sizeRight.w() / 2.0f, 0.0f, sizeRight);
    }

    void
    TextBox::drawPartOnCanvas(const utils::Uuid& uuid,
                              const utils::Boxf& localDst,
                              const utils::Uuid& canvas,
                              const utils::Boxf& env,
                              const utils::Boxf& toUpdate)
    {
      // Determine whether some part of the input `uuid` texture are spanned by the
      // area to update.
      utils::Boxf dstRectToUpdate = localDst.intersect(toUpdate);

      if (dstRectToUpdate.valid()) {
        // Some portion of the `uuid` should be repainted as it matches the area to
        // update. Retrieve the dimensions of the `uuid` texture.
        utils::Sizef sizeText = getEngine().queryTexture(uuid);

        // Convert the area which should be repaint to the local `uuid` coordinate
        // frame: indeed the `dstRectToUpdate` is expressed in the parent's frame.
        utils::Boxf srcRect = convertToLocal(dstRectToUpdate, localDst);

        // Convert both the source and destination areas to engine format.
        utils::Boxf srcRectEngine = convertToEngineFormat(srcRect, utils::Boxf::fromSize(sizeText, true));
        utils::Boxf dstRectEngine = convertToEngineFormat(dstRectToUpdate, env);

        // Draw the `uuid` onto the `canvas` at last.
        getEngine().drawTexture(uuid, &srcRectEngine, &canvas, &dstRectEngine);
      }
    }

  }
}
