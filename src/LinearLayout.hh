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

        int
        removeItem(core::LayoutItem* item) override;

        const Direction&
        getDirection() const noexcept;

        const float&
        getComponentMargin() const noexcept;

      protected:

        void
        computeGeometry(const utils::Boxf& window) override;

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
