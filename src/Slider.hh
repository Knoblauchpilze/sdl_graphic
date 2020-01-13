#ifndef    SLIDER_HH
# define   SLIDER_HH

# include <memory>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class Slider: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a new slider witht he specified range and value.
         * @param name - the name of `this` slider.
         * @param value - the initial value of the slider.
         * @param range - the range of the slider. Note that in case the `value`
         *                is not consistent with the range it is clamped to the
         *                closest bound.
         * @param steps - the number of steps to create between both bounds. If
         *                the value does not lie exactly on such a step it is
         *                rounded to be on a whole step.
         * @param font - the name of the font to use to render the current
         *               value selected.
         * @param size - the size of the font to set for the label indicating
         *               the currently selected value.
         * @param parent - a pointer to the parent widget for this textbox.
         * @param area - the size hint for this text box.
         */
        Slider(const std::string& name,
               float value,
               const utils::Vector2f& range,
               int steps,
               const std::string& font,
               unsigned size = 15,
               SdlWidget* parent = nullptr,
               const utils::Sizef& area = utils::Sizef());

        virtual ~Slider();

      protected:

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide
         *          specific behavior upon detecting a key event.
         *          We want to make the slider move when some specified key is
         *          pressed.
         * @param e - the key event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        keyPressEvent(const core::engine::KeyEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` in order to provide
         *          custom behavior when the user clicks on the slider. We want to
         *          move the cursor one step in the direction of the click (i.e. if
         *          the mouse is between the slider and the end of the range it will
         *          be moved forward and backward otherwise.
         * @param e - the event to be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseButtonReleaseEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `EngineObject` method to provide
         *          specific behavior when the user double click on the slider. We
         *          will use a similar behavior to what happen on a click but will
         *          send the slider all the way to the bound.
         * @param e - the double click event which should be handled.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDoubleClickEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Specialization of the base `EngineObject` method to provide
         *          custom behavior upon dragging the mouse. We will handle the
         *          progressive displacement of the slider to follow the mouse.
         * @param e - the drag event to process.
         * @return - `true` if the event was recognized, `false` otherwise.
         */
        bool
        mouseDragEvent(const core::engine::MouseEvent& e) override;

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. Several textures
         *          are used to represent the slider and we should update them upon
         *          calling this method so that it is always up-to-date with the value
         *          selected. Only the specified part is updated by this function.
         * @param uuid - the identifier of the canvas which we can use to draw the
         *               overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

      private:

        /**
         * @brief - Used internally upon constructing the slider to initialize internal
         *          states.
         */
        void
        build();

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this slider.
         */
        std::mutex m_propsLocker;
    };

    using SliderShPtr = std::shared_ptr<Slider>;
  }
}

# include "Slider.hxx"

#endif    /* SLIDER_HH */
