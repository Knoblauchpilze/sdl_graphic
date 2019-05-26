#ifndef    LINEARLAYOUT_HH
# define   LINEARLAYOUT_HH

# include <memory>
# include <maths_utils/Size.hh>
# include <sdl_core/Layout.hh>
# include <sdl_core/SizePolicy.hh>

namespace sdl {
  namespace graphic {

    class LinearLayout: public core::Layout {
      public:

        LinearLayout(const Direction& direction,
                     const float& margin = 1.0f,
                     const float& interMargin = 0.0f,
                     core::SdlWidget* widget = nullptr);

        virtual ~LinearLayout();

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
        computeDefaultWidgetBox(const utils::Sizef& area,
                                const unsigned& widgetsCount) const;

        utils::Sizef
        computeSizeOfWidgets(const std::vector<utils::Boxf>& boxes) const;

      private:

        Direction m_direction;
        float m_componentMargin;

    };

    using LinearLayoutShPtr = std::shared_ptr<LinearLayout>;
  }
}

# include "LinearLayout.hxx"

#endif    /* LINEARLAYOUT_HH */
