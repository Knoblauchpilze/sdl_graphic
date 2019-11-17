#ifndef    SCROLL_AREA_HH
# define   SCROLL_AREA_HH

# include <memory>
# include <sdl_core/SdlWidget.hh>
# include "ScrollBar.hh"
# include "GridLayout.hh"
# include "ScrollableWidget.hh"

namespace sdl {
  namespace graphic {

    class ScrollArea: public core::SdlWidget {
      public:

        /**
         * @brief - Describes the possible scroll bar display mode for this scroll area.
         *          This allows the user to customize how the scroll bars will be made
         *          visible based on the content to display in the element.
         */
        enum class BarPolicy {
          AsNeeded,   //<! - Scroll bars are displayed whenever needed.
          AlwaysOff,  //<! - Scroll bars are never displayed.
          AlwaysOn    //<! - Scroll bars are always displayed.
        };

      public:

        /**
         * @brief - Creates a scroll area with the specified name and parent.
         *          Some other information can also be provided such as the
         *          policy to use when displaying the scroll bars.
         *          Upon creating such a widget, a `build` method is called
         *          which does perform the creation of default scroll bars
         *          so that the user does not need to do so.
         *          Each scroll bar can be customized through the interface
         *          if needed anyway.
         * @param name - the name of this scroll area.
         * @param parent - the parent element to which this widget is attached.
         * @param area - the preferred size of this scroll area.
         * @param hBar - the policy to display the horizontal scroll bar.
         * @param vBar - the policy to display the vertical scroll bar.
         */
        ScrollArea(const std::string& name,
                   core::SdlWidget* parent = nullptr,
                   const utils::Sizef& area = utils::Sizef(),
                   const BarPolicy& hBar = BarPolicy::AsNeeded,
                   const BarPolicy& vBar = BarPolicy::AsNeeded);

        virtual ~ScrollArea();

        /**
         * @brief - Used to assign a new corner widget. The corner widget is the
         *          widget displayed at the intersection of the two scrollbars of
         *          this scroll area.
         *          The default constructor does not create any corner widget.
         * @param corner - the new corner widget to assign to this scroll area.
         */
        void
        setCornerWidget(core::SdlWidget* corner = nullptr);

        /**
         * @brief - Assigns a new scroll bar policy for the horizontal direction.
         *          This policy will override the existing policy.
         * @param policy - the new policy to assign to the horizontal scroll bar.
         */
        void
        setHorizontalScrollBarPolicy(const BarPolicy& policy);

        /**
         * @brief - Assigns a new horizontal scroll bar widget. This widget will
         *          override any existing scroll bar and trigger its deletion.
         *          If the scrollbar is visible this widget will be displayed
         *          right away.
         * @param scrollBar - the new horizontal scroll bar to assign to this area.
         */
        void
        setHorizontalScrollBar(ScrollBar* scrollBar);

        /**
         * @brief - Assigns a new scroll bar policy for the vertical direction.
         *          This policy will override the existing policy.
         * @param policy - the nez policy to assign to the vertical scroll bar.
         */
        void
        setVerticalScrollBarPolicy(const BarPolicy& policy);

        /**
         * @brief - Assigns a new vertical scroll bar widget. This widget will
         *          override any existing scroll bar and trigger its deletion.
         *          If the scrollbar is visible this widget will be displayed
         *          right away.
         * @param scrollBar - the new vertical scroll bar to assign to this area.
         */
        void
        setVerticalScrollBar(ScrollBar* scrollBar);

        /**
         * @brief - Used to define a new widget to display in this scroll area.
         *          The viewport represents the main widget displayed in this
         *          area which size is abstracted by the scrolling bars.
         *          If any existing viewport exists it will be marked to be
         *          deleted.
         *          Note that assigning a `null` widget to this function will
         *          clear the content displayed by this scroll area.
         * @param viewport - the new viewport widget to assign to this scroll
         *                   area.
         */
        void
        setViewport(core::SdlWidget* viewport);

      protected:

        /**
         * @brief - Reimplementation of the base `core::SdlWidget` method in order
         *          to provide a custom implementation for the size update process.
         *          Indeed when this widget is resized we need to make sure that the
         *          scroll bars are being displayed or hidden if needed.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

      private:

        /**
         * @brief - Used to retrieve a string describing the name of the viewport.
         *          This is particularly useful to easily find this component in
         *          the children list and to guarantee consistent naming all across
         *          this component.
         * @return - a string representing the name with which the viewport is
         *           registered in this object's children list.
         */
        std::string
        getViewportName() const noexcept;

        /**
         * @brief - Similar method to `getViewportName` but retrieves the name to
         *          use to retrieve the horizontal scroll bar.
         * @return - a string representing the name with which the horizontal scroll
         *           bar is registered in this object's children list.
         */
        std::string
        getHBarName() const noexcept;

        /**
         * @brief - Similar method to `getViewportName` but retrieves the name to
         *          use to retrieve the vertical scroll bar.
         * @return - a string representing the name with which the vertical scroll
         *           bar is registered in this object's children list.
         */
        std::string
        getVBarName() const noexcept;

