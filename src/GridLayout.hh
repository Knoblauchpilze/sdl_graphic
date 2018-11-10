#ifndef    GRIDLAYOUT_HH
# define   GRIDLAYOUT_HH

# include <memory>
# include <unordered_map>
# include <sdl_core/SdlLayout.hh>

namespace sdl {
  namespace graphic {

    class GridLayout: public sdl::core::SdlLayout {
      public:

        GridLayout(const unsigned& width,
                   const unsigned& height,
                   const float& margin = 0.0f,
                   sdl::core::SdlWidget* widget = nullptr);

        virtual ~GridLayout();

        const unsigned&
        getWidth() const noexcept;

        const unsigned&
        getHeight() const noexcept;

        const float&
        getMargin() const noexcept;

        int
        addItem(sdl::core::SdlWidget* container,
                const unsigned& x,
                const unsigned& y,
                const unsigned& w,
                const unsigned& h) override;

        void
        setGrid(const unsigned& w, const unsigned& h);

      protected:

        void
        updatePrivate(const sdl::core::Boxf& area) override;

      private:

        // Convenience record to hold the position of items in the layout.
        struct ItemInfo {
          float x, y, w, h;
        };

        unsigned m_width;
        unsigned m_height;
        float m_margin;
        std::unordered_map<int, ItemInfo> m_itemsLocation;

    };

    using GridLayoutShPtr = std::shared_ptr<GridLayout>;
  }
}

# include "GridLayout.hxx"

#endif    /* GRIDLAYOUT_HH */
