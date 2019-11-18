
# include "VirtualLayoutItem.hh"
# include <sdl_engine/ResizeEvent.hh>

namespace sdl {
  namespace graphic {

    VirtualLayoutItem::VirtualLayoutItem(const std::string& name,
                                         const utils::Sizef& min,
                                         const utils::Sizef& hint,
                                         const utils::Sizef& max,
                                         const core::SizePolicy& policy):
      core::LayoutItem(name, hint),
      m_manageWidth(false),
      m_manageHeight(false),
      m_box()
    {
      // Assign size hints.
      setMinSize(min);
      setMaxSize(max);
      setSizePolicy(policy);

      // Register the filtering of all events except `Resize` ones: this
      // is automatically handled by the reimplementation of the method
      // provided by the `EngineObject` interface.
      disableEventsProcessing();
    }

    void
    VirtualLayoutItem::postLocalEvent(core::engine::EventShPtr e) {
      // The virtual layout item does not perform events queuing.
      // Instead it performs direct analysis of the event to react
      // upon receiving a resize event.

      // Check whether this is a resize event.
      if (e == nullptr || e->getType() != core::engine::Event::Type::Resize) {
        // Discard this event.
        return;
      }

      // Cast the input event into its dynamic type.
      core::engine::ResizeEventShPtr resize = std::dynamic_pointer_cast<core::engine::ResizeEvent>(e);

      // We need to keep only the coordinate and dimensions as indicated
      // by the internal flags.
      const utils::Boxf box = resize->getNewSize();

      if (isWidthManaged()) {
        m_box.x() = box.x();
        m_box.w() = box.w();
      }

      if (isHeightManaged()) {
        m_box.y() = box.y();
        m_box.h() = box.h();
      }
    }

    void
    VirtualLayoutItem::updateMaxSize(const utils::Sizef& upperBound) {
      // We need to update the internal max, min and size hint based on the value
      // of the input `upperBound`.
      // Basically we want to decrease the maximum size to not be greater than the
      // `upperBound`. We also want to decrease the size hint to not be greater
      // than the new maximum size. This is only possible if the widget can shrink
      // and if the minimum size allows it.
      // In the case that `upperBound` is larger than the current maximum size we
      // also want to update the maximum size to reflect this new size.
      // Meanwhile we also want to guarantee some consistency in this item to make
      // sure that the minimum size does not get smaller than the maximum size due
      // to a maximum size update otherwise we will not be able to apply the
      // computed size from the layout item to the actual real widget.

      // In order to work efficiently, we retrieve each size hint into a local
      // variable to avoid posting events all the time.
      utils::Sizef min = getMinSize();
      utils::Sizef hint = getSizeHint();
      utils::Sizef max = getMaxSize();

      // Update both dimensions using the corresponding handler.
      if (max.w() <= upperBound.w()) {
        extendDim(min.w(), hint.w(), max.w(), upperBound.w());
      }
      else {
        contractDim(min.w(), hint.w(), max.w(), upperBound.w());
      }

      if (max.h() <= upperBound.h()) {
        extendDim(min.h(), hint.h(), max.h(), upperBound.h());
      }
      else {
        contractDim(min.h(), hint.h(), max.h(), upperBound.h());
      }

      // Update size bounds for this item.
      setMinSize(min);
      setSizeHint(hint);
      setMaxSize(max);
    }

    bool
    VirtualLayoutItem::extendDim(float& /*min*/,
                                 float& /*hint*/,
                                 float& max,
                                 float newMax) const
    {
      // Handle trivial cases where the maximum size is already larger
      // than the `newMax`. This means that we actually don't need to
      // extend any dimension and thus can return early.
      if (max >= newMax) {
        return false;
      }

      // Now we can update the maximum size: we know that the `newMax` is
      // larger than the current maximum so we can directly assign it.
      max = newMax;

      // We updated at least the maximum value.
      return true;
    }

    bool
    VirtualLayoutItem::contractDim(float& min,
                                   float& hint,
                                   float& max,
                                   float newMax) const
    {
      // Handle trivial cases where the maximum size is already smaller
      // than the `newMax`. This means that we actually don't need to
      // contract any dimension and thus can return early.
      if (max <= newMax) {
        return false;
      }

      // We know that the new maximum is smaller than the current maximum.
      // It is required to update this value.
      max = newMax;

      // Now the maximum size is consistent with the desired `newMax`. We
      // need to handle the size hint. If it is not valid, nothing to worry
      // about. Otherwise we need to make sure that it is not greater than
      // the maximum size.

      // All this is scheduled only if the hint is valid and if the hint is
      // larger than the new maximum.
      if (!utils::fuzzyEqual(hint, 0.0f) && hint > max) {
        // The current `hint` size is larger than the desired maximum
        // size based on the input `newMax`. This is only a problem if
        // we cannot shrink it.
        if (getSizePolicy().canShrinkHorizontally()) {
          hint = max;
        }
        else {
          // The widget cannot be shrunk, this is a problem.
          error(
            std::string("Cannot assign upper bound " + std::to_string(newMax) + " to layout item"),
            std::string("Item cannot shrink horizontally")
          );
        }
      }

      // The size hint is now either not existing or consistent with the
      // maximum size. One last step is to ensure that the minimum size
      // is also consistent with it. Basically we cannot do much here,
      // either the maximum size is larger than the minimum size and we're
      // all set, or it is not in which case it means that no matter what
      // we do we will not be able to assign properly the computed size to
      // the real widget afterwards.

      // All this is bound to whether we have a minimum size in the firts place.
      if (!utils::fuzzyEqual(min, 0.0f) && min > max) {
        error(
          std::string("Cannot assign upper bound " + std::to_string(newMax) + " to layout item"),
          std::string("Inconsistent with desired minimum width")
        );
      }

      // We updated at least one value.
      return true;
    }

  }
}
