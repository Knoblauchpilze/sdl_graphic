
# include "IntValidator.hh"

namespace sdl {
  namespace graphic {

    Validator::State
    IntValidator::validate(const std::string& input) const {
      log(
        std::string("Should perform float validation of \"") + input + "\"",
        utils::Level::Warning
      );
      // TODO: Implementation.
      return State::Valid;
    }

  }
}
