#ifndef    FLOAT_VALIDATOR_HXX
# define   FLOAT_VALIDATOR_HXX

# include "FloatValidator.hh"

namespace sdl {
  namespace graphic {

    inline
    FloatValidator::FloatValidator(float lower,
                                   float upper,
                                   const number::Notation& notation,
                                   float decimals):
      Validator(std::string("float_validator")),

      m_lower(lower),
      m_upper(upper),
      m_decimals(decimals),
      m_notation(notation)
    {}

    inline
    FloatValidator::~FloatValidator() {}

    inline
    void
    FloatValidator::setLowerBound(float lower) noexcept {
      m_lower = lower;
      m_upper = std::max(m_upper, m_lower);
    }

    inline
    void
    FloatValidator::setUpperBound(float upper) noexcept {
      m_upper = upper;
      m_lower = std::min(m_lower, m_upper);
    }

    inline
    void
    FloatValidator::setNotation(const number::Notation& notation) noexcept {
      m_notation = notation;
    }

  }
}

#endif    /* FLOAT_VALIDATOR_HXX */
