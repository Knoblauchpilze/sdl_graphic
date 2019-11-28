#ifndef    BUTTON_HH
# define   BUTTON_HH

# include <mutex>
# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class Button: public core::SdlWidget {
      public:

        Button(const std::string& name,
               const std::string& text,
               const std::string& icon,
               const std::string& font,
               unsigned size = 15,
               core::SdlWidget* parent = nullptr,
               const utils::Sizef& area = utils::Sizef());

        virtual ~Button();

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` in order to filter the
         *          returned widget if the position lies inside this button.
         *          Indeed even though the button uses some children to display its content
         *          we want to make it behave as if it was a single element.
         *          To do so we hijack the returned element if it corresponds to an ancestor
         *          of this element.
         * @param pos - a vector describing the position which should be spanned by the
         *              items.
         * @return - a valid pointer if any of the children items spans the input position
         *           and `null` otherwise.
         */
        const core::SdlWidget*
        getItemAt(const utils::Vector2f& pos) const noexcept override;

      protected:

        /**
         * @brief - Reimplementation of the base class method to provide update of the borders
         *          when a resize is requested.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. This allows to draw some
         *          sort of border for this button to make it resemble to a button.
         * @param uuid - the identifier of the canvas which we can use to draw a text overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base class method to provide update of the borders
         *          when the user push on this button.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonPressEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base class method to provide update of the borders
         *          when the user release on this button.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method to handle the highlight
         *          of this element. We want to progpagate the information to the children
         *          so that we keep a consistent state among all the button.
         * @param state - the current state of this widget.
         * @param gainedFocus - `true` if `this` widget just gained focus, `false` if it
         *                      lost the focus.
         */
        void
        stateUpdatedFromFocus(const core::FocusState& state,
                              bool gainedFocus) override;

      private:

        /**
         * @brief- Convenience structure describing the information to pass on to create the
         *         label widget to display the text of the button.
         */
        struct TextData {
          std::string title;
          std::string font;
          unsigned size;
        };

        /**
         * @brief - Used to retrieve the border's dimensions for this button.
         * @return - a value representing the borders' dimensions.
         */
        static
        float
        getBorderDims() noexcept;

        /**
         * @brief - Used to retrieve the maximum size available for an icon in a button. This
         *          size is assigned when creating the icon for any button.
         * @return - a size describing the icon maximum dimensions.
         */
        static
        utils::Sizef
        getIconMaxDims() noexcept;

        /**
         * @brief - Retrieves the default button for this button to be pushed.
         * @return - a default button for the click on this button.
         */
        static
        core::engine::mouse::Button
        getClickButton() noexcept;

        /**
         * @brief - Retrieves the color role to use to display borders for their first possible
         *          role. Indeed borders in button can have one of two roles based on whether
         *          the button is pushed.
         * @return - the first color role to make the borders' colors alternate.
         */
        static
        core::engine::Palette::ColorRole
        getBorderColorRole() noexcept;

        /**
         * @brief - Similar method as `getBorderColorRole` but returns the second possible color
         *          role.
         * @return - the second color role to make the borders' colors alternate.
         */
        static
        core::engine::Palette::ColorRole
        getBorderAlternateColorRole() noexcept;

        /**
         * @brief - Used to create the layout needed to represent this button.
         * @param icon - the icon to use for this button.
         * @param text - the text to display for this button.
         */
        void
        build(const std::string& icon,
              const TextData& text);

        /**
         * @brief - Describes whether the borders should be repainted.
         *          Note that the locker is assumed to already be acquired.
         * @return - `true` if the borders should be repainted and `false` otherwise.
         */
        bool
        bordersChanged() const noexcept;

        /**
         * @brief - Defines that the borders should be repainted. Also triggers a call
         *          to the repaint method from the parent class.
         *          Note that the locker should already be assumed before using this
         *          method.
         */
        void
        setBordersChanged();

        /**
         * @brief - Used to perform the loading of the borders to update the internal attributes.
         *          Note that the locker is assumed to already be acquired. The textures are not
         *          checked to determine whether we actually need a repaint.
         */
        void
        loadBorders();

        /**
         * @brief - Clears the textures representing the border of this button.
         */
        void
        clearBorders();

      private:

        /**
         * @brief - Convenience structure describing the internal properties to use to represent
         *          the borders for this button.
         */
        struct BordersData {
          utils::Uuid hLightBorder;
          utils::Uuid hDarkBorder;
          utils::Uuid vLightBorder;
          utils::Uuid vDarkBorder;

          bool pressed;
        };

        /**
         * @brief - Protects concurrent accesses to the properties of this button.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Describes whether the borders should be recomputed or can be
         *          used as is.
         */
        bool m_bordersChanged;

        /**
         * @brief - The borders' data for this button.
         */
        BordersData m_borders;
    };

    using ButtonShPtr = std::shared_ptr<Button>;
  }
}

# include "Button.hxx"

#endif    /* BUTTON_HH */
