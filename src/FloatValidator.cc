
# include "FloatValidator.hh"
# include <cmath>
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
      // Try to convert the input string to a float value: if this cannot be done
      // we have a trivial case of an invalid input.
      bool ok = false;
      float val = convertToFloat(input, &ok);

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
          return validateScientificNotation(val, digits);
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

        log("Value: " + std::to_string(value) + ", range: [" + std::to_string(lower) + "; " + std::to_string(upper) + "], digits: " + std::to_string(digits) + " c: " + std::to_string(lowerDigits) + "/" + std::to_string(upperDigits));

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
    FloatValidator::validateScientificNotation(float /*value*/,
                                               int /*digits*/) const noexcept
    {

      // TODO: Implementation, see here: https://code.woboq.org/qt5/qtbase/src/gui/util/qvalidator.cpp.html
      return State::Invalid;
# ifdef IMPLEMENTATION
      Q_Q(const QDoubleValidator);
      QByteArray buff;
      if (!locale.d->m_data->validateChars(input, numMode, &buff, q->dec, locale.numberOptions())) {
          return QValidator::Invalid;
      }
      if (buff.isEmpty())
          return QValidator::Intermediate;
      if (q->b >= 0 && buff.startsWith('-'))
          return QValidator::Invalid;
      if (q->t < 0 && buff.startsWith('+'))
          return QValidator::Invalid;
      bool ok = false;
      double i = buff.toDouble(&ok); // returns 0.0 if !ok
      if (i == qt_qnan())
          return QValidator::Invalid;
      if (!ok)
          return QValidator::Intermediate;
      if (i >= q->b && i <= q->t)
          return QValidator::Acceptable;


      if (notation == QDoubleValidator::StandardNotation) {
          double max = qMax(qAbs(q->b), qAbs(q->t));
          if (max < LLONG_MAX) {
              qlonglong n = pow10(numDigits(qlonglong(max)));
              // In order to get the highest possible number in the intermediate
              // range we need to get 10 to the power of the number of digits
              // after the decimal's and subtract that from the top number.
              //
              // For example, where q->dec == 2 and with a range of 0.0 - 9.0
              // then the minimum possible number is 0.00 and the maximum
              // possible is 9.99. Therefore 9.999 and 10.0 should be seen as
              // invalid.
              if (qAbs(i) > (n - std::pow(10, -q->dec)))
                  return QValidator::Invalid;
          }
      }
      return QValidator::Intermediate;
# endif
    }

  }
}
