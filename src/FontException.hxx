#ifndef    FONTEXCEPTION_HXX
# define   FONTEXCEPTION_HXX

# include "FontException.hh"

namespace sdl {
  namespace graphic {

    inline
    FontException::FontException(const std::string& cause):
      sdl::core::SdlException(cause)
    {}

  }
}

#endif    /* FONTEXCEPTION_HXX */
