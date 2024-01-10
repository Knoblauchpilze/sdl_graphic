
# include "ProgressBar.hh"
# include "LinearLayout.hh"
# include "GradientWidget.hh"

namespace sdl {
  namespace graphic {

    ProgressBar::ProgressBar(const std::string& name,
                             core::SdlWidget* parent,
                             const utils::Sizef& hint):
      core::SdlWidget(name, hint, parent, core::engine::Color::NamedColor::Silver),

      m_propsLocker(),

      m_completion(0.0f)
    {
      build();
    }

    void
    ProgressBar::setCompletion(float value) {
      // Protect from concurrent accesses.
      const std::lock_guard guard(m_propsLocker);

      // Assign the progression if it is different.
      bool changed = false;
      float newComp = std::max(0.0f, std::min(1.0f, value));

      if (m_completion != newComp) {
        verbose("Progression is now " + std::to_string(newComp) + " (from " + std::to_string(m_completion) + ")");

        m_completion = newComp;
        changed = true;
      }

      // Request a repaint and an update of the masking element if the internal
      // completion value has changed.
      if (changed) {
        updateMaskSizeFromCompletion();
        requestRepaint();
      }
    }

    void
    ProgressBar::build() {
      // No focus for this elements.
      setFocusPolicy(core::FocusPolicy());

      // Create a linear layout which will contain both the gradient
      // widget representing the progression of the loading and the
      // mask widget which will be used to `fake` some not-loaded
      // part of the gradient.
      LinearLayoutShPtr layout = std::make_shared<LinearLayout>(
        "layout_for_progress_bar",
        this,
        LinearLayout::Direction::Horizontal,
        getFrameDimensions()
      );

      if (layout == nullptr) {
        error(
          std::string("Could not create progress bar"),
          std::string("Cannot build layout")
        );
      }

      setLayout(layout);

      // Create the gradient and its corresponding widget to represent
      // the progress bar.
      core::engine::GradientShPtr gradient = std::make_shared<core::engine::Gradient>(
        "gradient_for_progress_bar",
        core::engine::gradient::Mode::Linear,
        core::engine::Color::NamedColor::Red,
        core::engine::Color::NamedColor::Green
      );

      GradientWidget* gradWidget = new GradientWidget(
        "grad_for_progress_bar",
        gradient,
        this
      );
      if (gradWidget == nullptr) {
        error(
          std::string("Could not create progress bar"),
          std::string("Cannot build gradient")
        );
      }

      // Create the mask used to hide part of the gradient in case the
      // completion is not set to maximum for the progress bar.
      core::SdlWidget* mask = new core::SdlWidget(
        getMaskName(),
        utils::Sizef(),
        this,
        core::engine::Color::NamedColor::White
      );
      if (mask == nullptr) {
        error(
          std::string("Could not create progress bar"),
          std::string("Cannot build progress mask")
        );
      }
      mask->setZOrder(1);
      mask->setFocusPolicy(core::FocusPolicy());
      mask->setAllowLog(false);

      // Add only the gradient widget to the layout, the masking element
      // is handled manually as we want some sort of overlapping between
      // both elements.
      layout->addItem(gradWidget);

      // Theoretically we should update the mask size to ensure the empty
      // initial completion. BUT as we might not have a queue yet the event
      // might not be able to be posted anyway.
      // Anyway as we should get a resize event quite early in the process
      // and as we already set the `m_completion·` to be `0` it should be
      // automatically enforced when the resize event is received. So we
      // will consider the job done here.
    }

    void
    ProgressBar::updateMaskSizeFromCompletion() {
      // Given that the gradient widget is given all the available space on this
      // widget we can assume that we are able to know its size through querying
      // this widget's size.
      // Now the goal is to resize the masking element so that it's size hides a
      // part of the gradient widget consistent with the current completion value.
      // By resizing it upon each progression notification we are able to fake
      // a feeling of progress.
      utils::Sizef window = LayoutItem::getRenderingArea().toSize();

      // Compute the masked part of the widget: this corresponds to the part that
      // it is not completed yet. We consider that the left part of the progress
      // bar is the part that is completed.
      float masked = std::min(1.0f, std::max(0.0f, 1.0f - m_completion));

      utils::Sizef dims(masked * window.w(), window.h());
      utils::Vector2f center(window.w() / 2.0f - dims.w() / 2.0f, 0.0f);

      // Create the resize event for the mask. Note that in case the mask is
      // set with a null width (typically when the percentage reaches `100%`
      // we will instead hide the mask.
      core::SdlWidget* mask = getMask();

      if (dims.w() < 1.0f) {
        mask->setVisible(false);
      }
      else {
        // Make the mask visible if needed.
        if (!mask->isVisible()) {
          mask->setVisible(true);
        }

        postEvent(
          std::make_shared<core::engine::ResizeEvent>(
            utils::Boxf(center, dims),
            mask->getRenderingArea(),
            mask
          )
        );
      }
    }

  }
}
