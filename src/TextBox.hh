#ifndef    TEXT_BOX_HH
# define   TEXT_BOX_HH

# include <memory>
# include <string>
# include <core_utils/Uuid.hh>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class TextBox: public core::SdlWidget {
      public:

        TextBox(const std::string& name,
                SdlWidget* parent = nullptr,
                const utils::Sizef& area = utils::Sizef());

        virtual ~TextBox();

      private:
    };

    using TextBoxShPtr = std::shared_ptr<TextBox>;
  }
}

# include "TextBox.hxx"

#endif    /* TEXT_BOX_HH */
