
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

    core::engine::Color
    Gradient::getColorAt(float coord) const noexcept {
      // Protect from concurrent accesses.
      Guard guard(m_propsLocker);

      core::engine::Color transparentBlack = core::engine::Color::fromRGBA(0.0f, 0.0f, 0.0f, 0.0f);

      // Check the case where no stops are provided.
      if (m_stops.empty()) {
        return transparentBlack;
      }

      // If a single stop is provided, we need to return its value
      // as no matter where the `coord` is located compared to the
      // stop's position the fill behavior will kicks in.
      if (m_stops.size() == 1u) {
        return m_stops[0u].second;
      }

      // Now check whether the `coord` is smaller than the first
      // stop's location or larger than the last one: in these
      // cases we want to return the corresponding color.
      bool replace = false;

      bool before = isBeforeStop(coord, m_stops[0u].first, replace);
      if (before) {
        return m_stops[0u].second;
      }

      before = isBeforeStop(m_stops.back().first, coord, replace);
      if (before) {
        return m_stops.back().second;
      }

      // At this point we know that the `coord` lies within the
      // range defined by all the stops. it's just a matter of
      // finding where.
      unsigned id = 0u;
      unsigned upBound = m_stops.size() - 1u;
      bool smaller = true;

      while (id < upBound && smaller) {
        // If the current stop is smaller than the input `coord`, continue.
        smaller = isBeforeStop(coord, m_stops[id + 1u].first, replace);
        if (smaller) {
          ++id;
        }
      }

      // Check consistency.
      if (id > upBound) {
        log(
          std::string("Could not determine color for coordinate ") + std::to_string(coord) +
          ", last stop is " + std::to_string(m_stops.back().first),
          utils::Level::Error
        );

        return transparentBlack;
      }

      // Mix colors for both stops.
      return mixStops(m_stops[id], m_stops[id + 1u], coord);
    }

    core::engine::Color
    Gradient::mixStops(const gradient::Stop& low,
                       const gradient::Stop& high,
                       float coord) const noexcept
    {
      // Compute the percentage of the interval corresponding to the
      // `coord` position. We assume that the stops are different as
      // they are provided by the internal methods.
      float perc = std::min(1.0f, std::max(0.0f, (coord - low.first) / (high.first - low.first)));

      float gradR = high.second.r() - low.second.r();
      float gradG = high.second.g() - low.second.g();
      float gradB = high.second.b() - low.second.b();
      float gradA = high.second.a() - low.second.a();

      return core::engine::Color::fromRGBA(
        low.second.r() + perc * gradR,
        low.second.g() + perc * gradG,
        low.second.b() + perc * gradB,
        low.second.a() + perc * gradA
      );
    }

  }
}
