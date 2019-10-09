
# include "FloatValidator.hh"

namespace sdl {
  namespace graphic {

    Validator::State
    FloatValidator::validate(const std::string& input) const {
      log(
        std::string("Should perform float validation of \"") + input + "\"",
        utils::Level::Warning
      );
      // TODO: Implementation.
      return State::Valid;
    }

  }
}
