#ifndef    LINEARLAYOUT_HH
# define   LINEARLAYOUT_HH

# include <memory>
# include <vector>
# include <maths_utils/Size.hh>
# include <sdl_core/Layout.hh>
# include <sdl_core/SizePolicy.hh>

namespace sdl {
  namespace graphic {

    class LinearLayout: public core::Layout {
      public:

        LinearLayout(const std::string& name,
                     core::SdlWidget* widget,
                     const Direction& direction,
                     const float& margin = 1.0f,
                     const float& interMargin = 0.0f);

        virtual ~LinearLayout();

        /**
         * @brief - Reimplementation of the base `Layout` method. The aim here is to provide
         *          an index and call the corresponding `addItem` overload.
         * @param item - the item to insert in the layout.
         * @return - the logical id of the inserted item or a negative value if the item could
         *           not be inserted for some reasons.
         */
        int
        addItem(core::LayoutItem* item) override;

        void
        addItem(core::LayoutItem* item,
                const int& index) override;

        const Direction&
        getDirection() const noexcept;

        const float&
        getComponentMargin() const noexcept;

      protected:

        void
        computeGeometry(const utils::Boxf& window) override;

      private:

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
        getLogicalIDFromPhysicalID(const int physID) const noexcept override;

        /**
         * @brief - Reimplementation of the base `Layout` method in order to associate
         *          the logical id to the physical id using the internal table of
         *          associations.
         * @param logicID - the logical id for which the physical id should be returned.
         * @return - an physical index which corresponds to the input logical id or a
         *           negative value if no such index exists in the layout.
         */
        int
        getPhysicalIDFromLogicalID(const int logicID) const noexcept override;

        /**
         * @brief - Reimplementation of the base `Layout` method to provide update of the
         *          internal associations table between the logical id and physical id.
         * @param logicID - the logical id which has just been removed.
         * @param physID - the physical id which has just been removed.
         * @return - true as this layout always needs a rebuild when an item is removed.
         */
        bool
        onIndexRemoved(const int logicID,
                       const int physID) override;

        utils::Sizef
        computeAvailableSize(const utils::Boxf& totalArea) const noexcept override;

        utils::Sizef
        computeDefaultItemBox(const utils::Sizef& area,
                              const unsigned& itemsCount) const;

        utils::Sizef
        computeSizeOfItems(const std::vector<utils::Boxf>& boxes) const;

      private:

        using IdToPosition = std::vector<int>;

        Direction m_direction;
        float m_componentMargin;

        /**
         * @brief - Allows to store the logical position of the item stored at a given
         *          position in the parent table. This allows to correctly assign the
         *          rendering area to widgets based on their index in the layout.
         */
        IdToPosition m_idsToPosition;
    };

    using LinearLayoutShPtr = std::shared_ptr<LinearLayout>;
  }
}

# include "LinearLayout.hxx"

#endif    /* LINEARLAYOUT_HH */
