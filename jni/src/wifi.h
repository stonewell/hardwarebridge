#ifndef __WIFI_H__
#define __WIFI_H__

int vendor_wifi_load_driver();
int vendor_wifi_unload_driver();
int vendor_wifi_start_supplicant();
int vendor_wifi_stop_supplicant();
int vendor_wifi_connect_to_supplicant();
int vendor_wifi_close_supplicant_connection();
int vendor_wifi_command(const char * cmd, char * reply, size_t * reply_len);
int vendor_wifi_wait_for_event(char *buf, size_t buflen);

#endif
