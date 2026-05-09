/**********************************************************************************************************  
    irmpemit: send IR on IRMP Pico

    Copyright (C) 2014-2026 Joerg Riechardt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

************************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>

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

static int irmpfd = -1;
uint8_t inBuf[4];
uint8_t outBuf[10];

static bool open_irmp(const char *devicename) {
	irmpfd = open(devicename, O_RDWR);
	if (irmpfd == -1) {
		printf("error opening irmp device: %s\n",strerror(errno));
		return false;
	}
	//printf("opened irmp device\n");
	return true;
}

static void read_irmp() {
	int i;
	int retVal;
	retVal = read(irmpfd, inBuf, sizeof(inBuf));
	if (retVal < 0) {
	    printf("read error\n");
        } else {
                printf("read %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", inBuf[i]);
                puts("\n");
        }
} 

static void write_irmp() {
	int i;
	int retVal;
	retVal = write(irmpfd, outBuf, sizeof(outBuf));
	if (retVal < 0) {
	    printf("write error\n");
        } else {
                printf("written %d bytes:\n\t", retVal);
                for (i = 0; i < retVal; i++)
                        printf("%02hhx ", outBuf[i]);
                puts("\n");
        }
}

int main(int argc, char *argv[]) {

	int opt = 0;
	char *dvalue = NULL;
	char *ivalue = NULL;
	uint64_t i = 0;
	uint8_t idx;

	while ((opt = getopt(argc, argv, ":d:i:")) != -1) {
	    switch (opt) {
	    case 'd':
		dvalue = optarg;
		break;
	    case 'i':
		ivalue = optarg;
		break;
	    default:
		break;
	    }
	}

	open_irmp(dvalue != NULL ? dvalue : "/dev/irmp_pico");
	outBuf[0] = REPORT_ID_CONFIG_OUT;
	outBuf[1] = STAT_CMD;
	idx = 2;

	if (ivalue != NULL) {
	    i = strtoul(ivalue, NULL, 0);
	    outBuf[idx++] = ACC_SET;
	    outBuf[idx++] = CMD_EMIT;
	    outBuf[idx++] = (i>>40) & 0xFF;
	    outBuf[idx++] = (i>>24) & 0xFF;
	    outBuf[idx++] = (i>>32) & 0xFF;
	    outBuf[idx++] = (i>>8) & 0xFF;
	    outBuf[idx++] = (i>>16) & 0xFF;
	    outBuf[idx++] = i & 0xFF;
	    write_irmp();
	    //usleep(3000);
	    read_irmp();
	    while (inBuf[0] == REPORT_ID_KBD || inBuf[0] == REPORT_ID_IR)
		read_irmp();

	    /*printf("%llX\n", strtoull(ivalue, NULL, 0));
	    printf("%x\n", (i>>40) & 0xFF);
	    printf("%x\n", (i>>24) & 0xFF);
	    printf("%x\n", (i>>32) & 0xFF);
	    printf("%x\n", (i>>8) & 0xFF);
	    printf("%x\n", (i>>16) & 0xFF);
	    printf("%x\n", i & 0xFF);*/
	}

	if (irmpfd >= 0) close(irmpfd);
}
