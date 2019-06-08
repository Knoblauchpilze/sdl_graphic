#ifndef    SELECTORLAYOUT_HH
# define   SELECTORLAYOUT_HH

# include <memory>
# include <sdl_core/Layout.hh>

namespace sdl {
  namespace graphic {

    class SelectorLayout: public core::Layout {
      public:

        SelectorLayout(const std::string& name,
                       core::SdlWidget* widget,
                       const float& margin = 1.0f);

        virtual ~SelectorLayout();

        int
        addItem(core::LayoutItem* item) override;

        int
        addItem(core::LayoutItem* item,
                const int& index) override;

        int
        addItem(core::LayoutItem* item,
                const unsigned& x,
                const unsigned& y,
                const unsigned& w,
                const unsigned& h) override;

        void
        setActiveItem(const std::string& name);

        void
        setActiveItem(const int& index);

        /**
         * @brief - Used to switch to the next item in the layout and activate it.
         *          According to the order into which the items have been inserted
         *          in the layout this method will activate the next available
         *          item in the list.
         *          If no item is currently selected the first one will be selected
         *          and if no more items can be found, it will loop over to the first
         *          item.
         */
        void
        switchToNext();

        int
        getActiveItemId() const;

      protected:

        void
        computeGeometry(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `Layout` class method so that we can
         *          perform the needed updates when handling an item deletion. Indeed
         *          we need to update the active item if any, as well as the internal
         *          array of associations between the logical ids of items and their
         *          real ids as defined in the base class.
         *          Note that this method should be called upon deleting an item by
         *          the base class: at this point we know that the item we want to
         *          remove exits in the layout and has not yet been deleted.
         * @param item - the index of the item to remove. This corresponds to the real
         *               id of the item to delete as described in the base `Layout`
         *               class.
         */
        void
        removeItemFromIndex(int item) override;

        /**
         * @brief - Used to handle the insertion of the input `item` at the specified
         *          `logicalID` in the layout. At this point the item is already inserted
         *          into the layout and the base class put it at index `realID`. However
         *          from the user's point of view we should do just like if the item was
         *          at position `logicalID`.
         *          In order to achieve that we maintain an internal array which contains
         *          the logical position of each real position. This allows for example to
         *          move to the next logical item and find the correct real index.
         * @param item - the item which has been inserted. Nothing happens if this value
         *               is null.
         * @param logicalID - the user defined position for this item. Indicates the position
         *                    of the `item` in the cycling loop.
         * @param realID - an index provided by the base `Layout` class and which indicates
         *                 the position of this `item` in the internal array.
         */
        void
        handleItemInsertion(core::LayoutItem* item,
                            const int& logicalID,
                            const int& realID);

      private:

        using IdToPosition = std::vector<int>;

        int m_activeItem;

        /**
         * @brief - Allows to store the logical position of the item stored at a given
         *          position in the parent table. This allows to correctly assign the
         *          rendering area to widgets based on their index in the layout.
         */
        IdToPosition m_idsToPosition;

    };

    using SelectorLayoutShPtr = std::shared_ptr<SelectorLayout>;
  }
}

# include "SelectorLayout.hxx"

#endif    /* SELECTORLAYOUT_HH */
