#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <libusb.h>

void list_all_devices();
void device_info(libusb_device* dev);