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

#define SRAM_END_RP2040      0x20042000
#define SRAM_END_RP2350      0x20082000

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
		if (desc.idVendor != 0x2e8a)
			continue;

		/* Check for product ID */
		if (desc.idProduct == 0x0003) {
			sprintf(model, "%s", "RP2040");
		}
		else if (desc.idProduct == 0x000f) {
			sprintf(model, "%s", "RP2350");
		}
		if (desc.idProduct == 0x0003  || desc.idProduct == 0x000f) {
			device = dev;
			libusb_ref_device(device); // needed for Windows!
			printf("found %s in boot mode at %d:%d\n", model, libusb_get_bus_number(dev), libusb_get_device_address(dev));
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

uint8_t * get_firmware(const char *firmwarefile, int *firmwareSize)
{
	FILE *fpFirmware;
	uint8_t *fw_buf;

	fpFirmware = fopen (firmwarefile, "rb");
	if(fpFirmware == NULL) {
		printf("error opening firmware file: %s\n",firmwarefile);
		return NULL;
	} else {
		printf("opened firmware file %s\n", firmwarefile);
	}

	if((fseek(fpFirmware, 0, SEEK_END) != 0) || ((*firmwareSize = ftell(fpFirmware)) < 0) ||
							(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
		printf("error determining firmware size\n");
		fclose(fpFirmware);
		return NULL;
	}

	fw_buf = (uint8_t*)malloc(*firmwareSize);
	if (fw_buf == NULL) {
		fclose(fpFirmware);
		printf("error allocating memory\n");
		return NULL;
	}

	if(fread(fw_buf,*firmwareSize,1,fpFirmware) != 1) {
		printf("read firmware error\n");
		fclose(fpFirmware);
		free(fw_buf);
		return NULL;
	} else {
		printf("read %d bytes of firmware\n", *firmwareSize);
	}

	fclose(fpFirmware);
	return fw_buf;
}

int main(int argc, const char **argv)
{
	int offset;
	int firmwareSize;
	uint8_t *fw_buf;
	int ret;
	uint32_t sram_end = 0;

	printf("\n=== Pico Firmware Upgrade ===\n");

	if(argc != 2){
		printf("Usage: picoflash firmware-file.bin\n");
		return 1;
	}

	if(!(fw_buf = get_firmware(argv[1], &firmwareSize)))
		return -1;

	uint8_t read_buf[firmwareSize];

	ret = libusb_init(NULL);
	if(ret < 0) {
		printf("Error initializing libusb: %s\n", libusb_error_name(ret));
		return -1;
	}

	open_device();

	picoboot_reset(dev_handle);
	picoboot_exclusive_access(dev_handle, 1);

	for(offset = 0; offset < firmwareSize; offset += 4096) {
		picoboot_exit_xip(dev_handle);
		picoboot_flash_erase(dev_handle, 0x10000000 + offset, 4096);
		picoboot_exit_xip(dev_handle);
		picoboot_write(dev_handle, 0x10000000 + offset, (uint8_t*)&fw_buf[offset], MIN(4096,firmwareSize - offset));
		picoboot_exit_xip(dev_handle);
		picoboot_read(dev_handle, 0x10000000 + offset, (uint8_t*)&read_buf[offset], MIN(4096,firmwareSize - offset));
		uint8_t length = MIN((offset+4096)*100/firmwareSize, 100) / 4;
		char progress[26] = "=========================";
		char prog[26] = "                         ";
		memcpy(prog, progress, length);
		printf("Progress: [%s] %d%%\r", prog, MIN((offset+4096)*100/firmwareSize, 100));
		fflush(stdout);
	}

	if (!memcmp(fw_buf, read_buf, firmwareSize))
		printf("\n=== verify successful ===\n");

	if (strcmp(model,"RP2040"))
		sram_end = SRAM_END_RP2040;
	else if (strcmp(model,"RP2350"))
		sram_end = SRAM_END_RP2350;

	picoboot_reboot(dev_handle, 0, sram_end, 500); // ...failed to send command -9 am Pico2
	picoboot_exclusive_access(dev_handle, 0);

	libusb_release_interface(dev_handle, 0);
	libusb_close(dev_handle);
	libusb_unref_device(device);
	libusb_exit(NULL);
	free(fw_buf);

	printf("=== Firmware Upgrade successful ===\n");
	fflush(stdout);

	return 0;
}
