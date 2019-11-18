#ifndef    VIRTUAL_LAYOUT_ITEM_HH
# define   VIRTUAL_LAYOUT_ITEM_HH

# include <memory>
# include <sdl_core/LayoutItem.hh>

namespace sdl {
  namespace graphic {

    class VirtualLayoutItem: public core::LayoutItem {
      public:

        VirtualLayoutItem(const std::string& name,
                          const utils::Sizef& min = utils::Sizef(),
                          const utils::Sizef& hint = utils::Sizef(),
                          const utils::Sizef& max = utils::Sizef::max(),
                          const core::SizePolicy& policy = core::SizePolicy());

        virtual ~VirtualLayoutItem();

        /**
         * @brief - Indicates that this item should manage the width of another item.
         *          This indicates that received `Resize` events should only be considered
         *          for their width component.
         *          Note that the `setManageHeight` can also be activated at the same time,
         *          both flags are not mutually exclusive.
         * @param managed - true if the width for this virtual layout item is managed,
         *                  false otherwise.
         */
        void
        setManageWidth(const bool managed) noexcept;

        bool
        isWidthManaged() const noexcept;

        /**
         * @brief - Used to assign a position along the x coordinate for this virtual layout
         *          item. Note that this function doesn't have any effect if the width of this
         *          item is set to managed.
         * @param x - the x coordinate to assign to this layout item.
         */
        void
        setX(float x) noexcept;

        /**
         * @brief - Used to assign a width for this virtual layout item. Note that this function
         *          doesn't have any effect if the width of this item is set to managed.
         * @param width - the width to assign to this layout item.
         */
        void
        setWidth(float width) noexcept;

        /**
         * @brief - Indicates that this item should manage the height of another item.
         *          This indicates that received `Resize` events should only be considered
         *          for their height component.
         *          Note that the `setManageWidth` can also be activated at the same time,
         *          both flags are not mutually exclusive.
         * @param managed - true if the width for this virtual layout item is managed,
         *                  false otherwise.
         */
        void
        setManageHeight(const bool managed) noexcept;

        bool
        isHeightManaged() const noexcept;

        /**
         * @brief - Used to assign a position along the y coordinate for this virtual layout
         *          item. Note that this function doesn't have any effect if the height of this
         *          item is set to managed.
         * @param y - the y coordinate to assign to this layout item.
         */
        void
        setY(float y) noexcept;

        /**
         * @brief - Used to assign a height for this virtual layout item. Note that this
         *          function doesn't have any effect if the height of this item is set
         *          to managed.
         * @param height - the height to assign to this layout item.
         */
        void
        setHeight(float height) noexcept;

        void
        postLocalEvent(core::engine::EventShPtr e) override;

        /**
         * @brief - Used to retrieve the virtual box computed from all the `Resize` events
         *          received by this item and considering the internal flags indicating
         *          whether the width or height should be retrieved.
         *          This is a reimplementation of the base `LayoutItem` method in order to
         *          retrieve the virtual box instead of the actual rendering area.
         * @return - the box computed from `Resize` event.
         */
        utils::Boxf
        getRenderingArea() const noexcept override;

        /**
         * @brief - Reimplementation of the base `LayoutItem` method in order to provide
         *          a direct application of the visible status. Indeed as the virtual
         *          layout item does not process events at all we wouldn't get notified
         *          of the change in case we relied on the basic process.
         *          For this item calling `isVisible` right after calling this method
         *          does return the value set by `visible` in here.
         * @param visible - the visibility status to apply to this layout item.
         */
        void
        setVisible(bool visible) noexcept override;

        /**
         * @brief - Used to update the internal maximum size based on the value provided in
         *          the input `upperBound` size. This new size will replace the old maixmum
         *          size in case it is smaller than the initial value.
         *          Of course we also take care of the minimum size so that we keep a valid
         *          configuration for this widget.
         *          Note that if no valid configuration can be found, an error is raised.
         * @param upperBound - the maximum size which should be checked against the internal
         *                     maximum size.
         */
        void
        updateMaxSize(const utils::Sizef& upperBound);

        /**
         * @brief - Reimplementation of the base `LayoutItem` method which allows to provide
         *          the deepest item spanning the input position. In the case of virtual items
         *          we will just return either null or this widget: indeed for now this type
         *          of item is not meant to be used in really complex hierarchy and so should
         *          not be too heavily rely upon.
         *          Note that the position is supposed to be expressed in window's coordinate
         *          frame.
         * @param pos - a vector describing the position which should be spanned by the items.
         * @return - a valid pointer if any of the children items spans the input position and
         *           `null` otherwise.
         */
        const core::LayoutItem*
        getItemAt(const utils::Vector2f& pos) const noexcept override;

      protected:

        /**
         * @brief - Reimplementation of the base `EngineObject` method. A virtual layout item
         *          is not meant to process any events apart from resize ones, which will be
         *          reflected in the return value of this method.
         * @param type - the event type which should be checked for activation.
         * @return - `true` if the event type should be kept inactive when the object becomes
         *           active and `false` otherwise.
         */
        bool
        staysInactiveWhileEnabled(const core::engine::Event::Type& type) const noexcept override;

      private:

        /**
         * @brief - Update the input `min`, `hint` and `max` values with the new provided maximum
         *          value. Note that we only do something if the `newMax` is larger than the input
         *          `max` value.
         *          Only the `hint` and `max` value are supposed to be updated, the minimum value
         *          cannot be modified by this function. If the new maximum value provided cannot
         *          be conciled with the `min` value an error is raised.
         * @param min - the minimum bound to update if needed.
         * @param hint - the size hint to update.
         * @param max - the current maximum bound to be updated.
         * @param newMax - the new upper bound to assign to the `max` value.
         * @return - `true` if at least one value was updated, `false` otherwise.
         */
        bool
        extendDim(float& min,
                  float& hint,
                  float& max,
                  float newMax) const;

        /**
         * @brief - Update the input `min`, `hint` and `max` values with the new provided maximum
         *          value. Note that we only do something if the `newMax` is smaller than the input
         *          `max` value.
         *          Only the `hint` and `max` value are supposed to be updated, the minimum value
         *          cannot be modified by this function. If the new maximum value provided cannot
         *          be conciled with the `min` value an error is raised.
         * @param min - the minimum bound to update if needed.
         * @param hint - the size hint to update.
         * @param max - the current maximum bound to be updated.
         * @param newMax - the new upper bound to assign to the `max` value.
         * @return - `true` if at least one value was updated, `false` otherwise.
         */
        bool
        contractDim(float& min,
                    float& hint,
                    float& max,
                    float newMax) const;

      private:

        bool m_manageWidth;
        bool m_manageHeight;

        utils::Boxf m_box;
    };

    using VirtualLayoutItemShPtr = std::shared_ptr<VirtualLayoutItem>;
  }
}

# include "VirtualLayoutItem.hxx"

#endif    /* VIRTUAL_LAYOUT_ITEM_HH */
