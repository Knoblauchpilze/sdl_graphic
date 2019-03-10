#ifndef    GRAPHICEXCEPTION_HXX
# define   GRAPHICEXCEPTION_HXX

# include "GraphicException.hh"

namespace sdl {
  namespace graphic {

    inline
    GraphicException::GraphicException(const std::string& message,
                                       const std::string& cause):
      utils::core::CoreException(message, sk_moduleName, cause)
    {}

  }
}

#endif    /* GRAPHICEXCEPTION_HXX */