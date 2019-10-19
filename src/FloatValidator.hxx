#ifndef    FLOAT_VALIDATOR_HXX
# define   FLOAT_VALIDATOR_HXX

# include "FloatValidator.hh"
# include <cmath>

namespace sdl {
  namespace graphic {

    inline
    FloatValidator::FloatValidator(float lower,
                                   float upper,
                                   const number::Notation& notation,
                                   int decimals):
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

    inline
    void
    FloatValidator::accountForDecimals(float& lower, float& upper) const noexcept {
      // Update each value by incrementing it with `1.0 - 10^(-decimals)`.
      // Note that the lower bound will only be updated in case it is negative.
      // We handle the special case where decimals is set to `0` separately to
      // avoid numerical instabilities.
      const float overshoot = (m_decimals == 0 ? 0.0f : 1.0f - std::pow(10.0f, -1.0f * m_decimals));

      lower = m_lower;
      if (m_lower < 0.0f) {
        lower = m_lower - overshoot;
      }

      upper = m_upper;
      if (m_upper > 0.0f) {
        upper = m_upper + overshoot;
      }
    }

  }
}

#endif    /* FLOAT_VALIDATOR_HXX */
