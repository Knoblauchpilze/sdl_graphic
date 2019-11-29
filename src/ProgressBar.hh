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

        ProgressBar(const std::string& name,
                    core::SdlWidget* parent = nullptr,
                    const core::engine::Color& color = core::engine::Color(),
                    const utils::Sizef& hint = utils::Sizef());

        virtual ~ProgressBar();

      protected:

      private:

        /**
         * @brief - Used to create the internal layout for this progress bar.
         */
        void
        build();

      private:

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this
         *          progress bar.
         */
        mutable std::mutex m_propsLocker;
    };

    using ProgressBarShPtr = std::shared_ptr<ProgressBar>;
  }
}

# include "ProgressBar.hxx"

#endif    /* PROGRESS_BAR_HH */
