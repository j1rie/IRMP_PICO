OUTFILE = picoflash

CC = gcc

ifeq ($(OS),Windows_NT)
CFLAGS = -I/ucrt64/include/libusb-1.0 -Wall -Wextra -g -s -mno-ms-bitfields -O2
LDFLAGS = -Wl,-Bstatic -lusb-1.0
else
CFLAGS = -I/usr/include/libusb-1.0 -Wall -Wextra -g -s -O2
LDFLAGS = -lusb-1.0
endif

OBJ =  picoflash.o picoboot_connection.o

all: $(OUTFILE)

$(OUTFILE) $(OUTFILE).exe: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#reboot: reboot

reboot: reboot.o picoboot_connection.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OUTFILE) $(OUTFILE).exe *.o
