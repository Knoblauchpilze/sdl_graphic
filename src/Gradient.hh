#ifndef    GRADIENT_HH
# define   GRADIENT_HH

# include <mutex>
# include <memory>
# include <string>
# include <vector>
# include <core_utils/CoreObject.hh>
# include <sdl_engine/Color.hh>

namespace sdl {
  namespace graphic {

    namespace gradient {

      /**
       * @brief - Description of the possible modes for a gradient.
       */
      enum class Mode {
        Linear,
        Radiant
      };

      /**
       * @brief - Defines a stop for this gradient, with a coordinate and a color.
       */
      using Stop = std::pair<float, core::engine::Color>;

      /**
       * @brief - A list of stop.
       */
      using Stops = std::vector<Stop>;

    }

    class Gradient: public utils::CoreObject {
      public:

        /**
         * @brief - Creates a gradient with the specified mode and name.
         * @param name - the name of this object.
         * @param mode - the mode of the gradient as defined in corresponding
         *               enumeration.
         */
        Gradient(const std::string& name,
                 const gradient::Mode& mode);

        /**
         * @brief - Creates a gradient with the specified mode and name. Two
         *          stops are created, one at `0` and `1` with the specified
         *          colors.
         * @param name - the name of this object.
         * @param mode - the mode of the gradient through an enumeration value.
         * @param low - the color of the coordinate `0`.
         * @param high - the color of the coordinate `1`.
         */
        Gradient(const std::string& name,
                 const gradient::Mode& mode,
                 const core::engine::Color& low,
                 const core::engine::Color& high);

        /**
         * @brief - Destruction of the object.
         */
        virtual ~Gradient() = default;

        /**
         * @brief - Retrieves the current mode for this gradient.
         */
        gradient::Mode
        getMode() const noexcept;

        /**
         * @brief - Retrieves the list of stops registered for this gradient.
         *          Note that the value is copied so cache the returned value
         *          when needed.
         * @return - the stops registered for this gradient.
         */
        gradient::Stops
        getStops() const noexcept;

        /**
         * @brief - Insert a new color at the specified coordinate. Based on the
         *          mode of the gradient the coordinate will be interpreted in a
         *          different way (radius for a radiant gradient for example).
         *          If a color already exists at this position it is replaced and
         *          it is added otherwise.
         *          Note that the input coordinate will be normalized and clamped
         *          to `1` if it exceeds it.
         * @param coord - the coordinate at which the color should be added.
         * @param color - the color to add.
         */
        void
        setColorAt(float coord,
                   const core::engine::Color& color);

        /**
         * @brief - Use to retrieve the color at the specified coordinate. The
         *          returned color is either one of the stop color defined with
         *          the `setColorAt` interface or a blend of the colors defined
         *          for the closest stops.
         *          If no stops are defined a transparent black color is returned.
         *          In case the coordinate lies outside of the bounds defined for
         *          the gradient, the closest color is returned.
         * @param coord - the coordinate at which the color should be retrieved.
         * @return - the color at the specified coordinate or transparent black.
         */
        core::engine::Color
        getColorAt(float coord) const noexcept;


      private:

        /**
         * @brief - Retrieves the comparison threshold when trying to replace a
         *          color by a new one. Basically when two stops are closer than
         *          this threshold they will be merged.
         * @return - a value representing the minimum interval to separate two
         *           distinct color stops.
         */
        static
        float
        getGradientStepThreshold() noexcept;

        /**
         * @brief - Determines whether the input `coord` lies before the specified
         *          stop value. Uses the gradient step threshold internally. One can
         *          ask whether the coordinate actually should replace the input
         *          stop based on the internal threshold.
         * @param coord - the coordinate to test.
         * @param stop - the stop to compare to the coordinate.
         * @param replace - an output value allowing to determine whether the `coord`
         *                  should replace the provided `stop`.
         * @return - `true` if the coordinate lies before the stop.
         */
        bool
        isBeforeStop(float coord,
                     float stop,
                     bool& replace) const noexcept;

        /**
         * @brief - Used to compute the color mix to apply to the coordinate specified
         *          by `coord` given the interval defined by both stops.
         *          Note that providing identical stops will cause undefined behavior.
         * @param low - the lower stop.
         * @param high - the upper stop.
         * @param coord - a coordinate supposedly lying in the range defined by the `low`
         *                and `high` stop.
         * @return - a color corresponding to the mix of `low` and `high` at the `coord`
         *           coordinates point.
         */
        core::engine::Color
        mixStops(const gradient::Stop& low,
                 const gradient::Stop& high,
                 float coord) const noexcept;

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this
         *          gradient.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - The gradient mode.
         */
        gradient::Mode m_mode;

        /**
         * @brief - The list of stops currently registered for this gradient.
         */
        gradient::Stops m_stops;
    };

    using GradientShPtr = std::shared_ptr<Gradient>;
  }
}

# include "Gradient.hxx"

#endif    /* GRADIENT_HH */
