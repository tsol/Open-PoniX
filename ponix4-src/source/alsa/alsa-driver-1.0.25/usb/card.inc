#include "usbaudio.inc"
#ifdef OLD_USB
static void * usb_audio_probe(struct usb_device *dev, unsigned int ifnum,
                              const struct usb_device_id *id);
static void usb_audio_disconnect(struct usb_device *dev, void *ptr);
static int usb_reset_configuration(struct usb_device *dev);
#endif

