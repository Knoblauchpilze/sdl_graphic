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
        updatePrivate(const utils::Boxf& window) override;

      private:

        int m_activeItem;

    };

    using SelectorLayoutShPtr = std::shared_ptr<SelectorLayout>;
  }
}

# include "SelectorLayout.hxx"

#endif    /* SELECTORLAYOUT_HH */
