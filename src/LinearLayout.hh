#ifndef    LINEARLAYOUT_HH
# define   LINEARLAYOUT_HH

# include <memory>
# include <sdl_core/Layout.hh>

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
        updatePrivate(const sdl::core::Boxf& window) override;

      private:

        float
        computeHintedSize(const std::vector<unsigned>& widgets,
                          const Direction& direction) const noexcept;

        float
        computeAvailableSize(const sdl::core::Boxf& totalArea,
                             const Direction& direction) const noexcept;

        std::vector<sdl::core::Boxf>
        handleWidgetsExpanding(const std::vector<unsigned>& widgets,
                               const float& targetSize,
                               const Direction& direction) const;

        std::vector<sdl::core::Boxf>
        handleWidgetsShrinking(const std::vector<unsigned>& widgets,
                               const float& targetSize,
                               const Direction& direction) const;

        std::vector<sdl::core::Boxf>
        handleHorizontalLayout(const sdl::core::Boxf& totalArea,
                               float& cw,
                               float& ch) const;

        std::vector<sdl::core::Boxf>
        handleVerticalLayout(const sdl::core::Boxf& totalArea,
                             float& cw,
                             float& ch) const;

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
