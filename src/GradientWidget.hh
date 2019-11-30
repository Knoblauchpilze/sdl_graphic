#ifndef    GRADIENT_WIDGET_HH
# define   GRADIENT_WIDGET_HH

# include <mutex>
# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>
# include "Gradient.hh"

namespace sdl {
  namespace graphic {

    class GradientWidget: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a new gradient widget used to display the input
         *          gradient.
         * @param name - the name of this widget.
         * @param gradient - the gradient to represent for this widget.
         * @param parent - the parent widget for this element.
         * @param hint - the size hint for this widget.
         */
        GradientWidget(const std::string& name,
                       GradientShPtr gradient,
                       core::SdlWidget* parent = nullptr,
                       const utils::Sizef& hint = utils::Sizef());

        virtual ~GradientWidget();

        /**
         * @brief - Assign a new gradient for this widget. Will trigger a
         *          repaint operation in order to display the new gradient.
         * @param gradient - the new gradient to assign to this object.
         */
        void
        setGradient(GradientShPtr gradient);

      protected:

        /**
         * @brief - Reimplementation of the base `SdlWidget` method to represent the
         *          internal gradient object. Basically represents it using a texture
         *          with colors following the color for each coordinate.
         * @param uuid - the identifier of the canvas which we can use to draw the
         *               overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

      private:

        /**
         * @brief - Used to create the internal layout for this progress bar.
         */
        void
        build();

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this
         *          gradient widget.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Describes the gradient associated to this widget. This gradient
         *          will be represented by this object.
         */
        GradientShPtr m_gradient;
    };

    using GradientWidgetShPtr = std::shared_ptr<GradientWidget>;
  }
}

# include "GradientWidget.hxx"

#endif    /* GRADIENT_WIDGET_HH */
