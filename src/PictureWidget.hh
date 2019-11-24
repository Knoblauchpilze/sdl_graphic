#ifndef    PICTUREWIDGET_HH
# define   PICTUREWIDGET_HH

# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>
# include <sdl_engine/Image.hh>

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

        /**
         * @brief - Used to protect concurrent accesses to the internal data of this label
         *          widget.
         */
        mutable std::mutex m_propsLocker;

        /**
         * @brief - The display mode of the picture. Allows to precisely place the picture in
         *          the space available for this widget. Typical values include `Crop` and `Fit`.
         */
        Mode m_mode;

        /**
         * @brief - Holds the picture to use to represent this widget. This value is created
         *          right upon building this widget and is used to provide an indication of
         *          the expected size of this widget.
         */
        core::engine::ImageShPtr m_img;

        /**
         * @brief - Holds the identifier of the texture associated to this picture widget as
         *          provided by the engine. This identifier may be empty in case the picture
         *          element has not been repainted yet. Otherwise it represents an up-to-date
         *          version of the picture unless the `m_picChanged` value is `true`.
         */
        mutable utils::Uuid m_picture;

        /**
         * @brief - Holds the current status of the picture's identifier. This value indicates
         *          whether it's safe to use the `m_picture` value or a repaint operation should
         *          be performed before.
         */
        mutable bool m_picChanged;

    };

    using PictureWidgetShPtr = std::shared_ptr<PictureWidget>;
  }
}

# include "PictureWidget.hxx"

#endif    /* PICTUREWIDGET_HH */
