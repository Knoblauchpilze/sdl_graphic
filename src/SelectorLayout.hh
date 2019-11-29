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
                       float margin = 1.0f);

        virtual ~SelectorLayout();

        int
        addItem(core::LayoutItem* item) override;

        void
        addItem(core::LayoutItem* item,
                int index) override;

        void
        setActiveItem(const std::string& name);

        void
        setActiveItem(int index);

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
         * @brief - Reimplementation of the base `Layout` method in order to associate
         *          the logical id to the physical id. This method basically traverses
         *          the internal array of associations and try to find the corresponding
         *          id.
         * @param physID - the physical id for which the logical id should be returned.
         * @return - an logical index which corresponds to the input physical id or a
         *           negative value if no such index exists in the layout.
         */
        int
        getLogicalIDFromPhysicalID(int physID) const noexcept override;

        /**
         * @brief - Reimplementation of the base `Layout` method in order to associate
         *          the logical id to the physical id using the internal table of
         *          associations.
         * @param logicID - the logical id for which the physical id should be returned.
         * @return - an physical index which corresponds to the input logical id or a
         *           negative value if no such index exists in the layout.
         */
        int
        getPhysicalIDFromLogicalID(int logicID) const noexcept override;

        /**
         * @brief - Reimplementation of the base `Layout` method to provide update of the
         *          internal associations table between the logical id and physical id.
         * @param logicID - the logical id which has just been removed.
         * @param physID - the physical id which has just been removed.
         * @return - true if the item that was removed corresponds to the active item,
         *           and false otherwise.
         */
        bool
        onIndexRemoved(int logicID,
                       int physID) override;

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
                            int logicalID,
                            int realID);

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
