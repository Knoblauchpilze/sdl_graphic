#ifndef    PICTUREWIDGET_HH
# define   PICTUREWIDGET_HH

# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class PictureWidget: public core::SdlWidget {
      public:

        enum class Mode {
          Crop,
          Fit
        };

      public:

        PictureWidget(const std::string& name,
                         const std::string& picture,
                         const Mode& mode = Mode::Crop,
                         SdlWidget* parent = nullptr,
                         const core::engine::Color& color = core::engine::Color(),
                         const utils::Sizef& area = utils::Sizef());

        virtual ~PictureWidget();

        void
        setImagePath(const std::string& path);

        void
        setMode(const Mode& mode);

        void
        clear();

      protected:

        /**
         * @brief - Reimplementation of the base `SdlWidget` method. A texture representing
         *          the picture associated to this widget widget will be drawn on the provided
         *          canvas. Only the specified part is updated by this function.
         * @param uuid - the identifier of the canvas which we can use to draw an image overlay.
         * @param area - the area of the canvas to update.
         */
        void
        drawContentPrivate(const utils::Uuid& uuid,
                           const utils::Boxf& area) override;

      private:

        void
        loadPicture() const;

        void
        clearPicture() const;

        bool
        pictureChanged() const noexcept;

        void
        setPictureChanged() const noexcept;

      private:

        std::string m_file;
        Mode m_mode;
        mutable utils::Uuid m_picture;
        mutable bool m_picChanged;

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this label widget.
         */
        mutable std::mutex m_propsLocker;

    };

    using PictureWidgetShPtr = std::shared_ptr<PictureWidget>;
  }
}

# include "PictureWidget.hxx"

#endif    /* PICTUREWIDGET_HH */
