/*
 * Copyright (C) 2020 - 2026 Joerg Riechardt
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#endif
#include "picoboot_connection.h"
#include <string.h>

char model[8];
libusb_device_handle *dev_handle = NULL;
libusb_device *device = NULL;

void open_device(void)
{
	struct libusb_device *dev, **devs;
	struct libusb_config_descriptor *config;
	int ret;
	ret = libusb_get_device_list(NULL, &devs);
	if(ret < 0) {
		printf("error getting device list(): %s\n", libusb_error_name(ret));
		return;
	}

	for (int i=0; (dev=devs[i]) != NULL; i++) {
		struct libusb_device_descriptor desc;
		if(libusb_get_device_descriptor(dev, &desc) < 0) {
			printf("couldn't get device descriptor\n");
			continue;
		}

		/* Check for vendor ID */
		if (desc.idVendor != 0x1209)
			continue;

		/* Check for product ID */
		if (desc.idProduct == 0x4446) {
			sprintf(model, "%s", "RP2040");
		}
		else if (desc.idProduct == 0x000f) {
			sprintf(model, "%s", "RP2350");
		}
		if (desc.idProduct == 0x4446  || desc.idProduct == 0x000f) {
			device = dev;
			libusb_ref_device(device); // needed for Windows!
			printf("found %s at %d:%d\n", model, libusb_get_bus_number(dev), libusb_get_device_address(dev));
			break;
		}
	}

	libusb_free_device_list(devs, 1);

	if(libusb_get_active_config_descriptor(dev, &config) != LIBUSB_SUCCESS) {
		printf("couldn't get config descriptor\n");
		return;
	}

	ret = libusb_open(dev, &dev_handle);
	if(ret < 0) {
		printf("error opening device: %s\n", libusb_error_name(ret));
		goto error;
	}
	printf("opened device\n");
	fflush(stdout);
	if (config->bNumInterfaces == 1)
		interface = 0;
	else
		interface = 1;
	if (config->interface[interface].altsetting[0].bInterfaceClass == 0xff &&
	    config->interface[interface].altsetting[0].bNumEndpoints == 2) {
		out_ep = config->interface[interface].altsetting[0].endpoint[0].bEndpointAddress;
		in_ep = config->interface[interface].altsetting[0].endpoint[1].bEndpointAddress;
	}
	if (out_ep && in_ep && !(out_ep & 0x80u) && (in_ep & 0x80u)) {
		//printf("found interface\n");
		ret = libusb_claim_interface(dev_handle, interface);
		if (ret != LIBUSB_SUCCESS) {
			printf("error claiming m_interface: %s\n", libusb_error_name(ret));
			libusb_close(dev_handle);
			dev_handle = NULL;
			goto error;
		}
		//printf("claimed interface\n");
	}
error:
	libusb_free_config_descriptor(config);
}

int main(void)
{
	int ret;

	printf("\n=== Pico Reboot ===\n");

	ret = libusb_init(NULL);
	if(ret < 0) {
		printf("Error initializing libusb: %s\n", libusb_error_name(ret));
		return -1;
	}

	open_device();

	picoboot_reset(dev_handle);
	picoboot_exclusive_access(dev_handle, 1);

	if (!strcmp(model,"RP2040")) {
		picoboot_reboot(dev_handle, 0, 0, 500);
printf("2\n");
}
	else if (!strcmp(model,"RP2350")) {
		struct picoboot_reboot2_cmd cmd = {
		    .dFlags = 4, // FLASH_UPDATE ???
		    .dDelayMS = 500,
		    .dParam0 = 0x10000000, // FLASH_START
		    .dParam1 = 0,
		};
		picoboot_reboot2(dev_handle, &cmd);
	}
printf("3\n");
	picoboot_exclusive_access(dev_handle, 0);

	libusb_release_interface(dev_handle, 0);
	libusb_close(dev_handle);
	libusb_unref_device(device);
	libusb_exit(NULL);

	printf("=== Reboot successful ===\n");
	fflush(stdout);

	return 0;
}
