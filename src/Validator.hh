#ifndef    VALIDATOR_HH
# define   VALIDATOR_HH

# include <memory>
# include <string>
# include <core_utils/CoreObject.hh>

namespace sdl {
  namespace graphic {

    class Validator: public utils::CoreObject {
      public:

        /**
         * @brief - Used to define the state of an expression to validate. Along with the `Valid`
         *          and `Invalid` which are self-explanatory we also add thr `Intermediate` case
         *          which describes a value which is not yet valid but not obivously invalid neither.
         */
        enum class State {
          Invalid,
          Intermediate,
          Valid
        };

      public:

        /**
         * @brief - Creates a new validator allowing to validate some user input. This class
         *          is meant to provide the basis of the validation but not perform any actual
         *          validation just yet.
         * @param name - the name of the validator.
         */
        Validator();

        virtual ~Validator();

        /**
         * @brief - Base validation method which should be implemented in subclasses. This method
         *          takes the input string and validates it against the rules defined by the class.
         *          The return value indicates whether the value is valid or invalid and if it can
         *          be modified to be made valid.
         * @param input - the input string to validate.
         * @return - a value among the enumeration to indicate the validity of the input string.
         */
        virtual
        State
        validate(const std::string& input) const = 0;

        /**
         * @brief - Provides some magic processes to attempt to fix the input string. If the string is
         *          not valid or in intermediate state this function might be able to repair it. It is
         *          not guaranteed though and the return value (or rather the `input`) should still be
         *          checked for validity.
         * @param input - the input to try to fix.
         */
        virtual
        void
        fixup(std::string& input) const;
    };

    using ValidatorShPtr = std::shared_ptr<Validator>;
  }
}

# include "Validator.hxx"

#endif    /* VALIDATOR_HH */
