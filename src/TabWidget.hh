#ifndef    TAB_WIDGET_HH
# define   TAB_WIDGET_HH

# include <memory>
# include <vector>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class TabWidget: public core::SdlWidget {
      public:

        TabWidget(const std::string& name,
                 core::SdlWidget* parent = nullptr,
                 const utils::Sizef& area = utils::Sizef());

        virtual ~TabWidget();

    };

    using TabWidgetShPtr = std::shared_ptr<TabWidget>;
  }
}

# include "TabWidget.hxx"

#endif    /* TAB_WIDGET_HH */
