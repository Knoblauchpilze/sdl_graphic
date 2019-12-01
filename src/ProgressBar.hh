#ifndef    PROGRESS_BAR_HH
# define   PROGRESS_BAR_HH

# include <mutex>
# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class ProgressBar: public core::SdlWidget {
      public:

        /**
         * @brief - Creates a progress bar with the specified name and parent.
         *          The default color for the gradient is set to range from `red`
         *          to `green` but can be updated if needed.
         *          In the part of the widget representing the `not-loaded` section
         *          a white color is used. The progress bar is represented into a
         *          light grey frame.
         * @param name - the name of the progress bar.
         * @param parent - the parent of this widget.
         * @param hint - the size hint of this item.
         */
        ProgressBar(const std::string& name,
                    core::SdlWidget* parent = nullptr,
                    const utils::Sizef& hint = utils::Sizef());

        /**
         * @brief - Destruction of the object.
         */
        virtual ~ProgressBar();

        /**
         * @brief - Used to assign a new completion percentage for this progress bar.
         *          The percentage is clamped in the valid range `[0; 1]`.
         *          A repaint event is triggered if the value is effectively different
         *          from the one kept internally.
         * @param value - the new completion percentage.
         */
        void
        setCompletion(float value);

      protected:

        /**
         * @brief - Reimplementation of the base class method to provide update of the
         *          size of the mask as it is not managed by the layout itself.
         * @param window - the available size to perform the update.
         */
        void
        updatePrivate(const utils::Boxf& window) override;

      private:

        /**
         * @brief - Used to retrieve the default dimensions of the frame surrounding
         *          the gradient display representing this progress bar.
         *          Internally it is used as a margin parameter for the layout.
         * @return - a value used to define the dimensions of the frame surrounding
         *           this progress bar.
         */
        static
        float
        getFrameDimensions() noexcept;

        /**
         * @brief - Used to retrieve the name to use to describe the masking element.
         * @return - a name to use to describe the masking element.
         */
        static
        const char*
        getMaskName() noexcept;

        /**
         * @brief - Used to build the layout of this widget.
         */
        void
        build();

        /**
         * @brief - Compute and assign a valid size to the mask element used by this
         *          progress bar to simulate some progress. The current size of the
         *          widget is used.
         *          Assumes that the locker is already acquired.
         */
        void
        updateMaskSizeFromCompletion();

        /**
         * @brief - Used to retrieve the mask widget used to fake the loading progress.
         *          The return value is always valid if the method returns (i.e. an error
         *          is raised if the mask cannot be retrieved).
         * @return - the masking element.
         */
        core::SdlWidget*
        getMask();

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this
         *          progress bar.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - Holds the current completion percentage for this progress bar. Used
         *          to determine whether a new call to `setCompletion` should trigger a
         *          repaint.
         */
        float m_completion;
    };

    using ProgressBarShPtr = std::shared_ptr<ProgressBar>;
  }
}

# include "ProgressBar.hxx"

#endif    /* PROGRESS_BAR_HH */
