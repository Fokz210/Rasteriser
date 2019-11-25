#ifndef MOUSEHPP
#define MOUSEHPP

#ifdef __unix
#include <cassert>
extern "C" {
#include <fcntl.h>
#include <unistd.h>
}
#endif

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#endif 

#ifdef _WIN32
int SDL_MouseEventHandler (void * data, SDL_Event * event);
#endif

struct mouse {
	struct event {
#ifdef __unix
		char flags;
		char dx;
		char dy;

#endif
#ifdef _WIN32
		Uint8 button;
		int x, y, px, py;
		bool need_handle;
#endif
		bool right() const noexcept
		{
#ifdef __unix
			return (flags & 0x2) != 0;
#endif 
#ifdef _WIN32
			return button == SDL_BUTTON_RIGHT;
#endif
		}

		bool left() const noexcept
		{
#ifdef __unix
			return (flags & 0x1) != 0;
#endif 
#ifdef _WIN32
			return button == SDL_BUTTON_LEFT;
#endif
		}

		bool middle() const noexcept
		{
#ifdef __unix
			return (flags & 0x4) != 0;
#endif 
#ifdef _WIN32
			return button == SDL_BUTTON_MIDDLE;
#endif
		}

		int dx () const noexcept
		{
#ifdef __unix
			return dx;
#endif
#ifdef _WIN32
			return x - px;
#endif
		}

		int dy () const noexcept
		{
#ifdef __unix
			return dy;
#endif
#ifdef _WIN32
			return y - py;
#endif
		}

#ifdef __unix
		event () 
			: flags(),
			  dx(),
			  dy()
		{
		}
#endif
#ifdef _WIN32
		event ()
			: button (),
			x (),
			y (),
			px (),
			py (),
			need_handle ()
		{
			SDL_AddEventWatch (SDL_MouseEventHandler, reinterpret_cast<void *>(this));
		}
#endif 
	};

	mouse();
	~mouse();
	bool poll(event &e);

#ifdef __unix
	int mouseFD;
#endif
#ifdef _WIN32
	SDL_Event sdl_event;
#endif
};

#ifdef _WIN32
mouse::mouse ()
	: sdl_event ()
{
}
#endif
#ifdef __unix
mouse::mouse ()
	: mouseFD (::open ("/dev/input/mice", O_RDONLY | O_NONBLOCK))
{
}
#endif 


mouse::~mouse()
{
#ifdef __unix
	::close(mouseFD);
#endif
}

bool mouse::poll(event &e)
{

#ifdef __unix
	return ::read(mouseFD, &e, sizeof(event)) > 0;
#endif
#ifdef _WIN32
	SDL_WaitEvent (&sdl_event);
	if (!e.need_handle)
		return false;

	e.need_handle = false;
	return true;
#endif

}

#ifdef _WIN32
int SDL_MouseEventHandler (void * data, SDL_Event * event)
{
	mouse::event * event_ptr = reinterpret_cast <mouse::event *> (data);

	switch (event->type)
	{
	default:
		return 1;

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		event_ptr->button = event->button.button;
		event_ptr->need_handle = true;
		return 0;

	case SDL_MOUSEMOTION:
		event_ptr->px = event_ptr->x;
		event_ptr->py = event_ptr->y;
		event_ptr->x = event->motion.x;
		event_ptr->y = event->motion.y;
		event_ptr->need_handle = true;
		return 0;
	}
}

#endif

#endif //MOUSEHPP
