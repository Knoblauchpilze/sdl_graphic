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
         *          number should lie into.
         *          The user can specify the expected notation for the number: this helps adapt
         *          to various user format.
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
                       float decimals = 6.0f);

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
         * @brief - Performs the validation of the input `value` extracted from a string using the
         *          standard notation. Returns a state which allow to determine whether the value
         *          is valid given the range and decimals count.
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
         * @param digits - the number of digits of the input string which generated the `value`.
         * @return - a state describing the input `value` is valid against the internal range.
         */
        State
        validateScientificNotation(float value,
                                   int digits) const noexcept;

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
        float m_decimals;

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
