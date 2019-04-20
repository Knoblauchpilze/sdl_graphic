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

        void
        drawContentPrivate(const utils::Uuid& uuid) const override;

      private:

        void
        loadPicture() const;

      private:

        std::string m_file;
        Mode m_mode;
        mutable utils::Uuid m_picture;
        mutable bool m_pictureDirty;

    };

    using PictureWidgetShPtr = std::shared_ptr<PictureWidget>;
  }
}

# include "PictureWidget.hxx"

#endif    /* PICTUREWIDGET_HH */
