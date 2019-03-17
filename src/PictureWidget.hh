#ifndef    PICTUREWIDGET_HH
# define   PICTUREWIDGET_HH

# include <memory>
# include <string>
# include <sdl_core/SdlWidget.hh>

namespace sdl {
  namespace graphic {

    class PictureWidget: public sdl::core::SdlWidget {
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
                         const bool transparent = false,
                         const core::engine::Palette& palette = core::engine::Palette(),
                         const utils::Sizef& area = utils::Sizef());

        virtual ~PictureWidget();

        void
        setImagePath(const std::string& path);

        void
        setMode(const Mode& mode);

        void
        clear();

      protected:

        void
        drawContentPrivate(const core::engine::Texture::UUID& uuid) const noexcept override;

      private:

        void
        loadPicture() const;

      private:

        std::string m_file;
        Mode m_mode;
        mutable std::shared_ptr<core::engine::Texture::UUID> m_picture;
        mutable bool m_pictureDirty;

    };

    using PictureWidgetShPtr = std::shared_ptr<PictureWidget>;
  }
}

# include "PictureWidget.hxx"

#endif    /* PICTUREWIDGET_HH */
