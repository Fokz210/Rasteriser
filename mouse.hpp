#ifndef MOUSEHPP
#define MOUSEHPP

#include <cassert>
extern "C" {
#include <fcntl.h>
#include <unistd.h>
}

struct mouse {
	struct event {
		char flags;
		char dx;
		char dy;

		bool right()
		{
			return (flags & 0x2) != 0;
		}

		bool left()
		{
			return (flags & 0x1) != 0;
		}

		bool middle()
		{
			return (flags & 0x4) != 0;
		}
	};

	mouse();
	~mouse();
	bool poll(event &e);

	int mouseFD;
};

mouse::mouse()
	: mouseFD(::open("/dev/input/mice", O_RDONLY | O_NONBLOCK))
{
	assert(mouseFD != -1);
}

mouse::~mouse()
{
	::close(mouseFD);
}

bool mouse::poll(event &e)
{
	return ::read(mouseFD, &e, sizeof(event)) > 0;
}

#endif //MOUSEHPP
