#pragma once

#include <stdexcept>

#ifdef __unix
#include <X11/XKBlib.h>
extern "C"
{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
}
#endif 
#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#endif

#include "context.hpp"

#ifdef _WIN32
int SDL_QuitHandler (void * data, SDL_Event * event);
#endif

class WindowContext :
        public context
{
public:
     virtual          ~WindowContext()                               override;
                      WindowContext();
                      WindowContext(WindowContext const &) = delete;
                      WindowContext(WindowContext &&)      = delete;

    virtual void      update()                 noexcept override;
    virtual void      clear()                  noexcept override;

	void			  close ()				   noexcept;
	bool			  is_open ()			   const noexcept;

private:
#ifdef __unix
    Display          *display;
    XImage           *image;
    int               screen;
    Window            window;
    GC                gc;
#endif
#ifdef _WIN32
	SDL_Window * sdl_window;
	SDL_Texture * sdl_texture;
	SDL_Renderer * sdl_renderer;
	bool open;
#endif
};

inline void  WindowContext::update() noexcept
{
#ifdef __unix
	XPutImage(display, window, gc, image, 0, 0, 0, 0, static_cast<unsigned>(_width), static_cast<unsigned>(_height));
#endif
#ifdef _WIN32
	SDL_UpdateTexture (sdl_texture, nullptr, static_cast<void *>(_fbc), _width * sizeof (color));
	SDL_RenderClear (sdl_renderer);
	SDL_RenderCopy (sdl_renderer, sdl_texture, nullptr, nullptr);
	SDL_RenderPresent (sdl_renderer);
#endif
}

inline WindowContext::WindowContext()
	: context(),
	open (true)
{
#ifdef __unix
    display = XOpenDisplay(getenv("DISPLAY"));
    if(display == nullptr)
        throw std::runtime_error("could not open x11 display");
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow
        (
            display,
            DefaultRootWindow(display),
            0,
            0,
            640,
            480,
            0,
            WhitePixel(display, screen),
            BlackPixel(display, screen)
        );
    Atom wm_state      = XInternAtom(display, "_NET_WM_STATE",            true);
    Atom wm_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", true);
    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&wm_fullscreen), 1);

    _width = static_cast<unsigned>(XWidthOfScreen (ScreenOfDisplay(display, screen)));
    _height = static_cast<unsigned>(XHeightOfScreen(ScreenOfDisplay(display, screen)));
    _fbc = static_cast<color *>(std::calloc(static_cast<size_t>(_width * _height), sizeof(color)));

    gc = XCreateGC(display, window, 0, nullptr);

    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
    image = XCreateImage
    (
        display,
        DefaultVisual(display, screen),
        static_cast<unsigned> (DefaultDepth(display, screen)),
        ZPixmap,
        0,
        reinterpret_cast<char *>(_fbc),
        _width,
        _height,
        32,
        0
    );
    XClearWindow(display, window);
    XMapRaised(display, window);

    Cursor invisibleCursor;
    Pixmap bitmapNoData;
    XColor black;
    static char noData[] = { 0,0,0,0,0,0,0,0 };
    black.red = black.green = black.blue = 0;

    bitmapNoData = XCreateBitmapFromData(display, window, noData, 8, 8);
    invisibleCursor = XCreatePixmapCursor(display, bitmapNoData, bitmapNoData,
                                         &black, &black, 0, 0);
    XDefineCursor(display,window, invisibleCursor);
    XFreeCursor(display, invisibleCursor);
    XFreePixmap(display, bitmapNoData);
#endif
#ifdef _WIN32
	sdl_window = SDL_CreateWindow ("rasteriser", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height, SDL_WINDOW_SHOWN);
	sdl_renderer = SDL_CreateRenderer (sdl_window, -1, SDL_RENDERER_ACCELERATED);
	sdl_texture = SDL_CreateTexture (sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _width, _height);
	//SDL_AddEventWatch (SDL_QuitHandler, reinterpret_cast<void *>(&open));
#endif
}

inline WindowContext::~WindowContext()
{
#ifdef __unix
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
#endif
#ifdef _WIN32
	SDL_DestroyWindow (sdl_window);
	SDL_DestroyRenderer (sdl_renderer);
	SDL_DestroyTexture (sdl_texture);
#endif
	delete[] _fbc;
}

inline void WindowContext::clear() noexcept
{
    context::clear();
}

inline void WindowContext::close () noexcept
{
    open = false;
}

inline bool WindowContext::is_open () const noexcept
{
	return open;
}

#ifdef _WIN32
int SDL_QuitHandler (void * data, SDL_Event * event)
{
	if (event->type == SDL_QUIT)
	{
		*reinterpret_cast <bool *> (data) = true;
		return 0;
	}

	return 1;
}
#endif
