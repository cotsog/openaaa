#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libusb.h>

#if LIBUSBX_API_VERSION >= 0x01000102

static int count = 0;

int
hotplug_callback(struct libusb_context *ctx, 
                 struct libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
	static libusb_device_handle *handle = NULL;
	struct libusb_device_descriptor desc;
	int rc;

	libusb_get_device_descriptor(dev, &desc);
	if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
		rc = libusb_open(dev, &handle);
		if (LIBUSB_SUCCESS != rc) {
			printf("Could not open USB device\n");
		}
	} else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
		if (handle) {
			libusb_close(handle);
			handle = NULL;
		}
	} else {
		printf("Unhandled event %d\n", event);
	}
	count++;
	return 0;
}

int
main(void) 
{
	libusb_hotplug_callback_handle handle;
	int rc;

	libusb_init(NULL);
	rc = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
	                                            LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, 0x045a, 0x5005,
	                                            LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &handle);
	if (LIBUSB_SUCCESS != rc) {
		printf("Error creating a hotplug callback\n");
		libusb_exit(NULL);
		return EXIT_FAILURE;
	}
	while (count < 2) {
		usleep(10000);
	}

	//libusb_hotplug_deregister_callback(handle);
	libusb_exit(NULL);
	return 0;
}

#else

int
main(void)
{
	return 0;
}

#endif

