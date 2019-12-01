#ifndef    PROGRESS_BAR_HXX
# define   PROGRESS_BAR_HXX

# include "ProgressBar.hh"

namespace sdl {
  namespace graphic {

    inline
    ProgressBar::~ProgressBar() {}

    inline
    float
    ProgressBar::getFrameDimensions() noexcept {
      return 5.0f;
    }

    inline
    const char*
    ProgressBar::getMaskName() noexcept {
      return "progress_mask";
    }

    inline
    void
    ProgressBar::updatePrivate(const utils::Boxf& window) {
      // Call the base class method.
      core::SdlWidget::updatePrivate(window);

      // Update the size of the mask to be consistent with the current state.
      updateMaskSizeFromCompletion();
    }

    inline
    core::SdlWidget*
    ProgressBar::getMask() {
      return getChildAs<core::SdlWidget>(getMaskName());
    }

  }
}

#endif    /* PROGRESS_BAR_HXX */
