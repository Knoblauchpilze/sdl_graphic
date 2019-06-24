#ifndef    COMBO_BOX_HH
# define   COMBO_BOX_HH

# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class ComboBox: public core::SdlWidget {
      public:

        /**
         * @brief - Describes the insertion policy applied by this combobox. When the user selects
         *          the current item, a combobox might allow insertion based on the following policy.
         */
        enum class InsertPolicy {
          NoInsert             = 0, //<! - No item can be inserted into the combobox.
          InsertAtTop          = 1, //<! - Insertion is allowed and performed before the first element.
          InsertAtCurrent      = 2, //<! - Insertion is allowed and performed in place at the current element.
          InsertAtBottom       = 3, //<! - Insertion is allowed and performed after the last element.
          InsertAfterCurrent   = 4, //<! - Insertion is allowed and performed after the current element.
          InsertBeforeCurrent  = 5, //<! - Insertion is allowed and performed before the current element.
          InsertAlphabetically = 6  //<! - Insertion is allowed and performed alphabetically.
        };

        static
        std::string
        getNameFromPolicy(const InsertPolicy& policy) noexcept;

      public:

        ComboBox(const std::string& name,
                 const InsertPolicy& policy,
                 core::SdlWidget* parent = nullptr,
                 const utils::Sizef& area = utils::Sizef(),
                 const int maxVisibleItems = 10);

        virtual ~ComboBox();

        int
        getItemsCount() const noexcept;

        bool
        hasActiveItem() const noexcept;

        int
        getActiveItem() const noexcept;

        /**
         * @brief - Inserts the provided `text` in this combobox and assigned it the `icon` if any is
         *          provided. The index at which the `text` should be inserted is computed from the
         *          internal `InsertPolicy`.
         *          Note that if the `InsertPolicy` does not allow insertion, an error is raised.
         *          Also note that this widget will take ownership of the provided `icon` and proceed
         *          to remove it at most appropriate time.
         * @param text - the text to insert using a position derived from the insert policy.
         * @param icon - a string representing the path to reach the icon to associate to this `text`
         *               entry. Note that if the provided value is null (default) this parameter is
         *               ignored and no icon will be associated to the item.
         */
        void
        insertItem(const std::string& text,
                   const std::string& icon = std::string());

        /**
         * @brief - Inserts the provided `text` in this combobox at the specified `index`. If the
         *          `index` cannot be reached according to the registered items, the closest match
         *          is selected which means:
         *          1. If `index > getItemsCount()`, the item is inserted in last position.
         *          2. If `index < 0`, the item is inserted in first position.
         *          Note that this method does not respect the `InsertPolicy` defined for this
         *          widget in most cases. To insert automatically using the policy one should use
         *          `addItem` with no `index` provided instead.
         *          Also note that this widget will take ownership of the provided `icon` and proceed
         *          to remove it at most appropriate time.
         * @param index - the index at which the `text` should be inserted in this combobox.
         * @param text - the text to insert at the specified location.
         * @param icon - a string representing the path to reach the icon to associate to this `text`
         *               entry. Note that if the provided value is null (default) this parameter is
         *               ignored and no icon will be associated to the item.
         */
        void
        insertItem(const int index,
                   const std::string& text,
                   const std::string& icon = std::string());

        void
        removeItem(const int index);

      protected:

        /**
         * @brief - Reimplementation of the base `EngineObject` method which allows to react
         *          on the click of the mouse on this widget to select a new active item.
         * @param e - the mouse event describing the click.
         * @return - true if the event was recognized, false otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `LayoutItem` method to allow saving of the
         *          area to assign so that it can be used to open/close the combobox.
         * @param e - the resize event describing the resize operation.
         * @return - true if the event was recognized, false otherwise.
         */
        bool
        resizeEvent(const core::engine::ResizeEvent& e) override;

      private:

        /**
         * @brief - Enumeration describing the possible states for a combo box.
         */
        enum class State {
          Closed,
          Dropped
        };

        /**
         * @brief - Used to build this component by creating the adequate layout and the component
         *          to use to represent each item of the combobox.
         */
        void
        build();

        /**
         * @brief - Used to determine the index at which an item should be inserted. This method
         *          fills a pair as return value: the first element of the pair describes the index
         *          at which the item should be inserted, the second one indicates whether the
         *          item currently located at the position should be erased.
         *          Note that if the provided index is larger than the current items count, the
         *          second value can be ignored.
         *          Note that an error is raised if the policy does not allow insertion.
         * @param text - the text to insert in this combobox.
         * @return - a pair indicating both the index at which an item should be inserted and whether
         *           the current item at the provided index should be erased.
         */
        std::pair<int, bool>
        getIndexFromInsertPolicy(const std::string& text) const;

        /**
         * @brief - Assign a new active item to the combobox. The item is checked against internal
         *          data to determine whether it actually exists and the corresponding display is
         *          updated to reflect this new active item.
         * @param index - the index of the item to activate.
         */
        void
        setActiveItem(const int& index);

        /**
         * @brief - Returns true if this combobox is dropped (i.e. displays the available options)
         *          and false otherwise.
         * @return - true if the combobox is dropped, false otherwise.
         */
        bool
        isDropped() const noexcept;

        /**
         * @brief - Equivalent to `!isDropped()`. Returns true if the combobox is true and false
         *          otherwise.
         * @¶eturn - true if the combobox is closed, false otherwise.
         */
        bool
        isClosed() const noexcept;

        /**
         * @brief - Assign the input state to this combobox. The state can either be identical to
         *          the current state in which case this method does nothing or need a repaint in
         *          which case this method does the necessary work to do so.
         *          Changing the state will likely result in a resize of the component in order to
         *          be able to display all the options.
         * @param state - the new state to assign to this combobox.
         */
        void
        setState(const State& state);

        /**
         * @brief - Used to retrieve the size of this combobox when it is dropped. We use the box
         *          describing the size of the box when closed (held in the internal `m_closedBox`
         *          attribute) and multiply it by the number of items to be displayed.
         *          The number of items to be displayed is the minimum between the total number of
         *          items available and the `m_maxVisibleItems` value.
         * @return - the size of this combobox when it is dropped.
         */
        utils::Boxf
        getDroppedSize() const noexcept;

        /**
         * @brief - Used to retrieve the visible items count. This value is at least one, and is
         *          the minimum between the total items count and the maximum visible count.
         * @return - the number of items visible.
         */
        int
        getVisibleItemsCount() const noexcept;

        /**
         * @brief - Retrieves the name of the icon widget for the specified index.
         * @param id - the index of the icon name which shall be retrieved.
         * @return - the string assigned to the icon widget representing item `id`.
         */
        std::string
        getIconNameFromID(const int& id) const noexcept;

        /**
         * @brief - Retrieves the name of the text widget for the specified index.
         * @param id - the index of the text name which shall be retrieved.
         * @return - the string assigned to the text widget representing item `id`.
         */
        std::string
        getTextNameFromID(const int& id) const noexcept;

      private:

        /**
         * @brief - Convenience structure allowing to store the information related to a combobox
         *          item. Basically contains its associated text and the potential icon associated
         *          to it.
         */
        struct ComboBoxItem {
          std::string text;
          std::string icon;
        };

        using ItemsMap = std::vector<ComboBoxItem>;

        InsertPolicy m_insertPolicy;
        int m_maxVisibleItems;

        /**
         * @brief - Describes the current state of the combobox. For now the combobox has two macro
         *          states which corresponds to a situation where all the options are displayed or
         *          only the active one.
         */
        State m_state;

        /**
         * @brief - Describes the area assigned to this widget when in closed state. We need to keep
         *          this value in order to allow the widget to be opened and closed without needing
         *          to ask the layout about the size for this widget.
         */
        utils::Boxf m_closedBox;

        int m_activeItem;
        ItemsMap m_items;

    };

    using ComboBoxShPtr = std::shared_ptr<ComboBox>;
  }
}

# include "ComboBox.hxx"

#endif    /* COMBO_BOX_HH */
