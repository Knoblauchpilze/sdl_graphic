#ifndef    SCROLL_BAR_HXX
# define   SCROLL_BAR_HXX

# include "ScrollBar.hh"

namespace sdl {
  namespace graphic {

    inline
    ScrollBar::~ScrollBar() {}

    inline
    void
    ScrollBar::setMinimum(int minimum) {
      // Acquire the lock on this object.
      std::lock_guard<std::mutex> guard(m_locker);

      // Check whether the new value is different from the current value.
      if (minimum != m_minimum) {
        // Assign the value and check whether it is consistent with the
        // rest of the internal properties.
        m_minimum = minimum;

        if (m_maximum < m_minimum) {
          m_minimum = m_maximum;
        }

        m_pageStep = std::min(m_maximum - m_maximum, m_pageStep);

        if (m_value < m_minimum) {
          setValuePrivate(m_minimum);
        }
      }
    }

    inline
    void
    ScrollBar::setMaximum(int maximum) {
      // Acquire the lock on this object.
      std::lock_guard<std::mutex> guard(m_locker);

      // Check whether the new value is different from the current value.
      if (maximum != m_maximum) {
        // Assign the value and check whether it is consistent with the
        // rest of the internal properties.
        m_maximum = maximum;

        if (m_maximum < m_minimum) {
          m_maximum = m_minimum;
        }

        m_pageStep = std::min(m_maximum - m_maximum, m_pageStep);

        if (m_value > m_maximum) {
          setValuePrivate(m_maximum);
        }
      }
    }

    inline
    void
    ScrollBar::setValue(int value) {
      // Acquire the lock on this object.
      std::lock_guard<std::mutex> guard(m_locker);

      // Use the dedicated handler.
      setValuePrivate(value);
    }

    inline
    void
    ScrollBar::setValuePrivate(int value) {
      // Assume that the locker is already acquired.
      int old = m_value;

      // Check whether the value is different from the current value.
      if (value != m_value) {
        // Assign the value and check whether it is consistent with the
        // admissible range for this scroll bar.
        m_value = std::min(m_maximum, std::max(m_minimum, value));

        if (old != m_value) {
          log("Emitting on value changed for " + getName(), utils::Level::Notice);
          onValueChanged.emit(getName(), m_value);
        }
      }
    }

  }
}

#endif    /* SCROLL_BAR_HXX */
