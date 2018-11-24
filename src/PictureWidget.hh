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
                         const SDL_Color& backgroundColor = SDL_Color{0, 0, 0, SDL_ALPHA_OPAQUE},
                         const sdl::core::Boxf& area = sdl::core::Boxf());

        virtual ~PictureWidget();

        void
        setImagePath(const std::string& path);

        void
        setMode(const Mode& mode);

        void
        clear();

      protected:

        void
        drawContentPrivate(SDL_Renderer* renderer, SDL_Texture* texture) const noexcept override;

      private:

        void
        loadPicture(SDL_Renderer* renderer) const;

      private:

        std::string m_file;
        Mode m_mode;
        mutable SDL_Texture* m_picture;
        mutable bool m_pictureDirty;

    };

    using PictureWidgetShPtr = std::shared_ptr<PictureWidget>;
  }
}

# include "PictureWidget.hxx"

#endif    /* PICTUREWIDGET_HH */
