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

        TextBox(const std::string& name,
                SdlWidget* parent = nullptr,
                const utils::Sizef& area = utils::Sizef());

        virtual ~TextBox();

      protected:

        /**
         * @brief - Reimplementation of the base `SdlWidget` method in order to provide
         *          custom behavior when this widget receives focus.
         *          We set the policy to only accept click focus upon building `this`
         *          widget but it's only part of the job: indeed when the user clicks
         *          on the widget we want to make a cursor appear at the end of the text
         *          currently displayed in the text box. This cursor will indicate to
         *          the user that the text box is ready to accept focus.
         *          This method returns `true` or `false` based on whether the input
         *          focus has been handled or not. The return value for this class is
         *          identical to the value provided by the base class.
         * @param reason - the focus reason which triggered the update of the state in
         *                 the first place.
         * @param gainedFocus - `true` if this widget just gained focus, `false` if it
         *                      just lost the focus.
         * @return - `true` if the base texture role has been updated, `false` otherwise.
         */
        bool
        updateStateFromFocus(const core::engine::FocusEvent::Reason& reason,
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

      private:
    };

    using TextBoxShPtr = std::shared_ptr<TextBox>;
  }
}

# include "TextBox.hxx"

#endif    /* TEXT_BOX_HH */
