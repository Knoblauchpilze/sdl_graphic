#ifndef    SCROLL_BAR_HH
# define   SCROLL_BAR_HH

# include <memory>
# include <sdl_core/SdlWidget.hh>
# include "LinearLayout.hh"

namespace sdl {
  namespace graphic {

    class ScrollBar: public core::SdlWidget {
      public:

        /**
         * @brief - Describes the possible orientation of the scroll bar which
         *          allows to produce both horizontal and vertical layouts.
         */
        enum class Orientation {
          Horizontal, //<! - Scroll bar is horizontal.
          Vertical    //<! - Scroll bar is vertical.
        };

      public:

        /**
         * @brief - Creates a scroll bar with the specified name and parent.
         *          The user should also specify the orientation of the scroll
         *          bar.
         * @param name - the name of this scroll bar.
         * @param orientation - the orientation of the scroll bar.
         * @param color - the background color for this scroll bar.
         * @param parent - the parent element to which this widget is attached.
         * @param area - the preferred size of this scroll bar.
         */
        ScrollBar(const std::string& name,
                  const Orientation& orientation,
                  const core::engine::Color& color,
                  core::SdlWidget* parent = nullptr,
                  const utils::Sizef& area = utils::Sizef());

        virtual ~ScrollBar();

      private:

        /**
         * @brief - Called by the constructor which allows to create the components
         *          needed by this scrollbar.
         */
        void
        build();

      private:

        /**
         * @brief - Contains the orientation of the scroll bar. Allows to specify
         *          both main orientation (horizontal or vertical) to know how the
         *          components of the scroll bar should be represented.
         */
        Orientation m_orientation;

    };

    using ScrollBarShPtr = std::shared_ptr<ScrollBar>;
  }
}

# include "ScrollBar.hxx"

#endif    /* SCROLL_BAR_HH */
