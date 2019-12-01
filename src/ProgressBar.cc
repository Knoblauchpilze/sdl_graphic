
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
      Guard guard(m_propsLocker);

      // Assign the progression if it is different.
      bool changed = false;
      float newComp = std::max(0.0f, std::min(1.0f, value));

      if (m_completion != newComp) {
        log(
          "Progression is now " + std::to_string(newComp) + " (from " + std::to_string(m_completion) + ")",
          utils::Level::Notice
        );

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

      // Add only the gradient widget to the layout, the masking element
      // is handled manually as we want some sort of overlapping between
      // both elements.
      layout->addItem(gradWidget);

      // Update the mask size from the initial empty completion.
      updateMaskSizeFromCompletion();
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
      float masked = 1.0f - m_completion;

      utils::Sizef dims(masked * window.w(), window.h());
      utils::Vector2f center(window.w() / 2.0f - dims.w() / 2.0f, 0.0f);

      // Create the resize event for the mask.
      core::SdlWidget* mask = getMask();

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
