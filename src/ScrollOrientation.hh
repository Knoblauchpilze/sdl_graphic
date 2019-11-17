#ifndef    SCROLL_ORIENTATION_HH
# define   SCROLL_ORIENTATION_HH

namespace sdl {
  namespace graphic {

    namespace scroll {

        /**
         * @brief - Describes the possible orientation of a scrolling motion
         *          which allows to perform motion in both horizontal and
         *          vertical direction.
         */
        enum class Orientation {
          Horizontal, //<! - Scroll motion is horizontal.
          Vertical    //<! - Scroll motion is vertical.
        };

    }

  }
}

#endif    /* SCROLL_ORIENTATION_HH */