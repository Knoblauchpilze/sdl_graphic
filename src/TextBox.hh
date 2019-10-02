#ifndef    TEXT_BOX_HH
# define   TEXT_BOX_HH

# include <memory>
# include <string>
# include <core_utils/Uuid.hh>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class TextBox: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a new textbox with the specified properties. The user can
         *          specify the name of this textbox along with the initial text that
         *          will be displayed.
         * @param name - the name of `this` textbox.
         * @param font - the name of the font to use to render the text.
         * @param text - the initial content of the textbox.
         * @param size - the size of the font to use to render the text.
         * @param parent - a pointer to the parent widget for this textbox.
         * @param area - the size hint for this text box.
         */
        TextBox(const std::string& name,
                const std::string& font,
                const std::string& text = std::string(),
                const unsigned& size = 15,
                SdlWidget* parent = nullptr,
                const utils::Sizef& area = utils::Sizef());

        virtual ~TextBox();

      protected:

        /**
         * @brief - Specialization of the base `SdlWidget` method in order to handle keyboard focus
         *          update. Compared to the base method this class only adds the update of the cursor
         *          display.
         * @param e - the keyboard focus event to process.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        keyboardGrabbedEvent(const core::engine::Event& e) override;

        /**
         * @brief - Specialization of the base `SdlWidget` method in order to handle keyboard focus
         *          update. Compared to the base method this class only adds the update of the cursor
         *          display.
         * @param e - the keyboard focus event to process.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        keyboardReleasedEvent(const core::engine::Event& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide
         *          specific behavior upon detecting a key event. We want to update
         *          the text displayed in `this` textbox with the pressed key. We
         *          only react to key release event which indicates that the user
         *          really entered the key.
         *          The update of the internal text only happens when `this` textbox
         *          is selected (i.e. has the focus).
         * @param e - the key event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        keyPressEvent(const core::engine::KeyEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` in order to provide
         *          custom behavior when the user clicks on the textbox. We want to
         *          move the cursor to a location between two characters close to
         *          where the click occurred.
         * @param e - the event to be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide specific
         *          behavior when the user double click on a word of the textbox. It should
         *          select this word entirely.
         * @param e - the double click event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDoubleClickEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Specialization of the base `EngineObject` method to provide custom behavior
         *          upon dragging the mouse. We will handle the progressive selection of the
         *          text whenever the mouse hovers over the text.
         *          Note that the cursor will also be dragged along.
         * @param e - the drag event to process.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. A texture representing
         *          the text associated to this textbox along with a visual representation of
         *          the cursor indicating the current character being edited should be drawn
         *          on the provided canvas. Only the specified part is updated by this function.
         * @param uuid - the identifier of the canvas which we can use to draw a text overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

      private:

        /**
         * @brief - Used to specify the direction of a cursor motion. The directions allow to
         *          move the position of the cursor in the text displayed in the textbox.
         */
        enum class CursorMotion {
          Left,  //<!- A motion of the cursor on the left of its current position.
          Right  //<!- A motion of the cursor on the right of its current position.
        };

        /**
         * @brief - Used internally upon constructing the text box to initialize internal
         *          states.
         */
        void
        build();

        /**
         * @brief - Used to determine whether the input key can trigger a cursor motion inside
         *          the textbox. This is basically used to pin point keys which can update the
         *          position of the cursor in the internal text.
         * @param k - the key which should be checked for cursor motion trigger.
         * @return - `true` if the input key triggers a cursor position update and `false` in
         *           any other case.
         */
        bool
        canTriggerCursorMotion(const core::engine::Key& k) const noexcept;

        /**
         * @brief - Used to trigger the needed events and internal states so that the cursor
         *          indicating the position where the user is entering text is made visible
         *          or hidden based on the value of the input boolean. This boolean is set to
         *          be `true` if the cursor should be made visible and `false` otherwise.
         * @param visible - `true` if the cursor should be made visible, `false` otherwise.
         */
        void
        updateCursorState(const bool visible);

        /**
         * @brief - Used to update the position of the cursor given the specified motion. This
         *          function does not check that the cursor is actually visible before performing
         *          the motion.
         *          The cursor is advanced by one step in the correct direction if possible. If
         *          no motion in the specified direction is possible nothing happens.
         *          The `fastForward` attribute defines whether the motiion should fastforward
         *          until no more motion of this type is possible. Typically it indicates in the
         *          case of a left motion for example that the new position of the cursor should
         *          be set to the beginning of the text.
         * @param motion - the direction into which the cursor should be moved.
         * @param fastForward - `true` if the motion should be applied until it can't be applied
         *                      anymore and `false` otherwise.
         */
        void
        updateCursorPosition(const CursorMotion& motion,
                             bool fastForward = false);

        /**
         * @brief - Used to update the position of the cursor to the value specified in argument.
         *          The needed flag will be set in order to repaint the textbox if needed. Note
         *          that some checks are performed to verify that the input position is valid,
         *          which means that it is somewhat safe to call this method without be too strict
         *          on the input argument.
         * @param pos - the position to assign to the cursor.
         */
        void
        updateCursorToPosition(const unsigned pos);

        /**
         * @brief - Add the specified character to the internal text at the position specified
         *          according to the cursor position.
         *          Triggers a repaint upon completing the operation.
         * @param c - the character to add to the internal text.
         */
        void
        addCharToText(const char c);

        /**
         * @brief - Remove a character from the internal text according to the position pointed
         *          by the cursor index. Note that if no character can be removed nothing happens.
         *          Note that a repaint event is triggered only if the removal of the character
         *          succeeds.
         *          The input boolean allows to determine whether the character to remove should
         *          be removed from the front of the cursor or behind it.
         * @param forward - `true` if the character should be removed in front of the cursor's
         *                  position and `false` if the character should be removed behind of the
         *                  cursor's position.
         */
        void
        removeCharFromText(bool forward);

        /**
         * @brief - Used to start a selection from the internal position of the cursor. This will
         *          set the `m_selectionStarted` boolean to `true` and register the current cursor's
         *          position in order to correctly select the part of the text which needs
         *          selection.
         */
        void
        startSelection() noexcept;

        /**
         * @brief - Used to stop the selection of a text in this box. Note that this will invalidate
         *          the cache for this box as we most likely have some text which should be rendered
         *          in normal display again.
         *          If the selection was not started nothing happens. Also note that a repaint event
         *          is requested only if the selection contained at least one character.
         */
        void
        stopSelection() noexcept;

        /**
         * @brief - Used to perform the loading of the font to use to render the text.
         */
        void
        loadFont();

        /**
         * @brief - Used to perform the loading of the text into the various textures used to
         *          represent the text. This method handles the loading of the font if needed
         *          and the recreation of the texture representing the text if needed.
         */
        void
        loadText();

        /**
         * @brief - Used to perform the loading of the cursor into the `m_cursor` texture. This
         *          method handles the loading of the font if needed even though we should hardly
         *          ever need it in this function because the `loadText` method should handle it.
         */
        void
        loadCursor();

        /**
         * @brief - Destroys the texture contained in the various textures used to represent the
         *          text in thie box. Invalidate each one of them when the clearing is done.
         *          Should typically be used when recreating the text after a modification of
         *          the rendering mode.
         */
        void
        clearText();

        /**
         * @brief - Destroys the texture contained in the `m_cursor` and invalidates it.
         */
        void
        clearCursor();

        /**
         * @brief - Used to determine whether the cursor is visible. This is a convenience wrapper
         *          around the internal `m_cursorVisible` attribute to allow for easy modification
         *          of the behavior.
         * @return - `true` if the cursor is visible and `false` otherwise.
         */
        bool
        isCursorVisible() const noexcept;

        /**
         * *@brief - Used to determine whether the user is currently performing a text selection
         *           operation.
         * @return - `true` if a text selection operation is being performed and `false` otherwise.
         */
        bool
        selectionStarted() const noexcept;

        /**
         * @brief - Used to determine whether a left text part is active for this textbox. We
         *          check whether the cursor's position and the selection start allow for a
         *          part of the internal text to remain visible.
         *          Note that this does not tell whether the cursor is visible or not.
         * @return - `true` if a left text part exist for this box.
         */
        bool
        hasLeftTextPart() const noexcept;

        /**
         * @brief - Used to retrieve a string describing the content of the left part of the
         *          text based on the internal cursor's position and text.
         *          Note that if `hasLeftTextPart` returns `false` the returned string will
         *          be empty.
         * @return - a string representing the part of the internal `m_text` which is assigned
         *           to the left part of the text. Can be empty.
         */
        std::string
        getLeftText() const noexcept;

        /**
         * @brief - Used to determine whether a selected text part is active for this textbox. We
         *           check whether the selection is started and if the cursor's position has been
         *           changed to produce a valid interval between the value `m_cursorIndex` and the
         *           value `m_selectionStart`.
         * @return - `true` if a selected text part exist for this box.
         */
        bool
        hasSelectedTextPart() const noexcept;

        /**
         * @brief - Used to retrieve a string describing the content of the selected part the text
         *          based on the internal cursor's position and text.
         *          Note that if `hasSelectedTextPart` returns `false` the returned string will
         *          be empty.
         * @return - a string representing the part of the internal `m_text` which is assigned
         *           to the selected part of the text. Can be empty.
         */
        std::string
        getSelectedText() const noexcept;

        /**
         * @brief - Used to determine whether a right text part is active for this textbox. We
         *          check whether the cursor's position and the selection start allow for a
         *          part of the internal text to remain visible.
         *          Note that this does not tell whether the cursor is visible or not.
         * @return - `true` if a right text part exist for this box.
         */
        bool
        hasRightTextPart() const noexcept;

        /**
         * @brief - Used to retrieve a string describing the content of the right part the text
         *          based on the internal cursor's position and text.
         *          Note that if `hasRightTextPart` returns `false` the returned string will
         *          be empty.
         * @return - a string representing the part of the internal `m_text` which is assigned
         *           to the right part of the text. Can be empty.
         */
        std::string
        getRightText() const noexcept;

        /**
         * @brief - Used to determine the index of the character that is closest to the input
         *          position. The position is assumed to be expressed in local coordinate frame
         *          and the index returned can be assigned to the `m_cursorIndex` without
         *          problems.
         * @param pos - a vector representing a position expressed in local coordinate frame.
         * @return - a value representing the index to assign to the cursor so that it is located
         *           closest to the input position.
         */
        unsigned
        closestCharacterFrom(const utils::Vector2f& pos) const noexcept;

        /**
         * @brief - Used to determine whether any of the rendering properties of the text has
         *          been modified since the last `drawContentPrivate` operation.
         *          Internally uses the `m_textChanged` status to perform the check.
         *          Assumes that the `m_propsLocker` is already locked.
         * @return - `true` if at least one of the rendering properties have been modified and
         *           `false` otherwise.
         */
        bool
        textChanged() const noexcept;

        /**
         * @brief - Marks this widget for a rebuild of the text upon calling the `drawContentPrivate`
         *          method. Assumes that the `m_propsLocker` is already locked.
         */
        void
        setTextChanged() noexcept;

        /**
         * @brief - Used to determine whether the cursor has been modified since the last call to the
         *          `drawContentPrivate` operation. Internally uses the `m_cursorChanged` status 
         *          perform the check.
         *          Assumes that the `m_propsLocker` is already locked.
         * @return - `true` if the cursor has been modified and `false` otherwise.
         */
        bool
        cursorChanged() const noexcept;

        /**
         * @brief - Marks this widget for a rebuild of the text upon calling the `drawContentPrivate`
         *          method. Assumes that the `m_propsLocker` is already locked.
         */
        void
        setCursorChanged() noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the left part of the text
         *          displayed in this text box. The left part is most of the times on the left most
         *          part of the widget except when the cursor is set to be before the first letter
         *          of the text or if a selection makes this part inexistant. In which case though
         *          the left part of the text should be invalid and this function should not be
         *          called anyways.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         *          We assume that a left text texture is available when calling this method.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the left part of the text and its
         *           position on the parent area.
         */
        utils::Boxf
        computeLeftTextPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the part of the text which
         *          is selected for modifications. This part of the text is usually displayed with
         *          a different background and color. In order to be displayed we obviously need
         *          to have a valid selection started.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         *          We assume that a selected text exist when calling this function.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the selected part of the text and its
         *           position on the parent area.
         */
        utils::Boxf
        computeSelectedTextPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Function very similar to the `computeSelectedTextPosition` except it is used to
         *          determine a valid position of the background to associate to the selected text
         *          so that it is visible.
         *          The assumptions made upon calling this function are equivalent to the ones made
         *          when calling `computeSelectedTextPosition`.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the selection background and its
         *           position on the parent area.
         */
        utils::Boxf
        computeSelectedBackgroundPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the cursor displayed to help
         *          determine where the edition is being made in this textbox. Note that the cursor
         *          is only displayed when this widget has the keyboard focus.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         *          Note that the cursor's texture is assumed to be valid when calling this method
         *          but no checks are performed to verify that it is visible.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the cursor and its position on the parent
         *           area.
         */
        utils::Boxf
        computeCursorPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the right part of the text
         *          displayed in this text box. The right part comes after the left part and usually
         *          after the cursor and selection. The only exception is when the cursor is set to
         *          be displayed after the last character. In which case though the right part of
         *          the text should be invalid and this function should not be called anyways.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         *          Note that the right text's texture is assumed to be valid when calling this
         *          function.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the right part of the text and its
         *           position on the parent area.
         */
        utils::Boxf
        computeRightTextPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to perform the drawing of the portion of the texture described by `uuid` to
         *          the specified `canvas`. In order to determine which portion of the `uuid` should
         *          be repainted we use the `toUpdate` which represents an area expressed in parent's
         *          coordinate frame of the area to update. The `localDst` represents the area in
         *          parent's coordinate frame covered by the `uuid` texture.
         *          By computing the intersection of both this method is able to derive and repaint
         *          only the relevant part of the `uuid` (if any) to the provided `canvas` texture.
         * @param uuid - The texture to repaint. We assume that this texture is valid.
         * @param localDst - the area covered by the `uuid` texture in parent coordinate frame.
         * @param canvas - the identifier of the texture onto which the `uuid` should be repainted.
         * @param env - a description of the size of the environment (i.e. the `canvas` texture) which
         *              is used to perform conversion of various areas in engine format.
         * @param toUpdate - an area representing the area to update in parent's coordinate frame.
         */
        void
        drawPartOnCanvas(const utils::Uuid& uuid,
                         const utils::Boxf& localDst,
                         const utils::Uuid& canvas,
                         const utils::Boxf& env,
                         const utils::Boxf& toUpdate);

      private:

        /**
         * @brief - The text to display in this textbox. The text is rendered according to the
         *          internal rendering properties and displayed on the area available for this
         *          widget. By default the text is always displayed starting from the left of
         *          the textbox.
         *          A modification of any of the properties or of the text itself triggers a
         *          repaint operation which will recreate the text's texture. Such operations
         *          are thus to be handled with care.
         */
        std::string m_text;

        /**
         * @brief - Indicates the current position of the cursor. The position indicates the
         *          first index which is right to the cursor. For example if this value is `0`
         *          it indicates that the cursor is at the left of the first character of the
         *          string representing the text of this textbox.
         *          The valid range is thus `[0; m_text.size()]`.
         */
        unsigned m_cursorIndex;

        /**
         * @brief - Indicates whether the cursor is visible. We display the cursor only when
         *          this textbox has keyboard focus, which is likely to indicate that the user
         *          is ready to make some modifications on the internal text. The cursor is a
         *          visual aid to indicate which character will be affected by the next update
         *          to the textbox.
         */
        bool m_cursorVisible;

        /**
         * @brief - Used to determine whether the texture cached in `m_cursor` is valid and can
         *          be reused as is or if it should be recreated. Typical case where this value
         *          is set to `true` is when the cursor becomes visible or when its color should
         *          be updated as a result of a selection operation.
         */
        bool m_cursorChanged;

        /**
         * @brief - Describes the starting index of the selected text. Basically we consider that
         *          the text ocated between the `m_cursorIndex` and the `m_selectionStart` is to
         *          be rendered as selected text. Note that depending on the actions of the user
         *          after starting the selection the `m_selectionStart` can either be larger or
         *          smaller than the `m_cursorIndex`.
         *          The value contained in this attribute is only relevant if the boolean named
         *          `m_selectionStarted` is `true`.
         */
        unsigned m_selectionStart;

        /**
         * @brief - This value is `true` whenever a text selection operation is running and `false`
         *          otherwise.
         */
        bool m_selectionStarted;

        /**
         * @brief - Information about the font to use to render the text. We use the `m_fontName`
         *          and `m_fontSize` to store information while the font is not loaded yet. The
         *          `m_font` itself holds an identifier returned by the engine which allows to
         *          access to the font's data through the engine.
         *          The information contained in these arguments may mismatch if the `m_textChanged`
         *          boolean is set to `true`. Upon the next repaint operation it should be corrected
         *          and if `m_textChanged` is `false` all these attributes should represent the
         *          same font.
         */
        std::string m_fontName;
        unsigned m_fontSize;
        utils::Uuid m_font;

        /**
         * @brief - Describes the role of the text's texture to use. Various roles usually implies
         *          different colors. This role is used so that the highlight operation of the base
         *          widget's content also updates the color of the text so that we can try to always
         *          display a highly visible text upon the background.
         */
        core::engine::Palette::ColorRole m_textRole;

        /**
         * @brief - Used to perform some caching of the data for this textbox. As long as the value
         *          of `m_textChanged` is set to `false` the content of `m_textTex` is considered
         *          valid and is drawn as is upon each `drawContentPrivate` operation.
         *          As soon as this value is set to `true` the information contained in the various
         *          engine-managed fields of the object (such as `m_textTex` or `m_font`) may not be
         *          up-to-date with the content of the other attributes.
         *          This is corrected upon calling the `drawContentPrivate` method.
         */
        bool m_textChanged;

        /**
         * @brief - Used to perform some caching of the data for this textbox. In order to account for
         *          the possible positions of the cursor when editing the text of the text box we need
         *          four separate textures to represent the text:
         *            - a texture to represent the text on the left of the cursor
         *            - a texture to represent the cursor itself
         *            - a texture to represent the text on the right of the cursor
         *            - a texture to represent the text currently selected
         * Not all of them are always valid, and the caching only holds until a character is added to
         * the text of this box or until the cursor is moved.
         * These values are also invalidated whenever the font or the text size is modified.
         * Note that unless the `m_textChanged` is set to `true` the content of these textures can be
         * used without modifications.
         * A general rule of thumb is that on screen the various textures are represented in an order
         * as follows:
         *  - the left part (`m_leftText`).
         *  - the cursor (`m_cursor`) or the selected text (`m_selectedText`).
         *  - the right part (`m_rightText`).
         */
        utils::Uuid m_leftText;
        utils::Uuid m_cursor;
        utils::Uuid m_selectedText;
        utils::Uuid m_rightText;

        /**
         * @brief - Used to handle a darker area behind the selected text so that it stands out from
         *          regular text.
         *          This texture is updated upon modifying the selected text and is usually only used
         *          in combination with the selection mechanism.
         */
        utils::Uuid m_selectionBackground;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this textbox.
         */
        std::mutex m_propsLocker;
    };

    using TextBoxShPtr = std::shared_ptr<TextBox>;
  }
}

# include "TextBox.hxx"

#endif    /* TEXT_BOX_HH */
