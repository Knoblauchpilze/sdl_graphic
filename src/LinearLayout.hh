#ifndef    LINEARLAYOUT_HH
# define   LINEARLAYOUT_HH

# include <memory>
# include <sdl_core/Layout.hh>
# include <sdl_core/Size.hh>
# include <sdl_core/SizePolicy.hh>

namespace sdl {
  namespace graphic {

    class LinearLayout: public sdl::core::Layout {
      public:

        LinearLayout(const Direction& direction,
                     const float& margin = 1.0f,
                     const float& interMargin = 0.0f,
                     sdl::core::SdlWidget* widget = nullptr);

        virtual ~LinearLayout();

        const Direction&
        getDirection() const noexcept;

        const float&
        getMargin() const noexcept;

        const float&
        getComponentMargin() const noexcept;

      protected:

        void
        updatePrivate(const utils::maths::Boxf& window) override;

      private:

        utils::maths::Sizef
        computeAvailableSize(const utils::maths::Boxf& totalArea) const;

        utils::maths::Sizef
        computeDefaultWidgetBox(const utils::maths::Sizef& area,
                                const unsigned& widgetsCount) const;

      private:

        Direction m_direction;
        float m_margin;
        float m_componentMargin;

    };

    using LinearLayoutShPtr = std::shared_ptr<LinearLayout>;
  }
}

# include "LinearLayout.hxx"

#endif    /* LINEARLAYOUT_HH */
