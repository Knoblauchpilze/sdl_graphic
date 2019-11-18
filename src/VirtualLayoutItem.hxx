#ifndef    VIRTUAL_LAYOUT_ITEM_HXX
# define   VIRTUAL_LAYOUT_ITEM_HXX

# include "VirtualLayoutItem.hh"

namespace sdl {
  namespace graphic {

    inline
    VirtualLayoutItem::~VirtualLayoutItem() {}

    inline
    void
    VirtualLayoutItem::setManageWidth(const bool managed) noexcept {
      m_manageWidth = managed;
    }

    inline
    bool
    VirtualLayoutItem::isWidthManaged() const noexcept {
      return m_manageWidth;
    }

    inline
    void
    VirtualLayoutItem::setX(float x) noexcept {
      m_box.x() = x;
    }

    inline
    void
    VirtualLayoutItem::setWidth(float width) noexcept {
      // Only assign if the width of this item is not managed.
      if (!isWidthManaged()) {
        m_box.w() = width;
      }
    }

    inline
    void
    VirtualLayoutItem::setManageHeight(const bool managed) noexcept {
      m_manageHeight = managed;
    }

    inline
    bool
    VirtualLayoutItem::isHeightManaged() const noexcept {
      return m_manageHeight;
    }

    inline
    void
    VirtualLayoutItem::setY(float y) noexcept {
      m_box.y() = y;
    }

    inline
    void
    VirtualLayoutItem::setHeight(float height) noexcept {
      // Only assign if the height of this item is not managed.
      if (!isHeightManaged()) {
        m_box.h() = height;
      }
    }

    inline
    utils::Boxf
    VirtualLayoutItem::getRenderingArea() const noexcept {
      return m_box;
    }

    inline
    void
    VirtualLayoutItem::setVisible(bool visible) noexcept {
      // We will trigger directly the `showEvent` or `hideEvent` handler based
      // on the input `visible` value.
      if (visible) {
        core::engine::Event s(core::engine::Event::Type::Show, this);
        s.setEmitter(this);

        showEvent(s);
      }
      else {
        core::engine::HideEvent h(getDrawingArea(), this);
        h.setEmitter(this);

        hideEvent(h);
      }
    }

    inline
    const core::LayoutItem*
    VirtualLayoutItem::getItemAt(const utils::Vector2f& /*pos*/) const noexcept {
      // The virtual layout item is by definition not visible so we should
      // act as if nothing was occupying the area. So we should always return
      // `null` as if no item where there.
      return nullptr;
    }

    inline
    bool
    VirtualLayoutItem::staysInactiveWhileEnabled(const core::engine::Event::Type& type) const noexcept {
      // We keep hide events active while enabled.
      return type != core::engine::Event::Type::Resize;
    }

  }
}

#endif    /* VIRTUAL_LAYOUT_ITEM_HXX */
