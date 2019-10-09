#ifndef    INT_VALIDATOR_HXX
# define   INT_VALIDATOR_HXX

# include "IntValidator.hh"

namespace sdl {
  namespace graphic {

    inline
    IntValidator::IntValidator(int lower,
                               int upper):
      Validator(std::string("int_validator")),

      m_lower(lower),
      m_upper(upper)
    {}

    inline
    IntValidator::~IntValidator() {}

    inline
    void
    IntValidator::setLowerBound(int lower) noexcept {
      m_lower = lower;
      m_upper = std::max(m_upper, m_lower);
    }

    inline
    void
    IntValidator::setUpperBound(int upper) noexcept {
      m_upper = upper;
      m_lower = std::min(m_lower, m_upper);
    }

  }
}

#endif    /* INT_VALIDATOR_HXX */
