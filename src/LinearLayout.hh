#ifndef    LINEARLAYOUT_HH
# define   LINEARLAYOUT_HH

# include <memory>
# include <unordered_set>
# include <sdl_core/Layout.hh>
# include <sdl_core/Size.hh>
# include <sdl_core/SizePolicy.hh>

namespace sdl {
  namespace graphic {

    class LinearLayout: public sdl::core::Layout {
      public:

        enum class Direction {
          Horizontal,
          Vertical
        };

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
        updatePrivate(const sdl::utils::Boxf& window) override;

      private:

        sdl::utils::Sizef
        computeAvailableSize(const sdl::utils::Boxf& totalArea) const;

        sdl::utils::Sizef
        computeDefaultWidgetBox(const sdl::utils::Sizef& area,
                                const unsigned& widgetsCount) const;

        sdl::utils::Sizef
        computeIncompressibleSize(std::unordered_set<unsigned>& fixedWidgetsAlongDirection,
                                  const std::vector<WidgetInfo>& widgets) const;

        sdl::utils::Sizef
        computeWorkingSize(const sdl::utils::Sizef& size,
                           const sdl::utils::Sizef& unavailable) const;

        sdl::utils::Sizef
        computeSizeFromPolicy(const sdl::utils::Sizef& desiredSize,
                              const WidgetInfo& info) const;

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
