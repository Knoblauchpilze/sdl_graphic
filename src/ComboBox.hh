#ifndef    COMBO_BOX_HH
# define   COMBO_BOX_HH

# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>
# include "PictureWidget.hh"

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
         * @param icon - a pointer to a picture widget to associate to this `text` entru. Note that
         *               of the provided value is null (default) this parameter is ignored and no icon
         *               will be associated to the item.
         */
        void
        insertItem(const std::string& text,
                   PictureWidget* icon = nullptr);

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
         * @param icon - a pointer to a picture widget to associate to this `text` entry. Note that
         *               if the provided value is null (default) this parameter is ignored and no
         *               icon will be associated to the item.
         */
        void
        insertItem(const int index,
                   const std::string& text,
                   PictureWidget* icon = nullptr);

        void
        removeItem(const int index);

      protected:

        /**
         * @brief - Reimplementation of the parent `SdlWidget` method to handle the repaint of this
         *          combobox. The goal is basically to display the icon and the text associated to
         *          the active item.
         * @param uuid - the identifier of the texture to draw on.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid) const override;

      private:

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

      private:

        /**
         * @brief - Convenience structure allowing to store the information related to a combobox
         *          item. Basically contains its associated text and the potential icon associated
         *          to it.
         */
        struct ComboBoxItem {
          std::string text;
          PictureWidget* icon;
        };

        using ItemsMap = std::vector<ComboBoxItem>;

        InsertPolicy m_insertPolicy;
        int m_maxVisibleItems;

        int m_activeItem;
        ItemsMap m_items;

    };

    using ComboBoxShPtr = std::shared_ptr<ComboBox>;
  }
}

# include "ComboBox.hxx"

#endif    /* COMBO_BOX_HH */
