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

        float
        computeIncompressibleSize(std::unordered_set<unsigned>& hintedWidgets,
                                  const std::vector<sdl::core::SizePolicy>& widgetsPolicies,
                                  const std::vector<sdl::utils::Sizef>& widgetsHints) const;

        sdl::utils::Sizef
        computeWorkingSize(const sdl::utils::Sizef& size,
                          const float& unavailable) const;

        sdl::utils::Sizef
        computeSizeFromPolicy(const sdl::utils::Sizef& desiredSize,
                              const sdl::core::SizePolicy& policy,
                              const sdl::utils::Sizef& minSize,
                              const sdl::utils::Sizef& hint,
                              const sdl::utils::Sizef& maxSize) const;

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
