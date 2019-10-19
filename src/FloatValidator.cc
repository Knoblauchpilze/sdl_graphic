
# include "FloatValidator.hh"
# include <string>

namespace {

  /**
   * @brief - Used to attempt to convert the input string to a valid float value.
   *          Note that if the input string cannot be converted the returned value
   *          will be `0.0` and the `ok` boolean will be set to `false` if it is
   *          not set to `nullptr`.
   * @param input - the string to convert to a float.
   * @param ok - a pointer which should be set if the user wants to know whether the
   *             input string could be successfully converted to a float value.
   * @return - the float represented by the input string or `0.0` if the string is
   *           not a valid number.
   */
  float
  convertToFloat(const std::string& input,
                 bool* ok = nullptr) noexcept
  {
    // Use the dedicated conversion function.
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
      // The value seems to be valid but cannot be represented using a float value.
      valid = false;
    }

    // Analyze the result of the conversion.
    if (ok != nullptr) {
      *ok = valid && (end >= input.size());
    }

    // Assign a `0` value in case the conversion failed.
    if (!valid || end < input.size()) {
      val = 0;
    }

    // Return the converted value.
    return val;
  }

}

namespace sdl {
  namespace graphic {

    Validator::State
    FloatValidator::validate(const std::string& input) const {
      // Try to convert the input string to an integer value: if this cannot be done
      // we have a trivial case of an invalid input.
      bool ok = false;
      float val = convertToFloat(input, &ok);

      // Trivial case of the empty string which allows to safely access at least the
      // first characters in later tests.
      if (input.empty()) {
        // Consider empty string as intermediate.
        return State::Intermediate;
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
      // TODO: Implementation, see here: https://code.woboq.org/qt5/qtbase/src/gui/util/qvalidator.cpp.html
      return State::Intermediate;
    }

  }
}
