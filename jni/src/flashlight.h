#ifndef __FLASH_LIGHT_H__
#define __FLASH_LIGHT_H__

int vendor_get_flashlight_enabled();
int vendor_set_flashlight_enabled(int on);
int vendor_enable_camera_flash(int milliseconds);


#endif
