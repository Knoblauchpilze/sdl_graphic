#ifndef    VALIDATOR_UTILS_HXX
# define   VALIDATOR_UTILS_HXX

# include <string>
# include <sstream>
# include <stdexcept>
# include <core_utils/CoreException.hh>

namespace sdl {
  namespace graphic {

    namespace number {

      /**
       * @brief - Defines the possible number representation modes. Typical values include
       *          standard notation or scientific notation.
       */
      enum class Notation {
        Standard,
        Scientific
      };

    }

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
    inline
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
    inline
    float
    convertToFloat(const std::string& input,
                   bool* ok = nullptr) noexcept
    {
      // Use the dedicated conversion function.
      size_t end;
      bool scientific = false;
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

      // If the parsing was not valid it might mean that we're facing something written
      // with scientific notation. Let's try to convert it using a string stream. It
      // might allow for some more conversions even though after testing it seems that
      // the standard `stof` approach handles these cases just fine.
      if (!valid || end < input.size()) {
        std::stringstream stream(input);
        stream >> val;

        // Check whether the conversion did happen successfully.
        valid = !stream.fail();
        scientific = true;
      }

      // Analyze the result of the conversion.
      if (ok != nullptr) {
        *ok = valid && (scientific || end >= input.size());
      }

      // Assign a `0` value in case the conversion failed.
      if (!valid || (!scientific && end < input.size())) {
        val = 0;
      }

      // Return the converted value.
      return val;
    }

    /**
     * @brief - Used to extract the components of a string suppsedly representing a
     *          number in scientific notation. Such a number looks like "1.2e3" and
     *          the goal of this method is to populate the `leading`, `decimals` and
     *          `exponent` output arguments with the corresponding parts (i.e. for
     *          the aforementionned example respectively `1`, `2` and `3`).
     *          In case nothing is entered yet the returned value is `0` and the
     *          associated boolean is set to `false`.
     *          Note that the input string is assumed to be a *valid* number in the
     *          scientific notation space: if this is not the case it causes undefined
     *          behavior.
     * @param input - the string representing a number in scientific notation to
     *                interpret.
     * @param leading - the leading digits of this number. Should be at most one digit
     *                  long.
     * @param hasLeading - `true` if the input string contains a leading part.
     * @param decimals - the decimals of this number.
     * @param hasDecimals - `true` if the input string contains some decimals.
     * @param exponent - the exponent of this number.
     * @param hasExponent - `true` if the input string contains an exponent.
     */
    inline
    void
    extractComponents(const std::string& input,
                      int* leading = nullptr,
                      bool* hasLeading = nullptr,
                      int* decimals = nullptr,
                      bool* hasDecimals = nullptr,
                      int* exponent = nullptr,
                      bool* hasExponent = nullptr)
    {
      // The structure of a number in scientific notation should be something like `1.2e3`.
      // We will first try to determine whether each part is filled or if some are missing.
      std::size_t indexDecSep = std::min(input.find('.'), input.find(','));
      std::size_t indexExp = std::min(input.find('e'), input.find('E'));

      bool leadExist = (std::min(indexDecSep, indexExp) > 0);
      bool decExist = (indexDecSep != std::string::npos);
      bool expExist = (indexExp != std::string::npos);

      // Tokenize the string into the corresponding values. Note that we take full advantage
      // of the fact that we know beforehand the structure of a number in scientific notation.

      // The leading part runs until we reach either the end of the string, the first decimals
      // or the exponent.
      std::string strLead;
      if (leadExist) {
        strLead = input.substr(0, std::min(indexDecSep, indexExp));
      }

      std::string strDec;
      if (decExist && indexDecSep < input.size() - 1) {
        strDec = input.substr(indexDecSep + 1u, indexExp - (indexDecSep + 1u));
      }

      std::string strExp;
      if (expExist && indexExp < input.size() - 1) {
        strExp = input.substr(indexExp + 1u);
      }

      // Convert each component to an int.
      bool ok = false;

      // If the user wants to retrieve the leading part of the number.
      if (leading != nullptr) {
        if (hasLeading != nullptr) {
          *hasLeading = leadExist;
        }

        if (!leadExist) {
          *leading = 0;
        }
        else {
          *leading = convertToInt(strLead, &ok);
          if (!ok) {
            throw utils::CoreException(
              std::string("Could not convert leading part of number \"") + input + "\"",
              std::string("float"),
              std::string("validator"),
              std::string("Invalid conversion to integer")
            );
          }
        }
      }

      // If the user wants to retrieve the decimal part of the number.
      if (decimals != nullptr) {
        if (hasDecimals != nullptr) {
          *hasDecimals = decExist;
        }

        if (!decExist) {
          *decimals = 0;
        }
        else {
          *decimals = convertToInt(strDec, &ok);
          if (!ok) {
            throw utils::CoreException(
              std::string("Could not convert decimals part of number \"") + input + "\"",
              std::string("float"),
              std::string("validator"),
              std::string("Invalid conversion to integer")
            );
          }
        }
      }

      // If the user wants to retrieve the exponent part of the number.
      if (exponent != nullptr) {
        if (hasExponent != nullptr) {
          *hasExponent = expExist;
        }

        if (!expExist) {
          *exponent = 0;
        }
        else {
          *exponent = convertToInt(strExp, &ok);
          if (!ok) {
            throw utils::CoreException(
              std::string("Could not convert exponent part of number \"") + input + "\"",
              std::string("float"),
              std::string("validator"),
              std::string("Invalid conversion to integer")
            );
          }
        }
      }
    }

  }
}

#endif    /* VALIDATOR_UTILS_HXX */
