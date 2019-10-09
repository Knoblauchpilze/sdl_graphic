#ifndef    FLOAT_VALIDATOR_HH
# define   FLOAT_VALIDATOR_HH

# include <memory>
# include <string>
# include <limits>
# include "Validator.hh"

namespace sdl {
  namespace graphic {

    class FloatValidator: public Validator {
      public:

        /**
         * @brief - Creates a new validator allowing to validate float input. Will return valid
         *          only if the float is a valid number. The user can specify a range which the
         *          number should lie into.
         *          Note that the validator is not strict in the sense that numbers equal to the
         *          bounds are considered valid.
         * @param lower - the lower bound of the validation interval.
         * @param upper - the upper bound of the validation interval.
         */
        FloatValidator(float lower = std::numeric_limits<float>::lowest(),
                       float upper = std::numeric_limits<float>::max());

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
         * @brief - The lower bound of the range where float values are considered valid.
         */
        float m_lower;

        /**
         * @brief - The lower bound of the range where float values are considered valid.
         */
        float m_upper;
    };

    using FloatValidatorShPtr = std::shared_ptr<FloatValidator>;
  }
}

# include "FloatValidator.hxx"

#endif    /* FLOAT_VALIDATOR_HH */
