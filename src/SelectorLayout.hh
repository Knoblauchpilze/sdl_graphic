#ifndef    SELECTORLAYOUT_HH
# define   SELECTORLAYOUT_HH

# include <memory>
# include <sdl_core/Layout.hh>

namespace sdl {
  namespace graphic {

    class SelectorLayout: public core::Layout {
      public:

        SelectorLayout(const float& margin = 1.0f,
                       core::SdlWidget* widget = nullptr);

        virtual ~SelectorLayout();

        int
        addItem(core::LayoutItem* item) override;

        int
        addItem(core::LayoutItem* item,
                const unsigned& x,
                const unsigned& y,
                const unsigned& w,
                const unsigned& h) override;

        int
        removeItem(core::LayoutItem* item) override;

        void
        setActiveItem(const std::string& name);

        void
        setActiveItem(const int& index);

        int
        getActiveItemId() const;

      protected:

        void
        computeGeometry(const utils::Boxf& window) override;

        void
        handleItemInsertion(core::LayoutItem* item);

      private:

        int m_activeItem;

    };

    using SelectorLayoutShPtr = std::shared_ptr<SelectorLayout>;
  }
}

# include "SelectorLayout.hxx"

#endif    /* SELECTORLAYOUT_HH */
