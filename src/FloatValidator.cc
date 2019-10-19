
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

      // Detect valid cases.
      if (val >= m_lower && val <= m_upper) {
        return State::Valid;
      }

      // We now need to check for the expected number notation: indeed when using
      // the scientific notation we cannot really rely on the number of digits to
      // determine whether we can still add numbers.
      // We will use dedicated handlers to perform the analysis.
      switch (m_notation) {
        case number::Notation::Scientific:
          return validateStandardNotation(val, input.size());
        case number::Notation::Standard:
          return validateScientificNotation(val, input.size());
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
    FloatValidator::validateStandardNotation(float /*value*/,
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

    Validator::State
    FloatValidator::validateScientificNotation(float /*value*/,
                                               int /*digits*/) const noexcept
    {
      // TODO: Implementation.
      return State::Invalid;
    }

  }
}
