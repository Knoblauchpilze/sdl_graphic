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
        keyReleaseEvent(const core::engine::KeyEvent& e) override;

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
         * @param motion - the direction into which the cursor should be moved.
         */
        void
        updateCursorPosition(const CursorMotion& motion);

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
         */
        void
        removeCharFromText();

        /**
         * @brief - Used to performt he loading of the text into the `m_textTex` texture. This
         *          method handles the loading of the font if needed and the recreation of the
         *          texture representing the text if needed.
         */
        void
        loadText();

        /**
         * @brief - Destroys the texture contained in the `m_label` identifier if it is valid
         *          and invalidate it.
         *          Should typically be used when recreating the text after a modification of
         *          the rendering mode.
         */
        void
        clearText();

        /**
         * @brief - Used to determine whether the cursor is visible. This is a convenience wrapper
         *          around the internal `m_cursorVisible` attribute to allow for easy modification
         *          of the behavior.
         * @return - `true` if the cursor is visible and `false` otherwise.
         */
        bool
        isCursorVisible() const noexcept;

        /**
         * @bvrief - Used to determine whether a left text part is active for this textbox. We
         *           check whether the cursor index is set to be located before the first character
         *           of this textbox.
         *           Note that this does not tell whether the cursor is visible or not.
         * @return - `true` if the cursor index is set to at least after the first character of
         *           this textbox and `false` otherwise.
         */
        bool
        hasLeftTextPart() const noexcept;

        /**
         * @bvrief - Used to determine whether a right text part is active for this textbox. We
         *           check whether the cursor index is set to be located after the last character
         *           of this textbox.
         *           Note that this does not tell whether the cursor is visible or not.
         * @return - `true` if the cursor index is set to at least before the last character of
         *           this textbox and `false` otherwise.
         */
        bool
        hasRightTextPart() const noexcept;

        /**
         * @brief - Used to determine whether any of the endering properties of the text has
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
         * @brief - Used to compute the position in the parent area for the left part of the text
         *          displayed in this text box. The left part is most of the times on the left most
         *          part of the widget except when the cursor is set to be before the first letter
         *          of the text. In which case though the left part of the text should be invalid
         *          so it should not happen.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the left part of the text and its
         *           position on the parent area.
         */
        utils::Boxf
        computeLeftTextPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the cursor displayed to help
         *          determine where the edition is being made in this textbox. Note that the cursor
         *          is only displayed when this widget has the keyboard focus.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the cursor and its position on the parent
         *           area.
         */
        utils::Boxf
        computeCursorPosition(const utils::Sizef& env) const noexcept;

        /**
         * @brief - Used to compute the position in the parent area for the right part of the text
         *          displayed in this text box. The right part comes after the left part and usually
         *          after the cursor. The only exception is when the cursor is set to be displayed
         *          after the last character. In which case though the right part of the text should
         *          be invalid so it should not happen.
         *          In order to provide accurate computation of the position relatively to a parent
         *          area the user needs to provide a size indicating the available space on said
         *          parent area. The position will be returned as if centered in this parent space.
         * @param env - a description of the available space in the parent area.
         * @return - a box indicating both the dimensions of the right part of the text and its
         *           position on the parent area.
         */
        utils::Boxf
        computeRightTextPosition(const utils::Sizef& env) const noexcept;

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
         *          three separate textures to represent the text:
         *            - a texture to represent the text on the left of the cursor
         *            - a texture to represent the cursor itself
         *            - a texture to represent the text on the right of the cursor
         * Not all of them are always valid, and the caching only holds until a character is added to
         * the text of this box or until the cursor is moved.
         * These values are also invalidated whenever the font or the text size is modified.
         * Note that unless the `m_textChanged` is set to `true` the content of these textures can be
         * used without modifications.
         */
        utils::Uuid m_leftText;
        utils::Uuid m_cursor;
        utils::Uuid m_rightText;

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
