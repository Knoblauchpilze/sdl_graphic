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
         * @brief - Reimplementation of the base `SdlWidget` method in order to provide
         *          custom behavior when this widget receives focus.
         *          We set the policy to only accept click focus upon building `this`
         *          widget but it's only part of the job: indeed when the user clicks
         *          on the widget we want to make a cursor appear at the end of the text
         *          currently displayed in the text box. This cursor will indicate to
         *          the user that the text box is ready to accept focus.
         * @param reason - the focus reason which triggered the state update.
         * @param gainedFocus - `true` if `this` widget just gained focus, `false` if it
         *                      lost the focus.
         */
        void
        stateUpdatedFromFocus(const core::FocusState& state,
                              const bool gainedFocus) override;

      private:

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
         * @brief - Used to performt he loading of the text into the `m_textTex` texture. This
         *          method handles the loading of the font if needed and the recreation of the
         *          texture representing the text if needed.
         */
        void
        loadText() const;

        /**
         * @brief - Destroys the texture contained in the `m_label` identifier if it is valid
         *          and invalidate it.
         *          Should typically be used when recreating the text after a modification of
         *          the rendering mode.
         */
        void
        clearText() const;

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
        setTextChanged() const noexcept;

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
        mutable utils::Uuid m_font;

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
        mutable bool m_textChanged;

        /**
         * @brief - An identifier provided by the engine and representing the texture containing the
         *          rendered text for this textbox. As long as the `m_textChanged` boolean is set to
         *          `false` this value can be cached and used as is.
         */
        mutable utils::Uuid m_textTex;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this textbox.
         */
        mutable std::mutex m_propsLocker;
    };

    using TextBoxShPtr = std::shared_ptr<TextBox>;
  }
}

# include "TextBox.hxx"

#endif    /* TEXT_BOX_HH */
