#ifndef    CHECKBOX_HH
# define   CHECKBOX_HH

# include <mutex>
# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>
# include "VirtualLayoutItem.hh"

namespace sdl {
  namespace graphic {

    class Checkbox: public core::SdlWidget {
      public:

        Checkbox(const std::string& name,
                 const std::string& text,
                 const std::string& font,
                 bool checked = false,
                 unsigned size = 15,
                 core::SdlWidget* parent = nullptr,
                 const utils::Sizef& area = utils::Sizef(),
                 const core::engine::Color& color = core::engine::Color::NamedColor::Silver);

        virtual ~Checkbox();

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` in order to produce a
         *          similar behavior to what is provided by the button class. As we want
         *          the checkbox to appear as if made from a single element we need to
         *          hijack the regular behavior to substitue this item to any of its
         *          children being picked for event.
         * @param pos - a vector describing the position which should be spanned by the
         *              items.
         * @return - a valid pointer if any of the children items spans the input position
         *           and `null` otherwise.
         */
        const core::SdlWidget*
        getItemAt(const utils::Vector2f& pos) const noexcept override;

        /**
         * @brief - Allow to check whether the checkbox is toggled.
         * @return - `true` if the checkbox is toggled and `false` otherwise.
         */
        bool
        toggled();

        /**
         * @brief - Used to switch the state of this checkbox to be toggled or not based on the
         *          value of the input boolean. Note that this might mess up the events based
         *          handling of the state so use with care.
         * @param toggled - `true` if the checkbox state should be set to `Toggled` and `false`
         *                  otherwise.
         */
        void
        toggle(bool toggled);

      protected:

        /**
         * @brief - Reimplementation of the base class method to provide update of the
         *          selection box itself when a resize is requested.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. This allows to draw
         *          the actual selection box to represent the checkbox.
         * @param uuid - the identifier of the canvas which we can use to draw the overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

        /**
         * @brief - Reimplementation of the base class method to handle cases where the
         *          mouse is dragged to another widget which would prevent the checkbox
         *          to correctly be un/selected.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        dropEvent(const core::engine::DropEvent& e) override;

        /**
         * @brief - Reimplementation of the base class method to provide update of the
         *          selection box state when the user releases the correct mouse button.
         * @param e - the event to be interpreted.
         * @return - `true` if the event was recognized and `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

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
         * @brief - Used to retrieve the border's dimensions for this checkbox.
         * @return - a value representing the borders' dimensions.
         */
        static
        float
        getBorderDims() noexcept;

        /**
         * @brief - Used to retrieve the maximum size available for the selection box to use
         *          to check this item.
         * @return - a size describing the selection box' maximum dimensions.
         */
        static
        utils::Sizef
        getSelectionBoxMaxDims() noexcept;

        /**
         * @brief - Determine a color that contrasts well with the provided color. The
         *          input color is typically assumed to refer to the general background
         *          of the checkbox and the color to be returned to the background color
         *          of the selection box. It can also be used to find a color for the
         *          selection's item to contrast correctly with the selection box' color.
         * @param c - a reference color for which we should find a well contrasted color.
         * @return - a color that contrasts well with the input color.
         */
        static
        core::engine::Color
        getContrastedColorFromRef(const core::engine::Color& c) noexcept;

        /**
         * @brief - Retrieve a size object which is slightly smaller than the input size and
         *          that is suited to represent the darker area representing the toggled part
         *          of the selection box.
         *          Having this area a little smaller makes for a nicer visual result. Note
         *          that this component has a fixed size decrement from the size of the input
         *          `rectangle` but is also clamped so that it don't get too small (i.e. with
         *          a null size).
         * @param rectangle - the total size of the selection box.
         * @return - a size representing the size of the toggling element of this box.
         */
        static
        utils::Sizef
        getTogglingElementSize(const utils::Sizef& rectangle) noexcept;

        /**
         * @brief - Retrieves the default mouse button for this checkbox to be toggled.
         * @return - a default button for the click on this checkbox.
         */
        static
        core::engine::mouse::Button
        getClickButton() noexcept;

        /**
         * @brief - Used to create the layout needed to represent this checkbox.
         * @param text - the text to display for this checkbox.
         */
        void
        build(const TextData& text);

        /**
         * @brief - Describes whether the selection box should be repainted.
         *          Note that the locker is assumed to already be acquired.
         * @return - `true` if the selection box should be repainted and `false`
         *           otherwise.
         */
        bool
        boxChanged() const noexcept;

        /**
         * @brief - Defines that the selection box should be repainted. Also triggers
         *          a call to the repaint method from the parent class.
         *          Note that the locker should already be assumed before using this
         *          method.
         */
        void
        setBoxChanged();

        /**
         * @brief - Used to perform the loading of the selection box to be able to correctly
         *          render it.
         *          Note that the locker is assumed to already be acquired.
         */
        void
        loadBox();

        /**
         * @brief - Clears the textures representing the selection box of this item.
         */
        void
        clearBox();

      private:

        /**
         * @brief - Protects concurrent accesses to the properties of this checkbox.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Describes whether the selection should be recomputed or can be
         *          used as is.
         */
        bool m_boxChanged;

        /**
         * @brief - Holds whether this checkbox is ticked or not.
         */
        bool m_toggled;

        /**
         * @brief - A virtual layout item which allows to position the box as if it
         *          was a regular item. This will help managing its size and also
         *          offset the `text` by the correct position to fit the selection
         *          box.
         */
        VirtualLayoutItemShPtr m_boxItem;

        /**
         * @brief - The selection box' data for this item. Used to represent the box
         *          for the selection of the checkbox. This item will receive the
         *          selection's item when the box is toggled.
         */
        utils::Uuid m_emptyBox;

        /**
         * @brief - The selection's item data for this item. Used only when the checkbox
         *          is toggled.
         */
        utils::Uuid m_selectionItem;
    };

    using CheckboxShPtr = std::shared_ptr<Checkbox>;
  }
}

# include "Checkbox.hxx"

#endif    /* CHECKBOX_HH */
