
# include "FloatValidator.hh"
# include <cmath>
# include <string>
# include "Validator_utils.hxx"

namespace sdl {
  namespace graphic {

    Validator::State
    FloatValidator::validate(const std::string& input) const {
      // Note: the actual implementation is based on what we found in the Qt repository
      // but refined in order to provide more precise analysis of intermediate and
      // invalid states. For more details, see here:
      // https://code.woboq.org/qt5/qtbase/src/gui/util/qvalidator.cpp.html.
      // Try to convert the input string to a float value: if this cannot be done
      // we have a trivial case of an invalid input.
      bool ok = false;
      float val = utils::convert(input, 0.0f, ok);

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

      // We should detect cases where the notation is expected to be scientific and
      // when we have an invalid leading part (i.e. with more than `1` digit).
      if (m_notation == number::Notation::Scientific) {
        // Retrieve the leading part of the number if any and compute its length.
        int leading = 0;
        bool hasLeading = false;
        extractComponents(input, &leading, &hasLeading);
        int digits = (std::abs(leading) == 0 ? 1 : static_cast<int>(std::log10(std::abs(leading)) + 1));

        if (hasLeading && digits > 1) {
          return State::Invalid;
        }
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

      float lower, upper;
      accountForDecimals(lower, upper);

      // Detect valid cases.
      if (val >= lower && val <= upper) {
        return State::Valid;
      }

      // Compute the number of digits used by the number to analyze. Unlike for the
      // integer case we can't really rely on the size of the input string to count
      // the number of digits so we should use the logarithm approach.
      int digits = (std::abs(val) < std::numeric_limits<float>::min() ? 1 : static_cast<int>(std::log10(std::abs(val)) + 1));

      switch (m_notation) {
        case number::Notation::Standard:
          return validateStandardNotation(val, digits);
        case number::Notation::Scientific:
          return validateScientificNotation(val, input);
        default:
          break;
      }

      // Failure to interpret the number notation results in an error.
      error(
        std::string("Could not validate input \"") + input + "\" as floating point value",
        std::string("Could not interpret number notation ") + std::to_string(static_cast<int>(m_notation))
      );

      // Make the compiler silent about not returning anything.
      return State::Invalid;
    }

    Validator::State
    FloatValidator::validateStandardNotation(float value,
                                             int digits) const noexcept
    {
      // Given that we're using standard notation we can rely on the number of digits of the
      // input data to determine whether the value is valid. We will use a process similar
      // to what's performed for the integer validation.
      float lower, upper;
      accountForDecimals(lower, upper);

      int lowerDigits = (lower == 0 ? 1 : static_cast<int>(std::log10(std::abs(lower)) + 1));
      int upperDigits = (upper == 0 ? 1 : static_cast<int>(std::log10(std::abs(upper)) + 1));

      // The input value is obviously not valid (otherwise we would have already validated it
      // in the main `validate` function) so we will either return `Invalid` or `Intermediate`.
      // The process is very similar to what happens for the integers and one should head over
      // there to get more details on the checks.
      if (value < 0) {
        // Let's use some examples to find out what to do.
        // Value: `-3`
        //  1. Range: `[  30,  60 ]` -> invalid
        //  2. Range: `[   5,   6 ]` -> invalid
        //  3. Range: `[   2,   6 ]` -> invalid
        //  4. Range: `[   1,   2 ]` -> invalid
        //  5. Range: `[  -1,   6 ]` -> invalid
        //  6. Range: `[  -6,   1 ]` -> valid
        //  7. Range: `[  -2,  -1 ]` -> invalid
        //  8. Range: `[  -4,  -2 ]` -> valid
        //  9. Range: `[  -5,  -4 ]` -> invalid
        // 10. Range: `[ -50, -40 ]` -> intermediate

        // Case ` 9`, `7`, `6`, `5`, `4`, `3`, `2` and `1`.
        if ((value > m_upper && digits >= upperDigits) || (value < m_lower && digits >= lowerDigits)) {
          return State::Invalid;
        }

        // Case `10`.
        return State::Intermediate;
      }
      else {
        // Let's use some examples to find out what to do.
        // Value: `3`
        //  1. Range: `[  30,  60 ]` -> intermediate
        //  2. Range: `[   5,   6 ]` -> invalid
        //  3. Range: `[   2,   6 ]` -> valid
        //  4. Range: `[   1,   2 ]` -> invalid
        //  5. Range: `[  -1,   6 ]` -> valid
        //  6. Range: `[  -6,   1 ]` -> intermediate
        //  7. Range: `[  -2,  -1 ]` -> invalid
        //  8. Range: `[  -4,  -2 ]` -> intermediate
        //  9. Range: `[  -5,  -4 ]` -> invalid
        // 10. Range: `[ -50, -40 ]` -> intermediate

        // Case `1` and `10`.
        if ((value < lower && digits < lowerDigits && value * lower > 0) ||
            (-value > upper && digits < upperDigits && value * upper < 0))
        {
          return State::Intermediate;
        }

        // Case `4` and `7`.
        if ((value > upper && digits >= upperDigits && value * upper > 0) ||
            (-value < lower && digits >= lowerDigits && value * lower < 0))
        {
          return State::Invalid;
        }

        // Case `2` and `9`.
        if ((value < lower && digits >= lowerDigits && value * lower > 0) ||
            (-value > upper && digits >= upperDigits && value * upper < 0))
        {
          return State::Invalid;
        }

        // Case `6` and `8`.
        return State::Intermediate;
      }
    }

    Validator::State
    FloatValidator::validateScientificNotation(float value,
                                               const std::string& digits) const noexcept
    {
      // This method is supposedly called when the input `digits` string represents a number
      // in scientific notation. Upon entering this function we already know that the input
      // value is not valid so we should try to determine whether it is an intermediate or
      // an invalid input.
      // Just like for the standard notation case we will consider that it's possible to add
      // some digits, a sign or an exponent (i.e. either '+' or '-' or 'e' or 'E') but not
      // to remove some existing symbols (otherwise everything is just intermediate and we
      // don't have anything to do).
      // Compared to the standard notation we can't really rely on the length of the input
      // string to determine whether some digits are missing or anything due to the scientific
      // notation.
      // We will also verify that the scientific notation is actually correctly handled in the
      // input string and that we're not handling something like "98e2".
      float lower, upper;
      accountForDecimals(lower, upper);

      // Determine whether there's already a decimal separator (a.k.a. '.' or ',' character)
      // or/and an exponent by extracting each component of the input number.
      // Extract each part of the input value.
      int leading = 0, decimals = 0, exponent = 0;
      bool hasLeading = false, hasDecimals = false, hasExponent = false;
      extractComponents(digits, &leading, &hasLeading, &decimals, &hasDecimals, &exponent, &hasExponent);

      // We want to detect valuies which are clearly too big or too large and which cannot
      // be made valid by adding digits and decimal separator or exponent if possible.
      int leadingDigits = (leading == 0 ? 1 : static_cast<int>(std::log10(std::abs(leading)) + 1));
      int decDigits = (decimals == 0 ? 1 : static_cast<int>(std::log10(std::abs(decimals)) + 1));
      int expDigits = (exponent == 0 ? 1 : static_cast<int>(std::log10(std::abs(exponent)) + 1));

      log("Number \"" + digits + " parsed to l: " + std::to_string(leading) + ", d: " + std::to_string(decimals) + ", e: " + std::to_string(exponent));
      log("Digits: (" + std::to_string(leadingDigits) + ", " + std::to_string(decDigits) + ", " + std::to_string(expDigits) + ")");

      // Number with more that one digit in the leading part are clearly invalid.
      if (leadingDigits > 1) {
        return State::Invalid;
      }

      // If the input number does not yet have an exponent we can't determine anything. Any number
      // can be remapped to any range using a suited exponent so we wan't to handle all these cases
      // by returning intermediate state right away.
      if (!hasExponent) {
        return State::Intermediate;
      }

      // We know want to determine clearly invalid cases. Below are a few examples to help
      // see the possible cases. A first distinction comes from the sign of the input value
      // as using a negative value already rules out half of the number space (as we don't
      // allow deletions).
      // We will consider in the rest of this method that the information can only be added
      // after the more specialized term of the number and that the sign of each component
      // cannot be changed. This helps reduce the scope of modifications that can be made to
      // a number and prevent always returning `Intermediate`.
      // Note that at this point we know that an exponent is provided in the input integer.
      if (value < 0) {
        if (exponent < 0) {
          // Ex: Value: `-1.23e-2`, range `[1, 3]`.
          if (value > m_upper) {
            // No matter the number of digits we will add to the exponent we won't be able to
            // decrease the number enough to reach the upper bound.
            return State::Invalid;
          }
        }

        if (exponent > 0) {
          // Ex: Value: `-1.23e2`, range `[-80, 3]`.
          if (value < m_lower) {
            // No matter the number of digits we will add to the exponent we won't be able to
            // increase the number enough to reach the lower bound.
            return State::Invalid;
          }
        }
      }
      else {
        if (exponent < 0) {
          // Ex: Value: `1.23e-2`, range `[1, 3]`.
          if (value < m_lower && m_lower >= 0.0f) {
            // No matter the number of digits we will add to the exponent we won't be able to
            // increase the number enough to reach the lower bound.
            return State::Invalid;
          }

          // Ex: Value: `1.23e-2`, range `[-3, -1]`.
          if (-value > m_upper && m_upper <= 0.0f) {
            // No matter the number of digits we will add to the exponent we won't be able to
            // decrease the number enough to reach the upper bound.
            return State::Invalid;
          }
        }

        if (exponent > 0) {
          // Ex: Value: `1.23e2`, range `[0.5, 1]`.
          // Ex: Value: `1.23e2`, range `[-0.5, 1]`.
          // Ex: Value: `1.23e2`, range `[-0.5, -0.1]`.
          if (value > m_upper && -value < m_lower) {
            // No matter the number of digits we will add to the exponent we won't be able to
            // decrease the number enough to reach the upper bound.
            return State::Invalid;
          }
        }
      }

      // No obvious problems with extending this number to make it valid.
      return State::Intermediate;
    }

  }
}
