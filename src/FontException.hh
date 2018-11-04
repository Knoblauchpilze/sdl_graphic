#ifndef    FONTEXCEPTION_HH
# define   FONTEXCEPTION_HH

# include <sdl_core/SdlException.hh>

namespace sdl {
  namespace graphic {

    class FontException: public sdl::core::SdlException {
      public:

        FontException(const std::string& cause);

        virtual ~FontException() = default;
    };

  }
}

# include "FontException.hxx"

#endif    /* FONTEXCEPTION_HH */