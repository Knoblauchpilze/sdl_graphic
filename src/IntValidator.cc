
# include "IntValidator.hh"

namespace sdl {
  namespace graphic {

    Validator::State
    IntValidator::validate(const std::string& input) const {
      // Check whether we can cast the input string to a valid integer.
      char* end = nullptr;

      int val = static_cast<int>(strtol(input.c_str(), &end, 10));

      if (*end == '\0') {
        log("Successfully converted \"" + input + "\" to " + std::to_string(val));
      }
      else {
        log("Could not convert \"" + input + "\" to a number");
      }

      // Discard error cases: if we could not convert the input string
      // to an integer this is obviously a failure.
      if (*end != '\0') {
        return State::Invalid;
      }

      // Now check the value of the integer against the range: we can either have
      // an integer which is valid (easy detection) or something which is not.
      // Some values are not valid but could be transformed into a valid number
      // while some are obviously wrong.

      // Handle valid case first.
      if (val >= m_lower && val <= m_upper) {
        return State::Valid;
      }

      // We consider values that could be transformed to the range by either
      // adding or removing some digits to be in intermediate state. All other
      // cases are marked invalid.
      // TODO: Implementation, see here: https://code.woboq.org/qt5/qtbase/src/gui/util/qvalidator.cpp.html
      return State::Intermediate;
    }

  }
}
