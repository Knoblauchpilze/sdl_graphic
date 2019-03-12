#ifndef    GRAPHICEXCEPTION_HXX
# define   GRAPHICEXCEPTION_HXX

# include "GraphicException.hh"

namespace sdl {
  namespace graphic {

    inline
    GraphicException::GraphicException(const std::string& message,
                                       const std::string& module,
                                       const std::string& cause):
      utils::CoreException(message, module, sk_serviceName, cause)
    {}

  }
}

#endif    /* GRAPHICEXCEPTION_HXX */