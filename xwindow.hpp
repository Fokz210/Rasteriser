#pragma once
#include <stdexcept>
extern "C"
{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
}

#include "context.hpp"

class XWindow :
        public context
{
public:
    ~XWindow();
    XWindow();
    XWindow(XWindow const &) = delete;
    XWindow(XWindow &&) = delete;

    int     width()         const noexcept;
    int     height()        const noexcept;

    virtual void    update()              noexcept override;
    virtual void    clear()               noexcept override;

    color       *memory()                    noexcept;
    color       *operator[](int const)       noexcept;
    color const *operator[](int const) const noexcept;

private:
    Display    *display;
    XImage     *image;
    int         screen;
    Window      window;
    GC          gc;
};

inline void     XWindow::update() noexcept {XPutImage(display, window, gc, image, 0, 0, 0, 0, static_cast<unsigned>(_width), static_cast<unsigned>(_height));}

inline XWindow::XWindow()
{
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
    XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_fullscreen, 1);

    _width = XWidthOfScreen (ScreenOfDisplay(display, screen));
    _height = XHeightOfScreen(ScreenOfDisplay(display, screen));
    _fbc = static_cast<color *>(std::calloc(_width * _height, sizeof(color)));

    gc = XCreateGC(display, window, 0, 0);

    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
    image = XCreateImage
    (
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
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
}

inline XWindow::~XWindow()
{
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
