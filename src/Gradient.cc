
# include "Gradient.hh"

namespace sdl {
  namespace graphic {

    Gradient::Gradient(const std::string& name,
                       const gradient::Mode& mode):
      utils::CoreObject(name),

      m_propsLocker(),

      m_mode(mode),
      m_stops()
    {
      setService("gradient");
    }

    Gradient::Gradient(const std::string& name,
                       const gradient::Mode& mode,
                       const core::engine::Color& low,
                       const core::engine::Color& high):
      utils::CoreObject(name),

      m_propsLocker(),

      m_mode(mode),
      m_stops()
    {
      setService("gradient");

      setColorAt(0.0f, low);
      setColorAt(1.0f, high);
    }

    void
    Gradient::setColorAt(float coord,
                         const core::engine::Color& color)
    {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      // Clamp input coordinate.
      float cCoord = std::min(1.0f, std::max(0.0f, coord));

      // Traverse the internal list of stops and insert the desired
      // stop at the right position.
      gradient::Stops::iterator it = m_stops.begin();
      bool isHere = false;
      bool replace = false;

      while (it != m_stops.end() && !isHere) {
        isHere = isBeforeStop(cCoord, it->first, replace);

        if (!isHere) {
          ++it;
        }
      }

      // Either insert the value or replace the color.
      if (it != m_stops.end() || !replace) {
        m_stops.insert(it, gradient::Stop{cCoord, color});
        return;
      }

      it->second = color;
    }

  }
}
