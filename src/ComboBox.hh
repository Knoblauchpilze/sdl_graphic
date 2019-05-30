#ifndef    COMBO_BOX_HH
# define   COMBO_BOX_HH

# include <memory>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class ComboBox: public core::SdlWidget {
      public:

        ComboBox(const std::string& name,
                 SdlWidget* parent = nullptr,
                 const core::engine::Color& color = core::engine::Color(),
                 const utils::Sizef& area = utils::Sizef());

        virtual ~ComboBox();

    };

    using ComboBoxShPtr = std::shared_ptr<ComboBox>;
  }
}

# include "ComboBox.hxx"

#endif    /* COMBO_BOX_HH */
