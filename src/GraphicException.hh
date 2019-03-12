#ifndef    GRAPHICEXCEPTION_HH
# define   GRAPHICEXCEPTION_HH

# include <core_utils/CoreException.hh>

namespace sdl {
  namespace graphic {

    class GraphicException: public utils::CoreException {
      public:

        GraphicException(const std::string& message,
                         const std::string& module,
                         const std::string& cause = std::string());

        virtual ~GraphicException() = default;

      private:

        static const char* sk_serviceName;
    };

  }
}

# include "GraphicException.hxx"

#endif    /* GRAPHICEXCEPTION_HH */