#include <sys/io.h>

#include <linux/kernel.h>

#include <assert.h>
#include <string.h>

#define KEYBOARD_IRQ 0x01
#define KEYBOARD_DATA_REG 0x60
#define KEYBOARD_SCANCODE 0x7F
#define KEYBOARD_STATUS 0x80

#define KEYS_SIZE 256

static int keys_pressed[KEYS_SIZE];
static struct mutex keys_pressed_lock;

static inline void init_keys_map(void)
{
	memset(keys_pressed, 0, sizeof(keys_pressed));
}

static irqreturn_t keyboard_interrupt_handler(int irq, void *device_id)
{
	char scancode;
	unsigned key;
	unsigned state;

	scancode = inb(KEYBOARD_DATA_REG);
	key = scancode & KEYBOARD_SCANCODE;
	state = scancode & KEYBOARD_STATUS;

	if (key >= KEYS_SIZE) {
		printk("%d scancode is out of bounds", scancode);
	} else {
		mutex_lock(&keys_pressed_lock);
		keys_pressed[key] = state ? 1 : 0;
		mutex_unlock(&keys_pressed_lock);
	}

	return IRQ_HANDLED;
}

static inline void keyboard_interrupt_handler_init(void)
{
	return request_irq(KEYBOARD_IRQ, keyboard_interrupt_handler,
			   IRQF_SHARED, "keyboard_interrupt_handler",
			   (void *)keyboard_interrupt_handler);
}

static inline void keyboard_interrupt_handler_exit(void)
{
	return free_irq(KEYBOARD_IRQ, (void *)keyboard_interrupt_handler);
}

static inline void init_my_library(void)
{
	mutex_init(&keys_pressed_lock);
	keyboard_interrupt_handler_init();
}

static inline int key_status(int id)
{
	int status;

	assert(id < KEYS_SIZE);

	mutex_lock(&keys_pressed_lock);
	status = keys_pressed[id];
	mutex_unlock(&keys_pressed_lock);

	return status;
}

static inline void free_my_library(void)
{
	keyboard_interrupt_handler_exit();
}
