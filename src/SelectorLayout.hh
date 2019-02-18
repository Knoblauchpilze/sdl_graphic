#ifndef    SELECTORLAYOUT_HH
# define   SELECTORLAYOUT_HH

# include <memory>
# include <sdl_core/Layout.hh>

namespace sdl {
  namespace graphic {

    class SelectorLayout: public sdl::core::Layout {
      public:

        SelectorLayout(const float& margin = 1.0f,
                       sdl::core::SdlWidget* widget = nullptr);

        virtual ~SelectorLayout();

        const float&
        getMargin() const noexcept;

        void
        setActiveItem(const std::string& name);

        void
        setActiveItem(const int& index);

        std::string
        getActiveItem() const;

        int
        getActiveItemId() const;

      protected:

        void
        updatePrivate(const sdl::core::Boxf& window) override;

      private:

        float m_margin;
        int m_activeItem;

    };

    using SelectorLayoutShPtr = std::shared_ptr<SelectorLayout>;
  }
}

# include "SelectorLayout.hxx"

#endif    /* SELECTORLAYOUT_HH */
