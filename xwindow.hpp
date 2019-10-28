#pragma once
#include <stdexcept>
extern "C"
{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
}
class XWindow
{
public:
    ~XWindow();
    XWindow();
    XWindow(XWindow const &) = delete;
    XWindow(XWindow &&) = delete;

    struct color
    {
        unsigned char b, g, r, a;
    };

    int     width()         const noexcept;
    int     height()        const noexcept;

    void    update()              noexcept;
    void    clear()               noexcept;

    color       *memory()                    noexcept;
    color       *operator[](int const)       noexcept;
    color const *operator[](int const) const noexcept;

private:
    Display    *display;
    int         screen;
    Window      window;
    int         w;
    int         h;
    GC          gc;
    XImage     *image;
    color      *pixels;
};

inline int      XWindow::width() const noexcept {return w;}
inline int      XWindow::height() const noexcept {return h;}
inline void     XWindow::update() noexcept {XPutImage(display, window, gc, image, 0, 0, 0, 0, w, h);}
inline void     XWindow::clear() noexcept {std::wmemset(reinterpret_cast<wchar_t *>(pixels), 0xff000000, w * h);}
inline typename XWindow::color *XWindow::memory() noexcept {return pixels;}
inline typename XWindow::color *XWindow::operator[](int const i) noexcept {return pixels + (h - i - 1) * w;}
inline typename XWindow::color const *XWindow::operator[](int const i) const noexcept {return pixels + (h - i - 1) * w;}

inline XWindow::XWindow()
{
    display = XOpenDisplay(getenv("DISPLAY"));
    if(display == NULL)
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

    w = XWidthOfScreen (ScreenOfDisplay(display, screen));
    h = XHeightOfScreen(ScreenOfDisplay(display, screen));
    pixels = static_cast<color *>(std::calloc(w * h, sizeof(color)));

    gc = XCreateGC(display, window, 0, 0);

    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
    image = XCreateImage
    (
        display,
        DefaultVisual(display, screen),
        DefaultDepth(display, screen),
        ZPixmap,
        0,
        reinterpret_cast<char *>(pixels),
        w,
        h,
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
