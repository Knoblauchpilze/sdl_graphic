#ifndef    FLOAT_VALIDATOR_HH
# define   FLOAT_VALIDATOR_HH

# include <memory>
# include <string>
# include <limits>
# include "Validator.hh"

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

    class FloatValidator: public Validator {
      public:

        /**
         * @brief - Creates a new validator allowing to validate float input. Will return valid
         *          only if the float is a valid number. The user can specify a range which the
         *          number should lie into. The user can specify the expected notation for the
         *          number: this helps adapt to various user formats.
         *          Despite being named `decimals` the attribute has actually little to do with
         *          its general meaning. Setting `decimals = 5` indicates that the range actually
         *          gets extended by `0.99999` in both direction if applicable (i.e. if the
         *          lower bound is negative the range is set to `[m_lower - 0.99999; m_upper]`,
         *          and in the positive case the range is set to `[m_lower; m_upper - 0.99999]`).
         *          Setting `decimals = 0` in this approach does not mean that we're actually
         *          only accepting integer values but rather than the range defined by the lower
         *          and upper bound is strict (i.e. `[lower; range]`).
         *          Note that the validator is not strict in the sense that numbers equal to the
         *          bounds are considered valid.
         * @param lower - the lower bound of the validation interval.
         * @param upper - the upper bound of the validation interval.
         * @param notation - the notation describing the expected format for numbers.
         * @param decimals - the number of decimals accepted to define a float value.
         */
        FloatValidator(float lower = std::numeric_limits<float>::lowest(),
                       float upper = std::numeric_limits<float>::max(),
                       const number::Notation& notation = number::Notation::Standard,
                       int decimals = 6);

        virtual ~FloatValidator();

        /**
         * @brief - Used to assign the lower bound for this validator. Note that if the provided
         *          value is not compatible with the desired max we update the max to be at least
         *          equal to this value.
         * @param lower - the new lower bound to set for this validator.
         */
        void
        setLowerBound(float lower) noexcept;

        /**
         * @brief - Used to assign the upper bound for this validator. Note that if the provided
         *          value is not compatible with the desired min we update the minimum to be at
         *          least equal to this value.
         * @param upper - the new upper bound to set for this validator.
         */
        void
        setUpperBound(float upper) noexcept;

        /**
         * @brief - Used to specify the expected number notation for numbers to validate. Depending
         *          on the local expectations of the user one can choose to configure this validator
         *          accordingly.
         * @param notation - the number notation to expect when validating numbers.
         */
        void
        setNotation(const number::Notation& notation) noexcept;

        /**
         * @brief - Reimplementation of the base `Validator` method to react on float input.
         *          Returns `Valid` if the input string represents a float value in the range
         *          associated to this validator.
         * @param input - the input string to validate.
         * @return - a value among the enumeration to indicate the validity of the input string.
         */
        State
        validate(const std::string& input) const override;

      private:

        /**
         * @brief - Used to update the input values with the actual bounds to use given the
         *          value of decimals count. Typically if `m_decimals` is set to `3`, the
         *          upper bound will be extended by `0.999` and the lower bound decreased by
         *          `0.999` if possible (i.e. if it is negative).
         * @param lower - output value which will contain the actual lower bound given the
         *                desired decimals count.
         * @param upper - output value which will contain the actual upper bound given the
         *                desired decimals count.
         */
        void
        accountForDecimals(float& lower, float& upper) const noexcept;

        /**
         * @brief - Performs the validation of the input `value` extracted from a string using the
         *          standard notation. Returns a state which allow to determine whether the value
         *          is valid given the range and decimals count.
         *          Note that the digits count should not include the potential signum characters
         *          (e.g. '+' or '-').
         * @param value - the float value to validate against he internal range.
         * @param digits - the number of digits of the input string which generated the `value`.
         * @return - a state describing the input `value` is valid against the internal range.
         */
        State
        validateStandardNotation(float value,
                                 int digits) const noexcept;

        /**
         * @brief - Performs the validation of the input `value` extracted from a string using the
         *          scientific notation. Returns a state which allow to determine whether the value
         *          is valid given the range and decimals count.
         * @param value - the float value to validate against he internal range.
         * @param digits - the initial string which produced the `value`.
         * @return - a state describing the input `value` is valid against the internal range.
         */
        State
        validateScientificNotation(float value,
                                   const std::string& digits) const noexcept;

      private:

        /**
         * @brief - The lower bound of the range where float values are considered valid.
         */
        float m_lower;

        /**
         * @brief - The lower bound of the range where float values are considered valid.
         */
        float m_upper;

        /**
         * @brief - The number of decimals accepted by this validator.
         */
        int m_decimals;

        /**
         * @brief - The format expected for numbers when validating input.
         */
        number::Notation m_notation;
    };

    using FloatValidatorShPtr = std::shared_ptr<FloatValidator>;
  }
}

# include "FloatValidator.hxx"

#endif    /* FLOAT_VALIDATOR_HH */
