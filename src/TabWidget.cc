
# include "TabWidget.hh"

namespace sdl {
  namespace graphic {

    TabWidget::TabWidget(const std::string& name,
                         core::SdlWidget* parent,
                         const utils::Sizef& area):
      core::SdlWidget(name, area, parent)
    {}

    TabWidget::~TabWidget() {}

  }
}
