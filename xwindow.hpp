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
     virtual          ~XWindow()                               override;
                      XWindow();
                      XWindow(XWindow const &) = delete;
                      XWindow(XWindow &&)      = delete;

    virtual void      update()                 noexcept override;
    virtual void      clear()                  noexcept override;

            Display * getDisplay()             noexcept;

private:
    Display          *display;
    XImage           *image;
    int               screen;
    Window            window;
    GC                gc;
};

inline void  XWindow::update() noexcept {XPutImage(display, window, gc, image, 0, 0, 0, 0, static_cast<unsigned>(_width), static_cast<unsigned>(_height));}

inline XWindow::XWindow()
    : context()
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
}

inline XWindow::~XWindow()
{
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

inline void XWindow::clear() noexcept
{
    context::clear();
}

inline Display* XWindow::getDisplay () noexcept
{
    return display;
}