        /**
         * @brief - Similar method to `getViewportName` but retrieves the name to
         *          use to retrieve the corner widget.
         * @return - a string representing the name with which the corner widget
         *           is registered in this object's children list.
         */
        std::string
        getCornerWidgetName() const noexcept;

        /**
         * @brief - Used to retrieve the size of the viewport widget if no
         *          scroll bars where used in this area. This basically tells
         *          how large the widget would be if it was not included in
         *          a scroll area.
         *          Note that it no viewport is specified the returned size
         *          is empty.
         *          This method assumes that the locker to protect from several
         *          thrzads accessing the data is already locked.
         * @return - a size object representing the area which would be covered
         *           by the viewport if it was not inside a scroll area.
         */
        utils::Sizef
        getMaximumViewportSize() const noexcept;

        /**
         * @brief - Used to determine whether the horizontal scroll bar is visible
         *          from both the policy (because some policy do not allow the bars
         *          to become visible) and from the size of the viewport if any).
         *          This method checks whether a width of `widegt` requires to show
         *          the horizontal scroll bar. Typical usage is to provide the
         *          current size of the widget to this method.
         *          Note that this method assumes that the locker protecting the
         *          internal attributes from concurrent access is already locked.
         * @param width - the size of the area to check.
         * @param remaining - the available space remaining considering the size
         *                    of the scroll bar. Typically this value will be a
         *                    positive value if the return value is `false` and a
         *                    negative value if the return value is `true`.
         * @return - `true` if the horizontal scroll bar is visible, and `false`
         *           otherwise.
         */
        bool
        isHSBarVisible(float width,
                       float* remaining = nullptr) const noexcept;

        /**
         * @brief - Similar to the `isHSBarVisible` method but for vertical scroll
         *          bar. This method checks whether a height of `height` requires
         *          to display the vertical scroll bar. Typical usage is to provide
         *          the current size of the widget to this method.
         *          Note that this method assumes that the locker protecting the
         *          internal attributes from concurrent access is already locked.
         * @param height - the size of the area to check.
         * @param remaining - the available space remaining considering the size
         *                    of the scroll bar. Typically this value will be a
         *                    positive value if the return value is `false` and a
         *                    negative value if the return value is `true`.
         * @return - `true` if the vertical scroll bar is visible, and `false`
         *           otherwise.
         */
        bool
        isVSBarVisible(float height,
                       float* remaining = nullptr) const noexcept;

        /**
         * @brief - Retrieves the layout associated to this scroll area as a valid
         *          reference. If the layout cannot be converted or retrieved for
         *          some reasons an error is raised.
         * @return - the layout associated to this scroll area.
         */
        GridLayout&
        getLayout() const;

        /**
         * @brief - Retrieves the viewport handler associated to this scroll area.
         *          This component should always be defined and an error is raised
         *          in case we can't find it.
         * @return - a pointer to the viewport handler. This return value is always
         *           valid if this function returns.
         */
        ScrollableWidget*
        getViewportHandler() const;

        /**
         * @brief - Removes the specified item from this scroll area. The area will
         *          be marked for update if needed.
         *          Note that if the widget does not exist an error is raised.
         * @param widget - the widget to remove.
         */
        void
        removeItem(core::SdlWidget* widget);

        /**
         * @brief - Used by the constructor to perform the creation of the needed
         *          resources for this scroll area (namely scroll bars).
         */
        void
        build();

        /**
         * @brief - Used when either a new content widget has been assigned or when
         *          the size of this widget has been changed. This method is used to
         *          update the controls like the scroll bars in order to make their
         *          appearance reflect the actual content of the scroll are.
         *          Input argument allow to get an idea of the size of this component
         *          while the size of the scrollable widget displayed in this area is
         *          retrieved through the `getMaximumViewportSize` method.
         * @param internal - the total internal size of the scroll area.
         */
        void
        updateControls(const utils::Sizef& internal);

      private:

        /**
         * @brief - Contains the policy to display the horizontal scroll bar in
         *          this element.
         */
        BarPolicy m_hBarPolicy;

        /**
         * @brief - Contains the policy to display the vertical scroll bar in
         *          this element.
         */
        BarPolicy m_vBarPolicy;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of
         *          this scroll area.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Describes the name of the various widgets used to describe this
         *          scroll area. This is used to correctly retrieve the element to
         *          delete when assigning a new scroll bar, viewport or corner.
         */
        std::string m_cornerName;
        std::string m_hBarName;
        std::string m_vBarName;

        /**
         * @brief - Describes the index of the signal emitted by the horizontal
         *          scroll bar. This identifier can be used when the scroll bar
         *          is changed to perform the disconnection of this object as
         *          listener. This prevents unused scroll bars to still send some
         *          signal to this area.
         */
        int m_hBarSignalID;

        /**
         * @brief - Similar to `m_hBarSignalID` but related to the vertical scroll
         *          bar.
         */
        int m_vBarSignalID;
    };

    using ScrollAreaShPtr = std::shared_ptr<ScrollArea>;
  }
}

# include "ScrollArea.hxx"

#endif    /* SCROLL_AREA_HH */
