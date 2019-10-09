
# include "FloatValidator.hh"
# include <string>

namespace sdl {
  namespace graphic {

    Validator::State
    FloatValidator::validate(const std::string& input) const {
      // Check whether we can cast the input string to a valid integer.
      size_t end;

      float val;
      bool valid = true;
      try {
        val = std::stof(input.c_str(), &end);
      }
      catch (const std::invalid_argument& e) {
        // No conversion could be performed.
        valid = false;
      }
      catch (const std::out_of_range& e) {
        log(
          std::string("Value \"") + input + "\" seems to be a valid float but can't be represented",
          utils::Level::Warning
        );
        valid = false;
      }

      // If the conversion could not occur, the input string is considered not valid.
      if (!valid) {
        return State::Invalid;
      }

      // Also check whether there is nothing after the number in the input string.
      if (end <= input.size()) {
        return State::Invalid;
      }

      if (end == '\0') {
        log("Successfully converted \"" + input + "\" to " + std::to_string(val));
      }
      else {
        log("Could not convert \"" + input + "\" to a number");
      }

      // Now check the value of the float against the range: we can either have
      // a float which is valid (easy detection) or something which is not.
      // Some values are not valid but could be transformed into a valid number
      // while some are obviously wrong.

      // Handle valid case first.
      if (val >= m_lower && val <= m_upper) {
        return State::Valid;
      }

      // We consider values that could be transformed to the range by either
      // adding or removing some digits to be in intermediate state. All other
      // cases are marked invalid.
      // TODO: Implementation.
      return State::Intermediate;
    }

  }
}
