#ifndef    GRADIENT_WIDGET_HH
# define   GRADIENT_WIDGET_HH

# include <mutex>
# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>
# include <sdl_engine/Gradient.hh>

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
                       core::engine::GradientShPtr gradient,
                       core::SdlWidget* parent = nullptr,
                       const utils::Sizef& hint = utils::Sizef());

        virtual ~GradientWidget();

        /**
         * @brief - Assign a new gradient for this widget. Will trigger a
         *          repaint operation in order to display the new gradient.
         * @param gradient - the new gradient to assign to this object.
         */
        void
        setGradient(core::engine::GradientShPtr gradient);

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

        /**
         * @brief - Performs the rendering of the texture to use to represent the
         *          gradient. Note that any existing texture is destroyed and that
         *          no checks are performed to verify that the repaint is needed.
         *          Assumes that the `m_propsLocker` is already locked.
         */
        void
        loadGradientTex();

        /**
         * @brief - Destroys the texture contained in the `m_tex` identifier if it is valid
         *          and invalidate it.
         *          Should typically be used when recreating the texture after a modification
         *          of the associated gradient.
         *          Assumes that the `m_propsLocker` is already locked.
         */
        void
        clearGradientTex();

        /**
         * @brief - Return `true` if the gradient has changed since the `m_tex` has been
         *          created. Typically allows to detect whether the texture should be
         *          recreated upon processing a repaint event or if we only need to draw
         *          it again.
         *          Assumes that the `m_propsLocker` is already locked.
         * @return - `true` if at least one of the rendering properties have been modified and
         *           `false` otherwise.
         */
        bool
        gradientTexChanged() const noexcept;

        /**
         * @brief - Marks the gradient's texture as dirty and indicate that it should be
         *          created again in the next call to the `drawContentPrivate` method.
         *          Assumes that the `m_propsLocker` is already locked.
         */
        void
        setGradientTexChanged() noexcept;

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
        core::engine::GradientShPtr m_gradient;

        /**
         * @brief - Used to determine whether the gradient has changed since it has
         *          been drawn for the last time. As long as this value is `false`
         *          the `m_tex` can be used as is, otherwise it should be recreated.
         */
        bool m_gradientChanged;

        /**
         * @brief - An identifier returned by the engine when creating the texture
         *          representing the gradient. This texture is assumed valid as long
         *          as the `m_gradientChanged` boolean is set to `false`.
         */
        utils::Uuid m_gradientTex;
    };

    using GradientWidgetShPtr = std::shared_ptr<GradientWidget>;
  }
}

# include "GradientWidget.hxx"

#endif    /* GRADIENT_WIDGET_HH */
