
# include "IntValidator.hh"
# include <cmath>

namespace {

  /**
   * @brief - Used to attempt to convert the input string to a valid integer value.
   *          Note that if the input string cannot be converted the returned value
   *          will be `0` and the `ok` boolean will be set to `false` if it is not
   *          set to `nullptr`.
   * @param input - the string to convert to an integer.
   * @param ok - a pointer which should be set if the user wants to know whether the
   *             input string could be successfully converted to an integer value.
   * @return - the integer represented by the input string or `0` if the string is
   *           not a valid number.
   */
  int
  convertToInt(const std::string& input,
               bool* ok = nullptr) noexcept
  {
    // Use the dedicated conversion function.
    char* end = nullptr;
    int val = static_cast<int>(strtol(input.c_str(), &end, 10));

    // Analyze the result of the conversion.
    if (ok != nullptr) {
      *ok = (*end == '\0');
    }

    // Assign a `0` value in case the conversion failed.
    if (*end != '\0') {
      val = 0;
    }

    // Return the converted value.
    return val;
  }

}

namespace sdl {
  namespace graphic {

    Validator::State
    IntValidator::validate(const std::string& input) const {
      // Try to convert the input string to an integer value: if this cannot be done
      // we have a trivial case of an invalid input.
      bool ok = false;
      int val = convertToInt(input, &ok);

      // Trivial case of the empty string which allows to safely access at least the
      // first characters in later tests.
      if (input.empty()) {
        // Consider empty string as intermediate.
        return State::Intermediate;
      }

      // Check whether the input string has at least the right sign compared to the
      // range. In order to do that we want to check for the existence of the '-'
      // and '+' characters in the string.
      if (m_lower >= 0 && input[0] == '-') {
        return State::Invalid;
      }
      if (m_upper < 0 && input[0] == '+') {
        return State::Invalid;
      }

      // Check whether the string is composed of a single '-' or '+' character: the
      // conversion fails on this type of string but it is considered intermediate
      // instead of `Invalid` so we need to check that before using the result of
      // the `ok` value.
      if ((input[0] == '-' || input[0] == '+') && input.size() == 1u) {
        return State::Intermediate;
      }

      // Now we can check if the conversion was successful: if this is not the case
      // it means that something is wrong with the input string and thus we can say
      // that's it's invalid in regard of the range.
      if (!ok) {
        return State::Invalid;
      }

      // Detect valid cases.
      if (val >= m_lower && val <= m_upper) {
        return State::Valid;
      }

      // Compute the number of digits needed to write the upper bound. We want
      // to account for the '+' or '-' sign when determining the length. This
      // will be used when determining whether an input can be modified to
      // make it valid.
      int digits = input.size();
      if (input[0] == '-' || input[0] == '+') {
        --digits;
      }
      int lowerDigits = (m_lower == 0 ? 1 : static_cast<int>(std::log10(std::abs(m_lower)) + 1));
      int upperDigits = (m_upper == 0 ? 1 : static_cast<int>(std::log10(std::abs(m_upper)) + 1));

      // The input value represents a value which is not in the specified range.
      // We can't have any `Valid` value produced here but we still need to see
      // if the value is invalid or intermediate (i.e. modified to reach a valid
      // state). As a general rule of thumb we consider that we can add some
      // characters to the input to make it valid but not remove some. Otherwise
      // it makes everything intermediate (as basically we are allowed to fully
      // edit the content).
      if (val < 0) {
        // Let's use some examples to find out what to do.
        // Value: `-3`
        //  1. Range: `[  30,  60 ]`
        //  2. Range: `[   5,   6 ]`
        //  3. Range: `[   2,   6 ]`
        //  4. Range: `[   1,   2 ]`
        //  5. Range: `[  -1,   6 ]`
        //  6. Range: `[  -6,   1 ]`
        //  7. Range: `[  -2,  -1 ]`
        //  8. Range: `[  -4,  -2 ]`
        //  9. Range: `[  -5,  -4 ]`
        // 10. Range: `[ -50, -40 ]`
        //
        // Case `1` should be considered invalid (we can't make the input valid
        // without deleting some characters).
        // Case `2` should be considered invalid (adding digits won't help).
        // Case `3` should be considered invalid (adding digits won't help).
        // Case `4` should be considered invalid (adding digits won't help).
        // Case `5` should be considered invalid (adding digits won't help).
        // Case `6` should be considered valid (and is thus handled before that).
        // Case `7` should be considered invalid (adding digits won't help).
        // Case `8` should be considered valid (and is thus handled before that).
        // Case `9` should be considered invalid (adding digits won't help).
        // Case `10` should be considered intermediate (adding digits can make the
        // input valid).

        // Case ` 9`, `7`, `6`, `5`, `4`, `3`, `2` and `1`.
        if ((val > m_upper && digits >= upperDigits) || (val < m_lower && digits >= lowerDigits)) {
          return State::Invalid;
        }

        // Case `10`.
        return State::Intermediate;
      }
      else {
        // Let's use some examples to find out what to do.
        // Value: `3`
        //  1. Range: `[  30,  60 ]`
        //  2. Range: `[   5,   6 ]`
        //  3. Range: `[   2,   6 ]`
        //  4. Range: `[   1,   2 ]`
        //  5. Range: `[  -1,   6 ]`
        //  6. Range: `[  -6,   1 ]`
        //  7. Range: `[  -2,  -1 ]`
        //  8. Range: `[  -4,  -2 ]`
        //  9. Range: `[  -5,  -4 ]`
        // 10. Range: `[ -50, -40 ]`
        //
        // Case `1` should be considered intermediate (adding digits can make the
        // input valid).
        // Case `2` should be considered invalid (adding digits won't help).
        // Case `3` should be considered valid (and is thus handled before that).
        // Case `4` should be considered invalid (adding digits won't help).
        // Case `5` should be considered valid (and is thus handled before that).
        // Case `6` should be considered intermediate (adding a minus sign can make
        // the input valid).
        // Case `7` should be considered invalid (adding digits or a minus sign won't
        // help).
        // Case `8` should be considered intermediate (adding a minus sign can make
        // the input valid).
        // Case `9` should be considered invalid (adding digits or a minus sign won't
        // help).
        // Case `10` should be considered intermediate (adding digits and a minus sign
        // could make the input valid).

        // Case `1` and `10`.
        if ((val < m_lower && digits < lowerDigits && val * m_lower > 0) ||
            (-val > m_upper && digits < upperDigits && val * m_upper < 0))
        {
          return State::Intermediate;
        }

        // Case `4` and `7`.
        if ((val > m_upper && digits >= upperDigits && val * m_upper > 0) ||
            (-val < m_lower && digits >= lowerDigits && val * m_lower < 0))
        {
          return State::Invalid;
        }

        // Case `2` and `9`.
        if ((val < m_lower && digits >= lowerDigits && val * m_lower > 0) ||
            (-val > m_upper && digits >= upperDigits && val * m_upper < 0))
        {
          return State::Invalid;
        }

        // Case `6` and `8`.
        return State::Intermediate;
      }
    }

  }
}
