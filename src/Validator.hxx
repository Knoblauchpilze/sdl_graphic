#ifndef    VALIDATOR_HXX
# define   VALIDATOR_HXX

# include "Validator.hh"

namespace sdl {
  namespace graphic {

    inline
    Validator::Validator(const std::string& name):
      utils::CoreObject(name)
    {
      setService(std::string("validator"));
    }

    inline
    Validator::~Validator() {}

    inline
    void
    Validator::fixup(std::string& /*input*/) const {
      // Nothing magic at this level. Inheriting classes are encouraged to do better.
    }

  }
}

#endif    /* VALIDATOR_HXX */
