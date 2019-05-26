
# include <sdl_core/SdlWidget.hh>
# include "SelectorLayout.hh"

namespace sdl {
  namespace graphic {

    SelectorLayout::SelectorLayout(const float& margin,
                                   core::SdlWidget* widget):
      core::Layout(widget, margin),
      m_activeItem(-1)
    {
      // Nothing to do.
    }

    SelectorLayout::~SelectorLayout() {}

    int
    SelectorLayout::removeItem(core::SdlWidget* item) {
      // We need to determine whether this item corresponds to the active item.
      // If it is the case we need to update this layout so that it does not
      // have any active widget anymore. Otherwise we have to handle the update
      // of the internal `m_activeItem` si that it still reflects the active
      // item if any.

      if (item == nullptr) {
        // Use the base handler, we cannot do anything on our side.
        return Layout::removeItem(item);
      }

      // Let's retrieve the index of this layout in the container.
      const int idItem = getIndexOf(item);

      // Check whether this item was found in the layout.
      if (idItem < 0) {
        // Use the base handler, we cannot do anything useful in here.
        return Layout::removeItem(item);
      }

      // Check whether this item is the active item.
      if (idItem == m_activeItem) {
        // No more active item.
        m_activeItem = -1;

        // Use the base handler to remove the item and we're done.
        return Layout::removeItem(item);
      }

      // The item is not the currently selected item. We need to update it
      // so that the active item stays the same even after removing the input
      // item.
      // To do so, we will first retrieve the widget associated to the active
      // item, then proceed to the removal of the input item and finally update
      // the active item with its new index.
      // Of course this only applies if there is an active item in the first
      // place.

      // Check whether there is an active item.
      if (m_activeItem < 0) {
        // We only have to perform the removal of the item using the base handler.
        return Layout::removeItem(item);
      }

      // First retrieve the widget corresponding to the active item.
      core::SdlWidget* activeItem = getWidgetAt(m_activeItem);

      // Remove the item using the dedicated handler.
      int removedID = Layout::removeItem(item);

      // Retrieve the index of the active item.
      const int newID = getIndexOf(activeItem);
      if (!isValidIndex(newID)) {
        // Should not occur: something must have gone wrong during the removal.
        m_activeItem = -1;

        error(
          std::string("Could not update active item after removing item \"") + item->getName() + "\"",
          std::string("Active item could not be retrieve")
        );
      }

      // Update the index of the active item.
      m_activeItem = newID;

      // Returnt he value provided by the base handler.
      return removedID;
    }

    void
    SelectorLayout::computeGeometry(const utils::Boxf& window) {
      // Check whether a child is active.
      if (m_activeItem < 0) {
        return;
      }

      // Compute the available space for the active child.
      const utils::Sizef componentSize = computeAvailableSize(window);

      // Assign the space for the active child: as this is the only
      // child, use all the available space.
      std::vector<utils::Boxf> bboxes(getItemsCount(), utils::Boxf());

      bboxes[m_activeItem] = utils::Boxf(
        getMargin().w() + componentSize.w() / 2.0f,
        getMargin().h() + componentSize.h() / 2.0f,
        componentSize.w(),
        componentSize.h()
      );

      // Use the base handler to assign bbox.
      assignRenderingAreas(bboxes, window);

      // Disable other items.
      std::vector<bool> visible(getItemsCount(), false);
      visible[m_activeItem] = true;
      assignVisibilityStatus(visible);
    }

  }
}
