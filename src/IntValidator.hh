#ifndef    INT_VALIDATOR_HH
# define   INT_VALIDATOR_HH

# include <memory>
# include <string>
# include <limits>
# include "Validator.hh"

namespace sdl {
  namespace graphic {

    class IntValidator: public Validator {
      public:

        /**
         * @brief - Creates a new validator allowing to validate integer input. Will return valid
         *          only if the integer is a valid number. The user can specify a range which the
         *          number should lie into.
         *          Note that the validator is not strict in the sense that numbers equal to the
         *          bounds are considered valid.
         * @param lower - the lower bound of the validation interval.
         * @param upper - the upper bound of the validation interval.
         */
        IntValidator(int lower = std::numeric_limits<int>::lowest(),
                     int upper = std::numeric_limits<int>::max());

        virtual ~IntValidator();

        /**
         * @brief - Used to assign the lower bound for this validator. Note that if the provided
         *          value is not compatible with the desired max we update the max to be at least
         *          equal to this value.
         * @param lower - the new lower bound to set for this validator.
         */
        void
        setLowerBound(int lower) noexcept;

        /**
         * @brief - Used to assign the upper bound for this validator. Note that if the provided
         *          value is not compatible with the desired min we update the minimum to be at
         *          least equal to this value.
         * @param upper - the new upper bound to set for this validator.
         */
        void
        setUpperBound(int upper) noexcept;

        /**
         * @brief - Reimplementation of the base `Validator` method to react on integer input.
         *          Returns `Valid` if the input string represents an integer in the range
         *          associated to this validator.
         * @param input - the input string to validate.
         * @return - a value among the enumeration to indicate the validity of the input string.
         */
        State
        validate(const std::string& input) const override;

      private:

        /**
         * @brief - The lower bound of the range where integer values are considered valid.
         */
        int m_lower;

        /**
         * @brief - The lower bound of the range where integer values are considered valid.
         */
        int m_upper;
    };

    using IntValidatorShPtr = std::shared_ptr<IntValidator>;
  }
}

# include "IntValidator.hxx"

#endif    /* INT_VALIDATOR_HH */
