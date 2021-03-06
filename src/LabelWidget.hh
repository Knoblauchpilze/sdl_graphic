#ifndef    LABELWIDGET_HH
# define   LABELWIDGET_HH

# include <memory>
# include <string>
# include <core_utils/Uuid.hh>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class LabelWidget: public core::SdlWidget {
      public:

        enum class VerticalAlignment {
          Top,
          Center,
          Bottom
        };

        enum class HorizontalAlignment {
          Left,
          Center,
          Right
        };

      public:

        LabelWidget(const std::string& name,
                    const std::string& text,
                    const std::string& font,
                    unsigned size = 15,
                    const HorizontalAlignment& hAlignment = HorizontalAlignment::Center,
                    const VerticalAlignment& vAlignment = VerticalAlignment::Center,
                    SdlWidget* parent = nullptr,
                    const core::engine::Color& color = core::engine::Color(),
                    const utils::Sizef& area = utils::Sizef());

        virtual ~LabelWidget();

        void
        setText(const std::string& text) noexcept;

        void
        setHorizontalAlignment(const HorizontalAlignment& alignment) noexcept;

        void
        setVerticalAlignment(const VerticalAlignment& alignment) noexcept;

      protected:

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. A texture representing
         *          the text associated to this label widget will be drawn on the provided
         *          canvas. Only the specified part is updated by this function.
         * @param uuid - the identifier of the canvas which we can use to draw a text overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method to handle the highlight
         *          of the text associated to this label. Performs the same mofification on
         *          the widget's content as the base handler in addition to update text's
         *          role.
         * @param state - the current state of this widget.
         * @param gainedFocus - `true` if `this` widget just gained focus, `false` if it
         *                      lost the focus.
         */
        void
        stateUpdatedFromFocus(const core::FocusState& state,
                              bool gainedFocus) override;

      private:

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
         * @brief - Used to update the internal value of the `m_textRole` value based on the role
         *          of the base widget's content provided as argument.
         *          Basically tries to assign a valid role corresponding to the background into
         *          which the text will be displayed.
         *          Assumes that the `m_propsLocker` is already locked.
         *          Note that nothing happen if the input identifier is not valid.
         * @param base - an identifier corresponding to the base texture onto which the text will
         *               be displayed.
         */
        void
        updateTextRole(const utils::Uuid& base);

      private:

        /**
         * @brief - The text to display in this label widget. The text is rendered according to
         *          the provided option and displayed on the area available for this widget based
         *          on the alignments defined by the user.
         *          A modification of any of the properties or of the text itself triggers a
         *          repaint operation which will recreate the text's texture. Such operations are
         *          thus to be handled with care.
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
        utils::Uuid m_font;

        /**
         * @brief - Describes the alignment of the text in the space available for this widget. Note
         *          that the alignment tries to apply the position of the text but depending on the
         *          size of the widget itself some alignment might not look very different.
         *          The user can specify both the horizontal and vertical alignment so that there is
         *          full control on the position of the text.
         */
        HorizontalAlignment m_hAlignment;
        VerticalAlignment m_vAlignment;

        /**
         * @brief - Describes the role of the text's texture to use. Various roles usually implies
         *          different colors. This role is used so that the highlight operation of the base
         *          widget's content also updates the color of the text so that we can try to always
         *          display a highly visible text upon the background.
         */
        core::engine::Palette::ColorRole m_textRole;

        /**
         * @brief - Used to perform some caching of the data for this label widget. As long as the
         *          `m_textChanged` is set to `false` the content of `m_label` is considered valid
         *          and is drawn as is upon each `drawContentPrivate` operation.
         *          As soon as this value is set to `true` the information contained in the various
         *          engine-managed fields of the object (such as `m_label` or `m_font`) may not be
         *          up-to-date with the content of the other attributes.
         *          This is corrected upon calling the `drawContentPrivate` method.
         */
        bool m_textChanged;

        /**
         * @brief - An identifier provided by the engine and representing the texture containing the
         *          rendered text for this label. As long as the `m_textChanged` boolean is set to
         *          `false` this value can be cached and used as is.
         */
        utils::Uuid m_label;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this label widget.
         */
        std::mutex m_propsLocker;
    };

    using LabelWidgetShPtr = std::shared_ptr<LabelWidget>;
  }
}

# include "LabelWidget.hxx"

#endif    /* LABELWIDGET_HH */
