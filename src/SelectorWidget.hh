#ifndef    SELECTORWIDGET_HH
# define   SELECTORWIDGET_HH

# include <memory>
# include <string>
# include <vector>
# include <unordered_map>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class SelectorWidget: public sdl::core::SdlWidget {
      public:

        SelectorWidget(const std::string& name,
                       sdl::core::SdlWidget* parent = nullptr,
                       const bool transparent = false,
                       const core::Palette& palette = core::Palette(),
                       const sdl::utils::Sizef& area = sdl::utils::Sizef());

        ~SelectorWidget() = default;

        void
        setActiveWidget(const std::string& name);

        void
        setActiveWidget(const int& index);

      private:

    };

    using SelectorWidgetShPtr = std::shared_ptr<SelectorWidget>;
  }
}

# include "SelectorWidget.hxx"

#endif    /* SELECTORWIDGET_HH */
