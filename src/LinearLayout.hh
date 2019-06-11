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

        int
        addItem(core::LayoutItem* item) override;

        int
        addItem(core::LayoutItem* item,
                const int& index) override;

        const Direction&
        getDirection() const noexcept;

        const float&
        getComponentMargin() const noexcept;

      protected:

        void
        computeGeometry(const utils::Boxf& window) override;

        /**
         * @brief - Reimplementation of the base `Layout` class method so that we can
         *          perform the needed updates when handling an item deletion. Indeed
         *          we need to relabel items based on their new position due to the
         *          removal of the input item.
         *          Note that this method should be called upon deleting an item by
         *          the base class: at this point we know that the item we want to
         *          remove exits in the layout and has not yet been deleted.
         * @param item - the index of the item to remove. This corresponds to the real
         *               id of the item to delete as described in the base `Layout`
         *               class.
         */
        void
        removeItemFromIndex(int item) override;

      private:

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
