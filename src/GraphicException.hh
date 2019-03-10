#ifndef    GRAPHICEXCEPTION_HH
# define   GRAPHICEXCEPTION_HH

# include <core_utils/CoreException.hh>

namespace sdl {
  namespace graphic {

    class GraphicException: public utils::core::CoreException {
      public:

        GraphicException(const std::string& message,
                         const std::string& cause = std::string());

        virtual ~GraphicException() = default;

      private:

        static const char* sk_moduleName;
    };

  }
}

# include "GraphicException.hxx"

#endif    /* GRAPHICEXCEPTION_HH */