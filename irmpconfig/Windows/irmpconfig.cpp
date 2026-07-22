/**********************************************************************************************************
	irmpconfig: configure and monitor IRMP Pico

	Copyright (C) 2014-2026 Joerg Riechardt

	based on work by Alan Ott
	Copyright 2010  Alan Ott

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

***********************************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <stdint.h>
#include <inttypes.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#endif
#include "usb_hid_keys.h"
#include <time.h>

enum access {
	ACC_GET,
	ACC_SET,
	ACC_RESET
};

enum command {
	CMD_CAPS,
	CMD_ALARM,
	CMD_IRDATA,
	CMD_KEY,
	CMD_WAKE,
	CMD_REBOOT,
	CMD_IRDATA_REMOTE,
	CMD_WAKE_REMOTE,
	CMD_REPEAT,
	CMD_EEPROM_RESET,
	CMD_EEPROM_COMMIT,
	CMD_EEPROM_GET_RAW,
	CMD_HID_TEST,
	CMD_STATUSLED,
	CMD_EMIT,
	CMD_NEOPIXEL,
	CMD_MACRO,
	CMD_MACRO_REMOTE,
	CMD_SEND_AFTER_WAKEUP,
	CMD_EEPROM_DIRTY,
};

enum status {
	STAT_CMD,
	STAT_SUCCESS,
	STAT_FAILURE
};

enum report_id {
	REPORT_ID_IR = 1,
	REPORT_ID_CONFIG_IN = 2,
	REPORT_ID_CONFIG_OUT = 3,
	REPORT_ID_KBD = 4
};

enum color {
	red,
	green,
	blue,
	yellow,
	white,
	off,
	custom,
	strong_red,
	orange,
	purple,
	strong_white
};

#define IRMP_FLAG_NEW                   0x00
#define IRMP_FLAG_REPETITION            0x01
#define IRMP_FLAG_RELEASE               0x02

#define NUM_PIXELS 64

hid_device *handle;
uint8_t inBuf[64];
uint8_t outBuf[64];
unsigned int in_size = 64, out_size = 64;
char error[6] = "error";

static inline uint32_t GetUsTicks(void)
{
#ifdef CLOCK_MONOTONIC
    struct timespec tspec;
    clock_gettime(CLOCK_MONOTONIC, &tspec);
    return (tspec.tv_sec * 1000 * 1000) + (tspec.tv_nsec / 1000);
#else
    struct timeval tval;
    if (gettimeofday(&tval, NULL) < 0) {
	return 0;
    }
    return (tval.tv_sec * 1000 * 1000) + (tval.tv_usec);
#endif
}

static bool open_irmp() {
	struct hid_device_info *devices, *cur_dev;
	// Open the device using the VID, PID.
	devices = hid_enumerate(0x1209, 0x4446);
	cur_dev = devices;
	while (cur_dev) {
		// select the hidraw device, not the keyboard device
		if(cur_dev->usage == 0x01) {
			break;
		}
		cur_dev = cur_dev->next;
	}
	if (!cur_dev)
		return -1;
	handle =  hid_open_path(cur_dev->path);
	if (!handle) {
		printf("error opening irmp device\n");
		return false;
	}
	printf("opened irmp device\n");
	return true;
}

static void read_irmp(int in_size, int show_len) {
	int retVal;
	retVal = hid_read(handle, inBuf, in_size);
	if (retVal < 0) {
		printf("read error\n");
	} else {
		printf("read %d bytes:\n\t", retVal);
		for (int i = 0; i < show_len; i++)
			printf("%02x ", (unsigned int)inBuf[i]);
		puts("\n");
	}
} 

static void write_irmp(int idx) {
	int retVal;
	retVal = hid_write(handle, outBuf, idx);
	if (retVal < 0) {
		printf("write error: %ls\n", hid_error(handle));
	} else {
		printf("written %d bytes:\n\t", retVal);
		for (int i = 0; i < idx; i++)
			printf("%02x ", (unsigned int)outBuf[i]);
		puts("\n");
	}
}

void write_and_check(int idx, int show_len) {
	write_irmp(idx);
	#ifdef WIN32
	Sleep(3);
	#else
	usleep(3000);
	#endif
	read_irmp(in_size, show_len); // blocking per default, waits until data arrive
	while (inBuf[0] == REPORT_ID_KBD || inBuf[0] == REPORT_ID_IR)
		read_irmp(in_size, show_len);
	if((inBuf[0] == REPORT_ID_CONFIG_IN) && (inBuf[1] == STAT_SUCCESS) && (inBuf[2] == outBuf[2]) && (inBuf[3] == outBuf[3])) {
		puts("*****************************OK********************************\n");
	} else {
		puts("***************************ERROR*******************************\n");
	}
}

char* get_key_from_hex(uint8_t hex) {
  for(int i = 0; i < lines; i++) {
    if (hex == mapusb[i].usb_hid_key)
      return mapusb[i].key;
  }
  return error;
}

char* get_modifier_from_hex(uint8_t hex) {
  for(int i = 0; i < 10; i++) {
    if (hex == modifier[i].usb_hid_key)
      return modifier[i].key;
  }
  return error;
}

int main(int argc, char* argv[])
{
	uint64_t i;
	uint16_t kk = 0x0000;
	char c, d, e;
	uint8_t l, idx = 3, eeprom_lines;
	int8_t k;
	unsigned int s, m;
	int retValm, jump_to_firmware;
	FILE *fp;
	char testfilename[10];
	uint16_t j = 0;
	uint16_t pc_rate[256] = {0};
	uint16_t uc_rate[256] = {0};
	uint32_t now_us;
	uint32_t last_us;
	uint32_t diff_us;
	uint8_t rrBuf[12];
	uint8_t first_time = 1;
	int count = 0;

#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif

	if (hid_init())
		return -1;

	open_irmp();

	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;
	outBuf[2] = ACC_GET;
	goto caps;

cont:	printf("set: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm, commit, statusled and neopixel(s)\nset by remote: wakeups, macros and IR-data (q)\nget: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm, capabilities, eeprom, raw eeprom and dirty eeprom from RP2xxx (g)\nreset: wakeups, macros, IR-data, keys, repeat, send_after_wakeup, alarm and eeprom (r)\nsend IR (i)\nreboot (b)\nmonitor until ^C (m)\nrepeat rate statistics until ^C (y)\nrun test (t)\nhid test (h)\nneopixel test (n)\nrun test2 (u)\nexit (x)\n");
	scanf("%s", &c);

	switch (c) {

	case 's':
set:		printf("set wakeup(w)\nset macro(m)\nset IR-data(i)\nset key(k)\nset repeat(r)\nset send_after_wakeup(x)\nset alarm(a)\ncommit on RP2xxx(c)\nstatusled(s)\nneopixel(n)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = m;
			printf("enter IRData (protocoladdresscommandflag)\n");
			scanf("%llx", &i);
			outBuf[idx++] = (i>>40) & 0xFF;
			outBuf[idx++] = (i>>24) & 0xFF;
			outBuf[idx++] = (i>>32) & 0xFF;
			outBuf[idx++] = (i>>8) & 0xFF;
			outBuf[idx++] = (i>>16) & 0xFF;
			outBuf[idx++] = i & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
			printf("enter key (KEY_xxx)\n");
			scanf("%s", &c);
			for(l=0; l < lines; l++) {
			    if(!strcmp(mapusb[l].key, &c)) {
				kk = mapusb[l].usb_hid_key;
				break;
			    }
			}
			outBuf[idx++] = kk;
			printf("enter modifier (KEY_xxx or ff for none)\n");
			scanf("%s", &c);
			for(l=0; l < lines; l++) {
			    if(!strcmp(mapusb[l].key, &c)) {
				kk = mapusb[l].usb_hid_key;
				break;
			    }
			}
			outBuf[idx++] = kk;
			write_and_check(idx, 4);
			break;
		case 'r':
			printf("set repeat delay(0)\nset repeat period(1)\nset repeat timeout(2)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
			printf("enter value (dec)\n");
			scanf("%hu", &kk);
			outBuf[idx++] = kk & 0xFF;
			outBuf[idx++] = (kk>>8) & 0xFF;
			write_and_check(idx, 4);
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			printf("enter send_after_delay (dec)\n");
			scanf("%" SCNu8 "", &l);
			outBuf[idx++] = l;
			write_and_check(idx, 4);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			printf("enter alarm\n");
			scanf("%llx", &i);
			memcpy(&outBuf[idx], &i, 4);
			write_and_check(idx + 4, 4);
			break;
		case 'c':
			outBuf[idx++] = CMD_EEPROM_COMMIT;
			write_and_check(idx, 4);
			break;
		case 's':
			outBuf[idx++] = CMD_STATUSLED;
			printf("enter 1 for on, 0 for off\n");
			scanf("%u", &s);
			outBuf[idx++] = s;
			write_and_check(idx, 4);
			break;
		case 'n':
			outBuf[idx++] = CMD_NEOPIXEL;
			printf("enter led number (starting with 1)\n");
			scanf("%u", &s);
			outBuf[idx++] = 3 * s;
			outBuf[idx++] = (s - 1) / 19;
			outBuf[idx++] = 3 * ((s - 1) % 19) + 1;
			idx += 3 * ((s - 1) % 19);
color: printf("red(r)\ngreen(g)\nblue(b)\nyellow(y)\nwhite(w)\noff(o)\ncustom(c)\nstrong_red(s)\norange(a)\npurple(p)\nstrong_white(x)\n");
			scanf("%s", &e);
			switch (e) {
			case 'r':
				outBuf[idx++] = 3;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'g':
				outBuf[idx++] = 0;
				outBuf[idx++] = 4;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'b':
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				outBuf[idx++] = 12;
				write_and_check(idx, 4);
				break;
			case 'y':
				outBuf[idx++] = 4;
				outBuf[idx++] = 2;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'w':
				outBuf[idx++] = 3;
				outBuf[idx++] = 3;
				outBuf[idx++] = 2;
				write_and_check(idx, 4);
				break;
			case 'o':
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'c':
				printf("enter red in dec\n");
				scanf("%u", &s);
				outBuf[idx++] = s;
				printf("enter green in dec\n");
				scanf("%u", &s);
				outBuf[idx++] = s;
				printf("enter blue in dec\n");
				scanf("%u", &s);
				outBuf[idx++] = s;
				write_and_check(idx, 4);
				break;
			case 's':
				outBuf[idx++] = 255;
				outBuf[idx++] = 0;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'a':
				outBuf[idx++] = 8;
				outBuf[idx++] = 2;
				outBuf[idx++] = 0;
				write_and_check(idx, 4);
				break;
			case 'p':
				outBuf[idx++] = 8;
				outBuf[idx++] = 0;
				outBuf[idx++] = 8;
				write_and_check(idx, 4);
				break;
			case 'x':
				outBuf[idx++] = 255;
				outBuf[idx++] = 255;
				outBuf[idx++] = 255;
				write_and_check(idx, 4);
				break;
			default:
				goto color;
			}
			break;
		default:
			goto set;
		}
		break;

	case 'q':
Set:		printf("set wakeup with remote control(w)\nset macro with remote control(m)\nset IR-data with remote control(i)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE_REMOTE;
			outBuf[idx++] = s;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO_REMOTE;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA_REMOTE;
			outBuf[idx++] = s;
			break;
		default:
			goto Set;
		}
		write_and_check(idx, 4);
		break;

	case 'g':
get:		printf("get wakeup(w)\nget macro(m)\nget IR-data (i)\nget key(k)\nget repeat(r)\nget send_after_weakeup(x)\nget caps(c)\nget alarm(a)\nget eeprom(e)\nget raw eeprom from RP2xxx(p)\nget dirty eeprom from RP2xxx(d)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_GET;
		switch (d) {
		case 'w':
			printf("enter wakeup number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			write_and_check(idx, 10);
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			write_and_check(idx, 10);
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = s;
			write_and_check(idx, 10);
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
			write_and_check(idx, 6);
			break;
		case 'r':
			printf("get repeat delay(0)\nget repeat period(1)\nget repeat timeout(2)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
			write_and_check(idx, 6);
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			write_and_check(idx, 5);
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			write_and_check(idx, 8);
			break;
		case 'c':
caps:			jump_to_firmware = 0;
			outBuf[idx++] = CMD_CAPS;
			for (l = 0; l < 20; l++) { // for safety stop after 20 loops
				outBuf[idx] = l;
				write_irmp(idx+1);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				read_irmp(in_size, l == 0 ? 9 : in_size);
				while (inBuf[0] == REPORT_ID_KBD || inBuf[0] == REPORT_ID_IR)
					read_irmp(in_size, l == 0 ? 9 : in_size);
				if (!l) { // first query for slots and depth
					printf("number of keys: %u\n", inBuf[4]);
					printf("number of wakeups (including reboot): %u\n", inBuf[6]);
					printf("hid in report count: %u\n", inBuf[7]);
					printf("hid out report count: %u\n", inBuf[8]);
					printf("number of macros: %u\n", inBuf[9]);
					printf("macro depth: %u\n", inBuf[10]);
					eeprom_lines = inBuf[4];
				} else {
					if(!jump_to_firmware) { // queries for supported_protocols
						printf("protocols: ");
						for (s = 4; s < in_size; s++) {
							if (!inBuf[s]) { // NULL termination
								printf("\n\n");
								jump_to_firmware = 1;
								goto again;
							}
							printf("%u ", inBuf[s]);
						}
					} else { // queries for firmware
						printf("firmware: ");
						for (s = 4; s < in_size; s++) {
							if (!inBuf[s]) { // NULL termination
								printf("\n\n");
								goto out;
							}
							printf("%c", inBuf[s]);
						}
					}
				}
				printf("\n\n");
again:			;
			}
			break;
		case 'e':
			for (l = 0; l < eeprom_lines; l++) {
				outBuf[idx++] = CMD_IRDATA;
				outBuf[idx++] = l;
				hid_write(handle, outBuf, idx);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_read(handle, inBuf, 10);
				printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx", inBuf[4],inBuf[6],inBuf[5],inBuf[8],inBuf[7],inBuf[9]);
				printf(" ");
				idx = 3;
				outBuf[idx++] = CMD_KEY;
				outBuf[idx++] = l;
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_write(handle, outBuf, idx);
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				hid_read(handle, inBuf, 6);
				for(int n=0; n < lines-1; n++) {
					if(mapusb[n].usb_hid_key == inBuf[5]) {
						printf("%s|", mapusb[n].key);
					}
				}
				for(int n=0; n < lines-1; n++) {
					if(mapusb[n].usb_hid_key == inBuf[4]) {
						printf("%s", mapusb[n].key);
					}
				}
				printf("\n");
				#ifdef WIN32
				Sleep(3);
				#else
				usleep(3000);
				#endif
				idx = 3;
			}
			goto out;
			break;
		case 'p':
			outBuf[idx++] = CMD_EEPROM_GET_RAW;
			for(k = 31; k >= 0; k--) { // FLASH_SECTOR_SIZE * nr_sectors / size
				outBuf[idx] = k;
				for(l = 0; l < 32; l++) { // size / 32
					outBuf[idx+1] = l;
					hid_write(handle, outBuf, idx+2);
					#ifdef WIN32
					Sleep(3);
					#else
					usleep(3000);
					#endif
					retValm = hid_read(handle, inBuf, in_size);
					if (retValm < 0) {
						printf("read error\n");
					} else {
						for (int i = 4; i < 36; i++) // 32
							printf("%02x ", inBuf[i]);
					}
				}
				printf("\n");
			}
			break;
		case 'd':
			outBuf[idx++] = CMD_EEPROM_DIRTY;
			write_and_check(idx, 5);
			break;
		default:
			goto get;
		}
out:
		break;

	case 'r':
reset:		printf("reset wakeup(w)\nreset macro slot(m)\nreset IR-data(i)\nreset key(k)\nreset repeat(r)\nreset send_after_wakeup(x)\nreset alarm(a)\nreset eeprom(e)\n");
		scanf("%s", &d);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_RESET;
		switch (d) {
		case 'w':
			printf("enter slot number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_WAKE;
			outBuf[idx++] = s;
			break;
		case 'm':
			printf("enter macro number (starting with 0)\n");
			scanf("%u", &m);
			outBuf[idx++] = CMD_MACRO;
			outBuf[idx++] = m;    // (m+1)-th macro
			printf("enter slot number, 0 for trigger\n");
			scanf("%u", &s);
			outBuf[idx++] = s;    // (s+1)-th slot
			break;
		case 'i':
			printf("enter IR-data number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_IRDATA;
			outBuf[idx++] = s;
			break;
		case 'k':
			printf("enter key number (starting with 0)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_KEY;
			outBuf[idx++] = s;
			break;
		case 'r':
			printf("reset repeat delay(0)\nreset repeat period(1)\nreset repeat timeout(2)\n");
			scanf("%u", &s);
			outBuf[idx++] = CMD_REPEAT;
			outBuf[idx++] = s;
			break;
		case 'x':
			outBuf[idx++] = CMD_SEND_AFTER_WAKEUP;
			break;
		case 'e':
			outBuf[idx++] = CMD_EEPROM_RESET;
			break;
		case 'a':
			outBuf[idx++] = CMD_ALARM;
			break;
		default:
			goto reset;
		}
		write_and_check(idx, 4);
		break;

	case 'i':
		printf("enter IRData (protocoladdresscommandflag)\n");
		scanf("%llx", &i);
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_EMIT;
		outBuf[idx++] = (i>>40) & 0xFF;
		outBuf[idx++] = (i>>24) & 0xFF;
		outBuf[idx++] = (i>>32) & 0xFF;
		outBuf[idx++] = (i>>8) & 0xFF;
		outBuf[idx++] = (i>>16) & 0xFF;
		outBuf[idx++] = i & 0xFF;
		write_and_check(idx, 4);
		break;

	case 'b':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_REBOOT;
		write_and_check(idx, 4);
		hid_close(handle);
		break;

	case 'm':
		goto monit;
		break;

	case 'y':
		goto rate;
		break;

	case 'n':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		idx = 2;
		outBuf[idx++] = ACC_SET;
		outBuf[idx++] = CMD_NEOPIXEL;
		outBuf[idx++] = NUM_PIXELS * 3;
		for (m = 0; m < 3; m++) {
			for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
				idx = 5;
				outBuf[idx++] = s; // chunk s
				outBuf[idx++] = 0;
				for (i = 0; i < 19; i++) {
					if (s * 19 + i < NUM_PIXELS) {
						outBuf[idx++] = s * 19 + i;
						outBuf[idx++] = 64 - s * 19 - i;
						outBuf[idx++] = 0;
					}
				}
				write_and_check(idx, 4);
			}
			#ifdef WIN32
			Sleep(1000);
			#else
			usleep(1000000);
			#endif
			for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
				idx = 5;
				outBuf[idx++] = s; // chunk s
				outBuf[idx++] = 0;
				for (i = 0; i < 19; i++) {
					if (s * 19 + i < NUM_PIXELS) {
						outBuf[idx++] = 64 - s * 19 - i;
						outBuf[idx++] = s * 19 + i;
						outBuf[idx++] = 0;
					}
				}
				write_and_check(idx, 4);
			}
			#ifdef WIN32
			Sleep(1000);
			#else
			usleep(1000000);
			#endif
		}
		for (s = 0; s < (NUM_PIXELS + 18) / 19; s++) {
			idx = 5;
			outBuf[idx++] = s; // chunk s
			outBuf[idx++] = 0;
			for (i = 0; i < 19; i++) {
				if (s * 19 + i < NUM_PIXELS) {
					outBuf[idx++] = 0;
					outBuf[idx++] = 0;
					outBuf[idx++] = 0;
				}
			}
			write_and_check(idx, 4);
		}
		break;

	case 't':
		goto test;
		break;

	case 'u':
		goto test2;
		break;

	case 'x':
		goto exit;
		break;

	case 'h':
		memset(&outBuf[2], 0, sizeof(outBuf) - 2);
		for(l = 2; l < out_size; l++){
			outBuf[l] = l - 1; // ACC_SET ...
		}
		outBuf[3] = CMD_HID_TEST;
		write_and_check(out_size, in_size);
		break;

	default:
		goto cont;
	}

	goto cont;

monit:	memset(inBuf, 0, sizeof(inBuf));
	while(true) {
		retValm = hid_read(handle, inBuf, in_size);
		if (retValm >= 0) {
			printf("read %d bytes:\n\t", retValm);
			for (l = 0; l < 64; l++)
				printf("%02x ", inBuf[l]);
			printf("\n");

			if (inBuf[0] == REPORT_ID_KBD) {
				if (!inBuf[1] && !inBuf[3])
					printf("release\n\n\n");
				else
					printf("modifier|key: %s|%s\n\n", get_modifier_from_hex(inBuf[1]), get_key_from_hex(inBuf[3]));
			}

			if (inBuf[0] == REPORT_ID_IR) {
				printf("converted to protocoladdresscommandflag:\n\t");
				printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx   delta: %f min_delta: %f max_delta: %f upper_border: %f same key: %d timeout: %d repeat detected: %d", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6], ((float)(inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) / 1000, ((float)(inBuf[53] * 0xFF + inBuf[52]) * inBuf[56]) / 1000, ((float)(inBuf[49] * 0xFF + inBuf[48]) * inBuf[56]) / 1000, ((float)(inBuf[51] * 0xFF + inBuf[50]) * inBuf[56]) / 1000, inBuf[54], inBuf[61], inBuf[60]);
				printf("\n\n");
			}
		}
	}

rate:	while(true) {
		retValm = hid_read(handle, inBuf, in_size);
		if (retValm >= 0) {
			if (inBuf[0] == REPORT_ID_IR && inBuf[6] != IRMP_FLAG_RELEASE) {
				printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
				now_us = GetUsTicks();
				diff_us = now_us - last_us;
				last_us = now_us;
				if (first_time) {
					for(l=0;l<5;l++) {
						rrBuf[l] = inBuf[l+1];
					first_time = 0;
					}
				} else {
					if (inBuf[1] != rrBuf[0]) {
						printf("protocol changed, stopping\n");
						goto exit;
					}
					uint8_t same_key = inBuf[2] == rrBuf[1] && inBuf[3] == rrBuf[2] && inBuf[4] == rrBuf[3] && inBuf[5] == rrBuf[4];
					if (same_key && inBuf[6] == IRMP_FLAG_REPETITION) {
						if ((diff_us + 500) / 1000 <= 255) {
							pc_rate[(diff_us + 500) / 1000]++;
							uc_rate[(((inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) + 500) / 1000]++;
							printf("delta: %f\n", ((float)(inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) / 1000);
							printf("min_delta: %f\n", ((float)(inBuf[53] * 0xFF + inBuf[52]) * inBuf[56]) / 1000);
							printf("max_delta: %f\n", ((float)(inBuf[49] * 0xFF + inBuf[48]) * inBuf[56]) / 1000);
							printf("max/min: %f %%\n", (((float)(inBuf[49] * 0xFF + inBuf[48]))/((float)(inBuf[53] * 0xFF + inBuf[52])) - 1) * 100);
					}
					printf("***********************\n");
					printf("*** pc rate - count ***\n");
					for(l=0;l<255;l++) {
						if (pc_rate[l]) printf("***     %03d - %04d  ***\n", l, pc_rate[l]);
					}
					printf("***********************\n");
					printf("*** uc rate - count ***\n");
					for(l=0;l<255;l++) {
						if (uc_rate[l]) printf("***     %03d - %04d  ***\n", l, uc_rate[l]);
					}
					printf("***********************\n");
				}
					if (!same_key) {
						printf("key changed, diff_ms: %d, delta: %f\n\n", (diff_us + 500) / 1000, ((float)(inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) / 1000);
						for(l=1;l<5;l++) {
							rrBuf[l] = inBuf[l+1];
						}
						continue;
					}
					if (inBuf[6] == IRMP_FLAG_NEW) {
						printf("new key, diff_ms: %d, delta: %f\n", (diff_us + 500) / 1000, ((float)(inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) / 1000);
					}
				}
				printf("\n");
			}
		}
	}

test:	sprintf(testfilename, "test%u", j); printf("write into %s\n", testfilename); // if directory, it needs to exist (or be created)!
	fp = fopen(testfilename, "w");
	while(true) {
		retValm = hid_read(handle, inBuf, in_size);
		if (retValm >= 0) {
			printf("%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			fprintf(fp, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			if (inBuf[1] == 0x3c && inBuf[3] == 0 && inBuf[2] == 0 && inBuf[5] == 0 && inBuf[4] == 0x3f && inBuf[6] == 1) { // 3c0000003f01, stopsequence TODO make configurable
				printf("received stopsequence\n");
				fclose(fp);
				j++;
				if (j >= 200) { // TODO make number of tests configurable
					printf("exit\n");
					goto exit;
				}
				goto test;
			}
		}
	}

test2:	sprintf(testfilename, "test2_%u", j); printf("write into %s\n", testfilename); // if directory, it needs to exist (or be created)!
	fp = fopen(testfilename, "w");
	while(true) {
		retValm = hid_read(handle, inBuf, in_size);
		if (retValm >= 0) {
			printf("%s%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", first_time? "-----NEW-----\n" : "", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
			if (first_time) {
				fprintf(fp, "-----NEW-----\n%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
				for(l=0;l<5;l++) {
					rrBuf[l] = inBuf[l+1];
				first_time = 0;
				}
			} else {
				uint8_t same_key = inBuf[1] == rrBuf[0] && inBuf[2] == rrBuf[1] && inBuf[3] == rrBuf[2] && inBuf[4] == rrBuf[3] && inBuf[5] == rrBuf[4];
				if (same_key && inBuf[6] != IRMP_FLAG_NEW) {
					if (!count) fprintf(fp, "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
					count++;
					uc_rate[(((inBuf[58] * 0xFF + inBuf[57]) * inBuf[56]) + 500) / 1000]++;
					}
				else if (inBuf[6] == IRMP_FLAG_NEW) {
					printf("*** uc rate - count ***\n");
					fprintf(fp, "*** uc rate - count ***\n");
					for(l=0;l<255;l++) {
						if (uc_rate[l]) {
							printf("***     %03d - %04d  ***\n", l, uc_rate[l]);
							fprintf(fp, "***     %03d - %04d  ***\n", l, uc_rate[l]);
						}
					}
					printf("***********************\n");
					printf("-----new-----, count: %d %s\n", count, count == 256 || (count == 512 && inBuf[1] == 0x29) ? "OK" : "");
					fprintf(fp, "-----new----- count: %d %s\n%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", count, count == 256 ? "OK" : "", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
					for(l=0;l<5;l++) {
						rrBuf[l] = inBuf[l+1];
					}
					count = 0;
					memset(uc_rate, 0, sizeof(uc_rate));
				}
				else if (!same_key) {
					printf("-----change----- count: %d %s\n", count, count == 256 ? "OK" : "");
					fprintf(fp, "-----change----- count: %d %s\n%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n", count, count == 256 ? "OK" : "", inBuf[1],inBuf[3],inBuf[2],inBuf[5],inBuf[4],inBuf[6]);
					for(l=0;l<5;l++) {
						rrBuf[l] = inBuf[l+1];
					}
					count = 0;
				}
				if (inBuf[1] == 0x3c && inBuf[3] == 0 && inBuf[2] == 0 && inBuf[5] == 0 && inBuf[4] == 0x3f && inBuf[6] == 2) { // 3c0000003f02, stopsequence TODO make configurable
					printf("-----STOP----- count: %d %s\n", count, count == 256 ? "OK" : "");
					fprintf(fp, "-----STOP----- count: %d %s\n", count, count == 256 ? "OK" : "");
					fclose(fp);
					j++;
					if (j >= 1) { // TODO make number of tests configurable
						printf("exit\n");
						goto exit;
					}
					goto test2;
				}
			}
		}
	}

exit:	hid_close(handle);

	/* Free static HIDAPI objects. */
	hid_exit();

#ifdef WIN32
	Sleep(2);
#endif

	return 0;
}
