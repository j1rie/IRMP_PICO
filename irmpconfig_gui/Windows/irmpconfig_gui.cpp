/*
 *  GUI Config Tool for IRMP PICO devices
 *
 *  Copyright (C) 2015-2026 Joerg Riechardt
 *
 *  based on work by Alan Ott
 *  Copyright 2010  Alan Ott
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <fx.h>
#include "hidapi.h"
//#include "mac_support.h"
#include <limits.h>
#include <inttypes.h>
#include <FXArray.h>
#include "icons.h"
#include "usb_hid_keys.h"
#include "fxkeys_jr.h"
#include "protocols.h"
#include "picoboot_connection.h"
#ifdef _WIN32
	#include <windows.h>
	#include <FXCP1252Codec.h>
#endif
#define SRAM_END_RP2040      0x20042000
#define SRAM_END_RP2350      0x20082000

class MainWindow : public FXMainWindow {
	FXDECLARE(MainWindow)
public:
	enum {
		ID_FIRST = FXMainWindow::ID_LAST,
		ID_DEVLIST,
		ID_CONNECT,
		ID_DISCONNECT,
		ID_RESCAN,
		ID_REBOOT,
		ID_SET_WAKEUP,
		ID_SET_MACRO,
		ID_SET_IRDATA,
		ID_SET_KEY,
		ID_SET_REPEAT,
		ID_SETBYREMOTE_WAKEUP,
		ID_SETBYREMOTE_MACRO,
		ID_SETBYREMOTE_IRDATA,
		ID_SETBYREMOTE_KBD_IRDATA,
		ID_GET_WAKEUP,
		ID_GET_MACRO,
		ID_GET_IRDATA,
		ID_GET_KEY,
		ID_GET_REPEAT,
		ID_GET_CAPS,
		ID_RESET_WAKEUP,
		ID_RESET_MACRO,
		ID_RESET_IRDATA,
		ID_RESET_KEY,
		ID_RESET_REPEAT,
		ID_SEND_IR,
		ID_RECEIVE_IR,
		ID_SET_ALARM,
		ID_GET_ALARM,
		ID_RESET_ALARM,
		ID_SELECT_WAKEUP,
		ID_SELECT_MACRO_NR,
		ID_SELECT_MACRO_SLOT,
		ID_SELECT_REPEAT,
		ID_SEND_DATA,
		ID_SET_EEPROM,
		ID_GET_EEPROM,
		ID_RESET_EEPROM,
		ID_COMMIT_EEPROM,
		ID_GETRAW_EEPROM,
		ID_UPGRADE,
		ID_OPEN,
		ID_SAVE,
		ID_CLEAR_LOG,
		ID_SAVE_LOG,
		ID_READIR_TIMER,
		ID_RED_TIMER,
		ID_KBD_TIMER,
		ID_MAC_TIMER,
		ID_SETBYREMOTE_IRDATA_TIMER,
		ID_LAST
	};

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

#define IRMP_FLAG_NEW                   0x00
#define IRMP_FLAG_REPETITION            0x01
#define IRMP_FLAG_RELEASE               0x02

private:
	FXList *device_list;
	FXButton *connect_button;
	FXButton *disconnect_button;
	FXButton *rescan_button;
	FXButton *reboot_button;
	FXButton *set_wakeup_button;
	FXButton *set_macro_button;
	FXButton *set_irdata_button;
	FXButton *set_key_button;
	FXButton *set_repeat_button;
	FXButton *setbyremote_wakeup_button;
	FXButton *setbyremote_macro_button;
	FXButton *setbyremote_irdata_button;
	FXButton *setbyremote_keyboard_irdata_button;
	FXButton *get_wakeup_button;
	FXButton *get_macro_button;
	FXButton *get_irdata_button;
	FXButton *get_key_button;
	FXButton *get_repeat_button;
	FXButton *get_caps_button;
	FXButton *reset_wakeup_button;
	FXButton *reset_macro_button;
	FXButton *reset_irdata_button;
	FXButton *reset_key_button;
	FXButton *reset_repeat_button;
	FXButton *send_ir_button;
	FXButton *receive_ir_button;
	FXButton *set_alarm_button;
	FXButton *get_alarm_button;
	FXButton *reset_alarm_button;
	FXListBox* select_wakeup;
	FXListBox* select_macro_nr;
	FXListBox* select_macro_slot;
	FXListBox* select_repeat;
	FXButton *send_data_button;
	FXButton *set_eeprom_button;
	FXButton *get_eeprom_button;
	FXButton *reset_eeprom_button;
	FXButton *commit_eeprom_button;
	FXButton *getraw_eeprom_button;
	FXButton *upgrade_button;
	FXButton *open_button;
	FXButton *save_button;
	FXButton *clear_log_button;
	FXButton *save_log_button;
	FXLabel *connected_label;
	FXLabel *connected_label2;
	FXLabel *connected_label3;
	FXTextField *send_data_text;
	FXTextField *protocol_text;
	FXTextField *address_text;
	FXTextField *command_text;
	FXTextField *flag_text;
	FXTextField *days_text;
	FXTextField *hours_text;
	FXTextField *minutes_text;
	FXTextField *seconds_text;
	FXTextField *setbyremote_kbd_irdata_text;
	FXTextField *setbyremote_kbd_irdata_text_2;
	FXText *debug_messages_text;
	FXText *eeprom_map_text;
	FXTextField *repeat_text;
	FXTextField *modifier_text;
	FXTextField *key_text;
	FXTextField *line_text;
	FXStatusBar *statusbar;
	struct hid_device_info *devices;
	hid_device *connected_device;
	size_t getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len);
	uint8_t buf[64];
	uint8_t bufw[64];
	uint8_t ReceiveActive;
	int wakeupslots;
	int macrodepth;
	int macroslots;
	int irdatanr;
	int in_size;
	int out_size;
	FXString protocols;
	FXString firmware;
	FXString firmware1;
	FXString uC;
	FXColor storedShadowColor;
	FXColor storedBaseColor;
	FXColor storedBackColor;
	int RepeatCounter;
	FXString map[400];
	int mapbeg[200];
	int active_lines;
	int max;
	int count;
	FXint cur_item;
	FXint num_devices_before_upgrade;
	FXint num_devices_after_rescan;
	FXlong starttime;
	int got_key;
	int got_modifier;
	int setbyremote_kbd_irdata_active;
	FXString last_modifier;
	FXString last_key;
	int template_mode;
	char model[8];
	libusb_device_handle *dev_handle = NULL;
	libusb_device *device = NULL;

protected:
	MainWindow() {};
public:
	MainWindow(FXApp *app);
	~MainWindow();
	virtual void create();
	
	long onConnect(FXObject *sender, FXSelector sel, void *ptr);
	long onDisconnect(FXObject *sender, FXSelector sel, void *ptr);
	long onRescan(FXObject *sender, FXSelector sel, void *ptr);
	long onReboot(FXObject *sender, FXSelector sel, void *ptr);
	long onSetWakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onSetMacro(FXObject *sender, FXSelector sel, void *ptr);
	long onSetIrdata(FXObject *sender, FXSelector sel, void *ptr);
	long onSetKey(FXObject *sender, FXSelector sel, void *ptr);
	long onSetRepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onSetByRemoteWakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onSetByRemoteMacro(FXObject *sender, FXSelector sel, void *ptr);
	long onSetByRemoteIrdata(FXObject *sender, FXSelector sel, void *ptr);
	long onSetByRemoteKeyboardIrdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGetWakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onGetMacro(FXObject *sender, FXSelector sel, void *ptr);
	long onGetIrdata(FXObject *sender, FXSelector sel, void *ptr);
	long onGetKey(FXObject *sender, FXSelector sel, void *ptr);
	long onGetRepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onGetCaps(FXObject *sender, FXSelector sel, void *ptr);
	long onResetWakeup(FXObject *sender, FXSelector sel, void *ptr);
	long onResetMacro(FXObject *sender, FXSelector sel, void *ptr);
	long onResetIrdata(FXObject *sender, FXSelector sel, void *ptr);
	long onResetKey(FXObject *sender, FXSelector sel, void *ptr);
	long onResetRepeat(FXObject *sender, FXSelector sel, void *ptr);
	long onSendIR(FXObject *sender, FXSelector sel, void *ptr);
	long onReadIR(FXObject *sender, FXSelector sel, void *ptr); //
	long onReceiveIR(FXObject *sender, FXSelector sel, void *ptr);
	long onSetAlarm(FXObject *sender, FXSelector sel, void *ptr);
	long onGetAlarm(FXObject *sender, FXSelector sel, void *ptr);
	long onResetAlarm(FXObject *sender, FXSelector sel, void *ptr);
	long onSelectWakeup(FXObject*,FXSelector,void*);
	long onSelectMacroNr(FXObject*,FXSelector,void*);
	long onSelectMacroSlot(FXObject*,FXSelector,void*);
	long onSelectRepeat(FXObject*,FXSelector,void*);
	long onSendData(FXObject *sender, FXSelector sel, void *ptr);
	long onSetEeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onGetEeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onResetEeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onCommitEeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onGetrawEeprom(FXObject *sender, FXSelector sel, void *ptr);
	long onUpgrade(FXObject *sender, FXSelector sel, void *ptr);
	long onOpen(FXObject *sender, FXSelector sel, void *ptr);
	long onSave(FXObject *sender, FXSelector sel, void *ptr);
	long onClearLog(FXObject *sender, FXSelector sel, void *ptr);
	long onSaveLog(FXObject *sender, FXSelector sel, void *ptr);
	long onReadirTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onRedTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onKbdTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onMacTimeout(FXObject *sender, FXSelector sel, void *ptr);
	long onSetByRemoteIrdataTimeout(FXObject *sender, FXSelector sel, void *ptr);
	uint8_t get_hex_from_key(FXString s);
	FXString get_key_from_hex(uint8_t hex);
	uint8_t get_hex_from_modifier(FXString s);
	FXString get_modifier_from_hex(uint8_t hex);
	long Write(int out_len);
	long Read(int show_len);
	long Write_and_Check(int out_len, int show_len);
	long saveFile(const FXString& file);
	long saveLogFile(const FXString& file);
	long onApply(FXObject *sender, FXSelector sel, void *ptr);
	long onDevlistDoubleclicked(FXObject *sender, FXSelector sel, void *ptr);
	long onCmdQuit(FXObject *sender, FXSelector sel, void *ptr);
	long onKeyPress(FXObject *sender, FXSelector sel, void *ptr);
	FXString get_key_from_event_code(uint32_t code);
	uint32_t timestamp;
	void check_eeprom_changed(void);
	void print_output(const char* format, ...);
	uint8_t * get_firmware(const char *firmwarefile, int *firmwareSize);
	void open_device(void);
	int picoflash(char const* firmwarefile);
};

// FOX 1.7 changes the timeouts to all be nanoseconds.
// Fox 1.6 had all timeouts as milliseconds. // this is true only for addTimeout(), but NOT for FXThread::sleep() !
#if (FOX_MINOR >= 7)
	const int timeout_scalar = 1000*1000;
#else
	const int timeout_scalar = 1;
#endif

FXMainWindow *g_main_window;

FXDEFMAP(MainWindow) MainWindowMap [] = {
	FXMAPFUNC(SEL_DOUBLECLICKED, MainWindow::ID_DEVLIST, MainWindow::onDevlistDoubleclicked ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CONNECT, MainWindow::onConnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_DISCONNECT, MainWindow::onDisconnect ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESCAN, MainWindow::onRescan ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_REBOOT, MainWindow::onReboot ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_WAKEUP, MainWindow::onSetWakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_MACRO, MainWindow::onSetMacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_IRDATA, MainWindow::onSetIrdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_KEY, MainWindow::onSetKey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_REPEAT, MainWindow::onSetRepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SETBYREMOTE_WAKEUP, MainWindow::onSetByRemoteWakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SETBYREMOTE_MACRO, MainWindow::onSetByRemoteMacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SETBYREMOTE_IRDATA, MainWindow::onSetByRemoteIrdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SETBYREMOTE_KBD_IRDATA, MainWindow::onSetByRemoteKeyboardIrdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_WAKEUP, MainWindow::onGetWakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_MACRO, MainWindow::onGetMacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_IRDATA, MainWindow::onGetIrdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_KEY, MainWindow::onGetKey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_REPEAT, MainWindow::onGetRepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_CAPS, MainWindow::onGetCaps ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_WAKEUP, MainWindow::onResetWakeup ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_MACRO, MainWindow::onResetMacro ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_IRDATA, MainWindow::onResetIrdata ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_KEY, MainWindow::onResetKey ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_REPEAT, MainWindow::onResetRepeat ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SEND_IR, MainWindow::onSendIR ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RECEIVE_IR, MainWindow::onReceiveIR ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_ALARM, MainWindow::onSetAlarm ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_ALARM, MainWindow::onGetAlarm ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_ALARM, MainWindow::onResetAlarm ),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_SELECT_WAKEUP, MainWindow::onSelectWakeup),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SELECT_WAKEUP, MainWindow::onSelectWakeup),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_SELECT_MACRO_NR, MainWindow::onSelectMacroNr),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SELECT_MACRO_NR, MainWindow::onSelectMacroNr),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_SELECT_MACRO_SLOT, MainWindow::onSelectMacroSlot),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SELECT_MACRO_SLOT, MainWindow::onSelectMacroSlot),
	FXMAPFUNC(SEL_CHANGED, MainWindow::ID_SELECT_REPEAT, MainWindow::onSelectRepeat),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SELECT_REPEAT, MainWindow::onSelectRepeat),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SEND_DATA, MainWindow::onSendData ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SET_EEPROM, MainWindow::onSetEeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GET_EEPROM, MainWindow::onGetEeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_RESET_EEPROM, MainWindow::onResetEeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_COMMIT_EEPROM, MainWindow::onCommitEeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_GETRAW_EEPROM, MainWindow::onGetrawEeprom ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_UPGRADE, MainWindow::onUpgrade ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_OPEN, MainWindow::onOpen ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE, MainWindow::onSave ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_CLEAR_LOG, MainWindow::onClearLog ),
	FXMAPFUNC(SEL_COMMAND, MainWindow::ID_SAVE_LOG, MainWindow::onSaveLog ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_READIR_TIMER, MainWindow::onReadirTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_RED_TIMER, MainWindow::onRedTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_KBD_TIMER, MainWindow::onKbdTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_MAC_TIMER, MainWindow::onMacTimeout ),
	FXMAPFUNC(SEL_TIMEOUT, MainWindow::ID_SETBYREMOTE_IRDATA_TIMER, MainWindow::onSetByRemoteIrdataTimeout ),
	FXMAPFUNC(SEL_KEYPRESS, MainWindow::ID_SETBYREMOTE_KBD_IRDATA, MainWindow::onKeyPress),
	FXMAPFUNC(SEL_CLOSE,   0, MainWindow::onCmdQuit ),
};

FXIMPLEMENT(MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER(MainWindowMap));

MainWindow::MainWindow(FXApp *app)
#ifdef _WIN32
	: FXMainWindow(app, "IRMP Pico Configuration                 (Version: " + (FXString)DATE_STR + ")", NULL, NULL, DECOR_ALL, 425, 30, 1200, 1168)  // for 1920x1200
#else
	: FXMainWindow(app, "IRMP Pico Configuration                 (Version: " + (FXString)DATE_STR + ")", NULL, NULL, DECOR_ALL, 425, 0, 1200, 1168)  // for 1920x1200
#endif
{
	this->setIcon(new FXGIFIcon(app,Icon,0,IMAGE_OPAQUE)); // for taskbar
	this->setMiniIcon(new FXGIFIcon(app,Icon,0,IMAGE_OPAQUE)); // for titlebar
	devices = NULL;
	connected_device = NULL;

	// create horizontal frame with two vertical frames
	FXHorizontalFrame *hf1 = new FXHorizontalFrame(this, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 2,0);
	FXVerticalFrame *vf11 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	FXVerticalFrame *vf12 = new FXVerticalFrame(hf1, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0/*,0,0,0*/);

	// second vertical frame for eeprom_map_text
	new FXLabel(vf12, "eeprom map                                       ");
	FXVerticalFrame *innerVF12 = new FXVerticalFrame(vf12, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
	eeprom_map_text = new FXText(new FXHorizontalFrame(innerVF12,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y/*,0,0,0,0, 0,0,0,0*/);

	// first vertical frame: everything else
	// devices list and buttons
	FXHorizontalFrame *hf111 = new FXHorizontalFrame(vf11, LAYOUT_FILL_X);
	device_list = new FXList(new FXHorizontalFrame(hf111,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0), this, ID_DEVLIST, LISTBOX_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,300,200);
	FXVerticalFrame *buttonVF111 = new FXVerticalFrame(hf111);
	connect_button = new FXButton(buttonVF111, "Connect", NULL, this, ID_CONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	disconnect_button = new FXButton(buttonVF111, "Disconnect", NULL, this, ID_DISCONNECT, BUTTON_NORMAL|LAYOUT_FILL_X);
	rescan_button = new FXButton(buttonVF111, "Re-Scan devices", NULL, this, ID_RESCAN, BUTTON_NORMAL|LAYOUT_FILL_X);
	reboot_button = new FXButton(buttonVF111, "Reboot device", NULL, this, ID_REBOOT, BUTTON_NORMAL|LAYOUT_FILL_X);
	connected_label = new FXLabel(vf11, "Disconnected");
	connected_label2 = new FXLabel(vf11, "Firmware:");
	connected_label3 = new FXLabel(vf11, "Protocols:");
	
	// horizontal frame of group boxes for set, set by remote, get and reset
	FXHorizontalFrame *hf112 = new FXHorizontalFrame(vf11, LAYOUT_FILL_X|PACK_UNIFORM_WIDTH);
	//set group box
	FXGroupBox *gb1121 = new FXGroupBox(hf112, "set", FRAME_GROOVE|LAYOUT_FILL_X,0,0,0,0, 4,4,4,32, 4,4);
	set_wakeup_button = new FXButton(gb1121, "wakeup", NULL, this, ID_SET_WAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	set_macro_button = new FXButton(gb1121, "macro", NULL, this, ID_SET_MACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	set_irdata_button = new FXButton(gb1121, "irdata", NULL, this, ID_SET_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	set_key_button = new FXButton(gb1121, "key", NULL, this, ID_SET_KEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	set_repeat_button = new FXButton(gb1121, "repeat", NULL, this, ID_SET_REPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	//set by remote group box
	FXGroupBox *gb1122 = new FXGroupBox(hf112, "set by remote", FRAME_GROOVE|LAYOUT_FILL_X,0,0,0,0, 4,4,4,4, 4,4);
	setbyremote_wakeup_button = new FXButton(gb1122, "wakeup", NULL, this, ID_SETBYREMOTE_WAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	setbyremote_macro_button = new FXButton(gb1122, "macro", NULL, this, ID_SETBYREMOTE_MACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	setbyremote_irdata_button = new FXButton(gb1122, "irdata", NULL, this, ID_SETBYREMOTE_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	setbyremote_keyboard_irdata_button = new FXButton(gb1122, "keyboard + irdata", NULL, this, ID_SETBYREMOTE_KBD_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	setbyremote_kbd_irdata_text = new FXTextField(new FXHorizontalFrame(gb1122,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	setbyremote_kbd_irdata_text_2 = new FXTextField(new FXHorizontalFrame(gb1122,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	setbyremote_kbd_irdata_text->setEditable(false);
	setbyremote_kbd_irdata_text_2->setEditable(false);
	//get group box
	FXGroupBox *gb1123 = new FXGroupBox(hf112, "get", FRAME_GROOVE|LAYOUT_FILL_X);
	get_wakeup_button = new FXButton(gb1123, "wakeup", NULL, this, ID_GET_WAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_macro_button = new FXButton(gb1123, "macro", NULL, this, ID_GET_MACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_irdata_button = new FXButton(gb1123, "irdata", NULL, this, ID_GET_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_key_button = new FXButton(gb1123, "key", NULL, this, ID_GET_KEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_repeat_button = new FXButton(gb1123, "repeat", NULL, this, ID_GET_REPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_caps_button = new FXButton(gb1123, "caps", NULL, this, ID_GET_CAPS, BUTTON_NORMAL|LAYOUT_FILL_X);
	//reset group box
	FXGroupBox *gb1124 = new FXGroupBox(hf112, "reset", FRAME_GROOVE|LAYOUT_FILL_X,0,0,0,0, 4,4,4,32, 4,4);
	reset_wakeup_button = new FXButton(gb1124, "wakeup", NULL, this, ID_RESET_WAKEUP, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_macro_button = new FXButton(gb1124, "macro", NULL, this, ID_RESET_MACRO, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_irdata_button = new FXButton(gb1124, "irdata", NULL, this, ID_RESET_IRDATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_key_button = new FXButton(gb1124, "key", NULL, this, ID_RESET_KEY, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_repeat_button = new FXButton(gb1124, "repeat", NULL, this, ID_RESET_REPEAT, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame of group boxes for IR, alarm, select listboxes, PC->IRMP, eeprom, firmware and eeprom map
	FXHorizontalFrame *hf113 = new FXHorizontalFrame(vf11, LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	// 3 vertical frames 2:1:1
	FXSpring *s1131 = new FXSpring(hf113, LAYOUT_FILL_X, 200, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf1131 = new FXVerticalFrame(s1131, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0, 0,0);
	FXSpring *s1132 = new FXSpring(hf113, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf1132 = new FXVerticalFrame(s1132, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,3,4,0, 0,8);
	FXSpring *s1133 = new FXSpring(hf113, LAYOUT_FILL_X, 100, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf1133 = new FXVerticalFrame(s1133, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 3,4,4,0, 0,0);
	// horizontal frame of group boxes for IR, alarm and select
	FXHorizontalFrame *hf11311 = new FXHorizontalFrame(vf1131, LAYOUT_FILL_X);
	// 2 vertical frames 7:3
	FXSpring *s113111 = new FXSpring(hf11311, LAYOUT_FILL_X, 70, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf113111 = new FXVerticalFrame(s113111, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	FXSpring *s113112 = new FXSpring(hf11311, LAYOUT_FILL_X, 30, 0, 0,0,0,0, 0,0,0,0, 0,0);
	FXVerticalFrame *vf113112 = new FXVerticalFrame(s113112, LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
	//IR group box
	FXGroupBox *gb1131111 = new FXGroupBox(vf113111, "IR (hex)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0/*, 4,4,4,10*/);
	FXMatrix *m1131111 = new FXMatrix(gb1131111, 5, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN, 0,0,0,0, 0,0,0,4, 4,0);
	new FXLabel(m1131111, "protocol");
	new FXLabel(m1131111, "address");
	new FXLabel(m1131111, "command");
	new FXLabel(m1131111, "flag");
	send_ir_button = new FXButton(m1131111, "send", NULL, this, ID_SEND_IR, BUTTON_NORMAL|LAYOUT_FILL_X/*,0,0,0,0,0,0,0,0*/);
	protocol_text = new FXTextField(m1131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	address_text = new FXTextField(m1131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	command_text = new FXTextField(m1131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	flag_text = new FXTextField(m1131111, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	receive_ir_button = new FXButton(m1131111, " receive mode ", NULL, this, ID_RECEIVE_IR, BUTTON_NORMAL|LAYOUT_FILL_X/*,0,0,0,0,0,0,0,0*/);
	//alarm group box
	FXGroupBox *gb1131112 = new FXGroupBox(vf113111, "alarm (dec)", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0/*, 4,4,4,10*/);
	FXMatrix *m1131112 = new FXMatrix(gb1131112, 7, MATRIX_BY_COLUMNS|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN, 0,0,0,0, 0,0,0,4, 4,0);
	new FXLabel(m1131112, "days");
	new FXLabel(m1131112, "hours");
	new FXLabel(m1131112, "minutes");
	new FXLabel(m1131112, "seconds");
	new FXLabel(m1131112, "");
	new FXLabel(m1131112, "");
	new FXLabel(m1131112, "");
	days_text = new FXTextField(m1131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	hours_text = new FXTextField(m1131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	minutes_text = new FXTextField(m1131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	seconds_text = new FXTextField(m1131112, 5, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	set_alarm_button = new FXButton(m1131112, "set", NULL, this, ID_SET_ALARM, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_alarm_button = new FXButton(m1131112, "get", NULL, this, ID_GET_ALARM, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_alarm_button = new FXButton(m1131112, "reset", NULL, this, ID_RESET_ALARM, BUTTON_NORMAL|LAYOUT_FILL_X);
	// select group box
	FXGroupBox *gb113112 = new FXGroupBox(vf113112, "select", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,6);
	select_wakeup=new FXListBox(gb113112,this,ID_SELECT_WAKEUP,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	select_macro_nr=new FXListBox(gb113112,this,ID_SELECT_MACRO_NR,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	select_macro_slot=new FXListBox(gb113112,this,ID_SELECT_MACRO_SLOT,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	select_repeat=new FXListBox(gb113112,this,ID_SELECT_REPEAT,FRAME_SUNKEN|FRAME_THICK|LAYOUT_TOP);
	FXHorizontalFrame *hf113112 = new FXHorizontalFrame(gb113112, LAYOUT_FILL_X, 0,0,0,0, 0,0,0,0/*, 0,0*/);
	new FXLabel(hf113112, "repeat");
	repeat_text = new FXTextField(hf113112, 10, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	// horizontal frame for PC->IRMP group box
	FXHorizontalFrame *hf11312 = new FXHorizontalFrame(vf1131, LAYOUT_FILL_X, 0,0,0,0/*, 0,0,0,0, 0,0*/);
	// PC->IRMP group box
	FXGroupBox *gb11312 = new FXGroupBox(hf11312, "PC->IRMP", FRAME_GROOVE|LAYOUT_FILL_X);
	FXHorizontalFrame *hf113121 = new FXHorizontalFrame(gb11312, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,4,3/*, 0,0*/);
	new FXLabel(hf113121, "Data");
	send_data_text = new FXTextField(hf113121, 29, NULL, 0, TEXTFIELD_NORMAL|LAYOUT_FILL_X|LAYOUT_FILL_COLUMN);
	send_data_button = new FXButton(hf113121, "Send to IRMP Device", NULL, this, ID_SEND_DATA, BUTTON_NORMAL|LAYOUT_FILL_X);
	// eeprom group box
	FXGroupBox *gb11321 = new FXGroupBox(vf1132, "eeprom", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,6);
	set_eeprom_button = new FXButton(gb11321, "flash", NULL, this, ID_SET_EEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	get_eeprom_button = new FXButton(gb11321, "get", NULL, this, ID_GET_EEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	reset_eeprom_button = new FXButton(gb11321, "reset", NULL, this, ID_RESET_EEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	commit_eeprom_button = new FXButton(gb11321, "commit", NULL, this, ID_COMMIT_EEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	getraw_eeprom_button = new FXButton(gb11321, "get raw", NULL, this, ID_GETRAW_EEPROM, BUTTON_NORMAL|LAYOUT_FILL_X);
	// firmware group box
	FXGroupBox *gb11322 = new FXGroupBox(vf1132, "firmware", FRAME_GROOVE|LAYOUT_FILL_X, 0,0,0,0, 4,4,4,11);
	upgrade_button = new FXButton(gb11322, "upgrade", NULL, this, ID_UPGRADE, BUTTON_NORMAL|LAYOUT_FILL_X);
	// eeprom map group box
	FXGroupBox *gb1133 = new FXGroupBox(vf1133, "eeprom map", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 4,4,0,4);
	open_button = new FXButton(gb1133, "open file", NULL, this, ID_OPEN, BUTTON_NORMAL|LAYOUT_FILL_X);
	save_button = new FXButton(gb1133, "save file", NULL, this, ID_SAVE, BUTTON_NORMAL|LAYOUT_FILL_X);
	new FXLabel(gb1133, "modifier");
	FXVerticalFrame *innerVF10 = new FXVerticalFrame(gb1133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	modifier_text = new FXTextField(new FXHorizontalFrame(innerVF10,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb1133, "key");
	FXVerticalFrame *innerVF9 = new FXVerticalFrame(gb1133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	key_text = new FXTextField(new FXHorizontalFrame(innerVF9,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, NULL, 0, LAYOUT_FILL_X);
	new FXLabel(gb1133, "line");
	FXVerticalFrame *innerVF1133 = new FXVerticalFrame(gb1133, LAYOUT_FILL_X/*|LAYOUT_FILL_Y*/, 0,0,0,0, 0,0,0,0);
	line_text = new FXTextField(new FXHorizontalFrame(innerVF1133,LAYOUT_FILL_X|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), 12, eeprom_map_text, FXText::ID_CURSOR_ROW, LAYOUT_FILL_X);

	// horizontal frame for debug messages group box
	FXHorizontalFrame *hf114 = new FXHorizontalFrame(vf11, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	// debug messages group box
	FXGroupBox *gb114 = new FXGroupBox(hf114, "debug messages", FRAME_GROOVE|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXHorizontalFrame *innerHF114 = new FXHorizontalFrame(gb114, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	debug_messages_text = new FXText(new FXHorizontalFrame(innerHF114,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_SUNKEN|FRAME_THICK, 0,0,0,0, 0,0,0,0), NULL, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y);
	debug_messages_text->setEditable(false);
	FXVerticalFrame *innerVF114 = new FXVerticalFrame(innerHF114,LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	clear_log_button = new FXButton(innerVF114, "Clear", NULL, this, ID_CLEAR_LOG, BUTTON_NORMAL|LAYOUT_FILL_X);
	save_log_button = new FXButton(innerVF114, "Save", NULL, this, ID_SAVE_LOG, BUTTON_NORMAL|LAYOUT_FILL_X);

	// horizontal frame for Status Bar
	FXHorizontalFrame *hf115 = new FXHorizontalFrame(vf11, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X,0,0,0,0, 1,2,0,3);
	//Status Bar
	statusbar = new FXStatusBar(hf115, LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);

	// HelpText
	device_list->setHelpText("select one of found IRMP_PICO devices (doubleclick connects)");
	connect_button->setHelpText("connect to selected device");
	disconnect_button->setHelpText("disconnect device");
	rescan_button->setHelpText("rescan devices");
	reboot_button->setHelpText("reboot device into mass storage mode (then drag and drop firmware onto it)");
	set_wakeup_button->setHelpText("set wakeup");
	set_macro_button->setHelpText("set macro");
	set_irdata_button->setHelpText("set irdata");
	set_key_button->setHelpText("set key");
	setbyremote_wakeup_button->setHelpText("set wakeup by remote");
	setbyremote_macro_button->setHelpText("set macro by remote");
	setbyremote_irdata_button->setHelpText("set irdata by remote");
	setbyremote_keyboard_irdata_button->setHelpText("set key by keyboard and irdata by remote, press again in order to stop - press twice for testing keys");
	get_wakeup_button->setHelpText("get wakeup");
	get_macro_button->setHelpText("get macro");
	get_irdata_button->setHelpText("get irdata");
	get_key_button->setHelpText("get key");
	get_caps_button->setHelpText("get capabilities");
	reset_wakeup_button->setHelpText("reset wakeup");
	reset_macro_button->setHelpText("reset macro");
	reset_irdata_button->setHelpText("reset irdata");
	reset_key_button->setHelpText("reset key");
	reset_alarm_button->setHelpText("reset alarm");
	protocol_text->setHelpText("IR protocol");
	address_text->setHelpText("IR address");
	command_text->setHelpText("IR command");
	flag_text->setHelpText("IR flags");
	send_ir_button->setHelpText("send IR");
	days_text->setHelpText("days");
	hours_text->setHelpText("hours");
	minutes_text->setHelpText("minutes");
	seconds_text->setHelpText("seconds");
	receive_ir_button->setHelpText("receive IRdata and keys until pressed again");
	upgrade_button->setHelpText("upgrade firmware");
	commit_eeprom_button->setHelpText("RP2xxx: flash permanently into eeprom");
	getraw_eeprom_button->setHelpText("RP2xxx: get eeprom raw");
	set_alarm_button->setHelpText("set alarm");
	get_alarm_button->setHelpText("get alarm");
	select_wakeup->setHelpText("wakeup to be set");
	select_macro_nr->setHelpText("macronumber to be set");
	select_macro_slot->setHelpText("macroslot to be set");
	select_repeat->setHelpText("repeat to be set");
	repeat_text->setHelpText("repeat");
	send_data_text->setHelpText("data to be sent to device (experts only)");
	send_data_button->setHelpText("send to device");
	debug_messages_text->setHelpText("debug messages");
	clear_log_button->setHelpText("clear debug messages");
	save_log_button->setHelpText("save debug messages");
	open_button->setHelpText("open translation map");
	modifier_text->setHelpText("modifier i.e. LeftShift");
	key_text->setHelpText("key");
	save_button->setHelpText("save translation map");
	eeprom_map_text->setHelpText("eeprom translation map");
	set_eeprom_button->setHelpText("STM32: flash permanently into eeprom ; RP2xxx: flash temporarily into eeprom cache in RAM, so finally press 'commit'!");
	get_eeprom_button->setHelpText("get from eeprom");
	reset_eeprom_button->setHelpText("reset eeprom");
	line_text->setHelpText("line in eeprom map");
	set_repeat_button->setHelpText("set repeat");
	get_repeat_button->setHelpText("get repeat");
	reset_repeat_button->setHelpText("reset repeat");
	setbyremote_kbd_irdata_text->setHelpText("hints for keyboard + irdata");
	setbyremote_kbd_irdata_text_2->setHelpText("hints for keyboard + irdata");

	// disable buttons
	get_alarm_button->disable();
	set_alarm_button->disable();
	commit_eeprom_button->disable();
	disconnect_button->disable();
	set_eeprom_button->disable();
	get_caps_button->disable();
	get_eeprom_button->disable();
	getraw_eeprom_button->disable();
	get_irdata_button->disable();
	get_key_button->disable();
	get_macro_button->disable();
	get_repeat_button->disable();
	get_wakeup_button->disable();
	open_button->disable();
	send_data_button->disable();
	set_irdata_button->disable();
	set_key_button->disable();
	set_macro_button->disable();
	set_repeat_button->disable();
	setbyremote_irdata_button->disable();
	setbyremote_keyboard_irdata_button->disable();
	setbyremote_macro_button->disable();
	setbyremote_wakeup_button->disable();
	set_wakeup_button->disable();
	reset_alarm_button->disable();
	receive_ir_button->disable();
	reboot_button->disable();
	reset_eeprom_button->disable();
	reset_irdata_button->disable();
	reset_key_button->disable();
	reset_macro_button->disable();
	reset_repeat_button->disable();
	reset_wakeup_button->disable();
	save_button->disable();
	send_ir_button->disable();

	// save Colors
	storedShadowColor = setbyremote_keyboard_irdata_button->getShadowColor();
	storedBaseColor = setbyremote_keyboard_irdata_button->getBaseColor();
	storedBackColor = setbyremote_keyboard_irdata_button->getBackColor();
	storedShadowColor = receive_ir_button->getShadowColor();
	storedBaseColor = receive_ir_button->getBaseColor();
	storedBackColor = receive_ir_button->getBackColor();

	// initialize
	ReceiveActive = 0;
	RepeatCounter = 0;
	active_lines = 0;
	wakeupslots = 0;
	macrodepth = 0;
	macroslots = 0;
	irdatanr = 0;
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	got_key = 0;
	got_modifier = 0;
	setbyremote_kbd_irdata_active = 0;
	setbyremote_kbd_irdata_text->setTextColor(FXRGB(255,0,0));
	setbyremote_kbd_irdata_text_2->setTextColor(FXRGB(255,0,0));
	last_modifier = "";
	last_key = "";
	template_mode = 0;
	in_size = 64;
	out_size = 64;
	hid_init();
	//hid_error(NULL);
}

MainWindow::~MainWindow()
{
	if (connected_device)
		hid_close(connected_device);
	hid_exit();
}

long
MainWindow::onCmdQuit(FXObject *sender, FXSelector sel, void *ptr)
{
	if(eeprom_map_text->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),"%s", tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	check_eeprom_changed();
	getApp()->exit(0);
	return 0;
}

void
MainWindow::create()
{
	FXMainWindow::create();
	show();

	onRescan(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);

#ifdef __APPLE__
	init_apple_message_system();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif
}

long
MainWindow::onConnect(FXObject *sender, FXSelector sel, void *ptr)
{
	if (connected_device)
		return 1;

	starttime = FXThread::time() / 1000000; // ms
	
	FXint cur_item = device_list->getCurrentItem();
	if (cur_item < 0)
		return -1;
	FXListItem *item = device_list->getItem(cur_item);
	if (!item)
		return -1;
	struct hid_device_info *device_info = (struct hid_device_info*) item->getData();
	if (!device_info)
		return -1;
	
	connected_device =  hid_open_path(device_info->path);

	if (!connected_device) {
		FXMessageBox::error(this, MBOX_OK, "Device Error onConnect", "Unable To Connect to Device");
		return -1;
	}

	hid_set_nonblocking(connected_device, 1);

	FXString s, t, u, v, w, x;
	s.format("%x %x %x %x 0 ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_CAPS); // hex!
	send_data_text->setText(s);
	Write_and_Check(5, 9);

	if(in_size != (buf[7] ? buf[7] : 17))
		u.format("warning: hid in report count mismatch: %u %u\n", in_size, buf[7] ? buf[7] : 17);
	if(out_size != (buf[8] ? buf[8] : 17))
		v.format("warning: hid out report count mismatch: %u %u\n", out_size,  buf[8] ? buf[8] : 17);
	if(!buf[7] || !buf[8])
		w.format("old firmware!\n");
	x += u+v+w;

	if(onGetCaps(NULL, 0, NULL) == -1)
		return -1;
	s.format("Connected to: %04hx:%04hx -", device_info->vendor_id, device_info->product_id);
	//s += FXString(" ") + device_info->manufacturer_string;
	s += FXString(" ") + device_info->product_string;
	connected_label->setText(s);
	s = "Firmware: ";
	FXint pos = firmware.find("   ", 3);
	firmware1 = firmware.left(pos);
	firmware.substitute("___","   ");
	firmware.substitute(":_",": ");
	s += firmware;
	connected_label2->setText(s);
	s = "Protocols: ";
	s += protocols;
	connected_label3->setText(s);
	for(int i = 0; i < wakeupslots; i++) {
		s = (i < wakeupslots-1) ? "wakeup" : "reboot";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += (i > 0 && i < wakeupslots-1) ? t : "";
#else
		s += (i > 0 && i < wakeupslots-1) ? FXStringVal(i,10) : "";
#endif
		select_wakeup->appendItem(s);
	}
	select_wakeup->setNumVisible(wakeupslots);
	for(int i = 0; i < macrodepth; i++) {
		s = "macro";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		select_macro_nr->appendItem(s);
	}
	select_macro_nr->setNumVisible(macrodepth);
	for(int i = 0; i < macroslots; i++) {
		s = "macroslot";
#if (FOX_MINOR >= 7)
		t.fromInt(i,10);
		s += t;
#else
		s += FXStringVal(i,10);
#endif
		select_macro_slot->appendItem(s);
	}
	select_macro_slot->setNumVisible(macroslots);
	select_repeat->appendItem("repeat delay");
	select_repeat->appendItem("repeat period");
	select_repeat->appendItem("repeat timeout");
	select_repeat->setNumVisible(3);
	onGetEeprom(NULL, 0, NULL);
	get_alarm_button->enable();
	set_alarm_button->enable();
	commit_eeprom_button->enable();
	connect_button->disable();
	disconnect_button->enable();
	set_eeprom_button->enable();
	get_caps_button->enable();
	get_eeprom_button->enable();
	getraw_eeprom_button->enable();
	get_irdata_button->enable();
	get_key_button->enable();
	get_macro_button->enable();
	get_repeat_button->enable();
	get_wakeup_button->enable();
	open_button->enable();
	send_data_button->enable();
	set_irdata_button->enable();
	set_key_button->enable();
	set_macro_button->enable();
	set_repeat_button->enable();
	setbyremote_irdata_button->enable();
	setbyremote_keyboard_irdata_button->enable();
	setbyremote_macro_button->enable();
	setbyremote_wakeup_button->enable();
	set_wakeup_button->enable();
	reset_alarm_button->enable();
	receive_ir_button->enable();
	reboot_button->enable();
	reset_eeprom_button->enable();
	reset_irdata_button->enable();
	reset_key_button->enable();
	reset_macro_button->enable();
	reset_repeat_button->enable();
	reset_wakeup_button->enable();
	save_button->enable();
	send_ir_button->enable();

	//list version, wakeups, macros and alarm and warn if no STM32
	u += "Version: " + (FXString)DATE_STR + "\n";
	for(int i = 0; i < wakeupslots; i++) {
#if (FOX_MINOR >= 7)
		t.fromInt(i,16);
#else
		t = FXStringVal(i,16);
#endif
		s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
		s += t;
		send_data_text->setText(s);
		Write_and_Check(5, 10);
		s = (i < wakeupslots-1) ? "wakeup: " : "reboot: ";
		t.format("%02x", buf[4]);
		v = t;
		t.format("%02x", buf[6]);
		v += t;
		t.format("%02x", buf[5]);
		v += t;
		t.format("%02x", buf[8]);
		v += t;
		t.format("%02x", buf[7]);
		v += t;
		t.format("%02x", buf[9]);
		v += t;
		s += v;
		s += "\n";
		if(v != "ffffffffffff") {
			u += s;
		}
	}
int show_macro = 0;
	for(int i = 0; i < macroslots; i++) {
		for(int k = 0; k < macrodepth; k++) {
#if (FOX_MINOR >= 7)
			t.fromUInt(i,16);
			v.fromUInt(k,16);
#else
			t = FXStringVal(i,16);
			v = FXStringVal(k,16);
#endif
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_MACRO);
			s += t;
			s += " ";
			s += v;
			send_data_text->setText(s);
			Write_and_Check(6, 10);
			t.format("%02x", buf[4]);
			v = t;
			t.format("%02x", buf[6]);
			v += t;
			t.format("%02x", buf[5]);
			v += t;
			t.format("%02x", buf[8]);
			v += t;
			t.format("%02x", buf[7]);
			v += t;
			t.format("%02x", buf[9]);
			v += t;
			if(v != "ffffffffffff") {
				if(!k) {
					w += "macro: ";
					w += v;
					w += " ->";
				} else {
					w += " ";
					w += v;
					if(k == macrodepth - 1)
						w += "\n";
					show_macro = 1;
				}
			} else {
				if(!k) {
					i = macroslots; // exit both loops
					break;
				} else {
					w += "\n";
					break;
				}
			}
		}
	}
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	send_data_text->setText(s);
	Write_and_Check(4, 8);
	unsigned int alarm = *((uint32_t *)&buf[4]);
	s = "alarm: ";
	t.format("%u", alarm/60/60/24);
	s += t;
	s += " days, ";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	s += " hours, ";
	t.format("%d", (alarm/60) % 60);
	s += t;
	s += " minutes, ";
	t.format("%d", alarm % 60);
	s += t;
	s += " seconds\n";
	if(uC != "RP2xxx"){
		s += "WARNING: This device's microcontroller is a ";
		s += uC;
		s += ", NOT a RP2xxx!\n";
	}
	debug_messages_text->setText("");
	send_data_text->setText("");
	debug_messages_text->appendText(x);
	debug_messages_text->appendText(u);
	if(show_macro)
		debug_messages_text->appendText(w);
	debug_messages_text->appendText(s);
	debug_messages_text->setBottomLine(INT_MAX);

	return 1;
}

void
MainWindow::check_eeprom_changed(void)
{
	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_EEPROM_DIRTY); // hex!
	send_data_text->setText(s);
	Write_and_Check(4, 5);
	if(buf[4]){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("eeprom was changed"),"%s",tr("Discard changes to eeprom? Otherwise press 'commit'"))==MBOX_CLICKED_NO);
	}
}

long
MainWindow::onDisconnect(FXObject *sender, FXSelector sel, void *ptr)
{
	hid_close(connected_device);
	connected_device = NULL;
	connected_label->setText("Disconnected");
	connected_label2->setText("Firmware:");
	connected_label3->setText("Protocols:");
	protocols = "";
	firmware = "";
	firmware1 = "";
	max = 0;
	count = 0;
	select_wakeup->clearItems();
	select_macro_nr->clearItems();
	select_macro_slot->clearItems();
	select_repeat->clearItems();
	get_alarm_button->disable();
	set_alarm_button->disable();
	commit_eeprom_button->disable();
	connect_button->enable();
	disconnect_button->disable();
	set_eeprom_button->disable();
	get_caps_button->disable();
	get_eeprom_button->disable();
	getraw_eeprom_button->disable();
	get_irdata_button->disable();
	get_key_button->disable();
	get_macro_button->disable();
	get_repeat_button->disable();
	get_wakeup_button->disable();
	open_button->disable();
	send_data_button->disable();
	set_irdata_button->disable();
	set_key_button->disable();
	set_macro_button->disable();
	set_repeat_button->disable();
	setbyremote_irdata_button->disable();
	setbyremote_keyboard_irdata_button->disable();
	setbyremote_macro_button->disable();
	setbyremote_wakeup_button->disable();
	set_wakeup_button->disable();
	reset_alarm_button->disable();
	receive_ir_button->disable();
	reboot_button->disable();
	reset_eeprom_button->disable();
	reset_irdata_button->disable();
	reset_key_button->disable();
	reset_macro_button->disable();
	reset_repeat_button->disable();
	reset_wakeup_button->disable();
	save_button->disable();
	send_ir_button->disable();
	getApp()->removeTimeout(this, ID_KBD_TIMER);
	getApp()->removeTimeout(this, ID_READIR_TIMER);
	getApp()->removeTimeout(this, ID_RED_TIMER);

	return 1;
}

long
MainWindow::onRescan(FXObject *sender, FXSelector sel, void *ptr)
{
	//check_eeprom_changed();
	// the selected device's position in the list may change, so make a new onConnect() mandatory
	onDisconnect(NULL, 0, NULL);

	struct hid_device_info *cur_dev;

	device_list->clearItems();
	
	// List the Devices
	hid_free_enumeration(devices);
	devices = hid_enumerate(0x1209, 0x4446);
	cur_dev = devices;
	while (cur_dev) {
		// select the hidraw device, not the keyboard device
		if(cur_dev->usage == 0x01) {
			// Add it to the List Box.
			FXString s;
			s.format("%04hx:%04hx -", cur_dev->vendor_id, cur_dev->product_id);
			s += FXString(" ") + cur_dev->manufacturer_string;
			s += FXString(" ") + cur_dev->product_string;
			FXListItem *li = new FXListItem(s, NULL, cur_dev);
			device_list->appendItem(li);
		}
		cur_dev = cur_dev->next;
	}

	if (device_list->getNumItems() == 0) {
		device_list->appendItem("*** No Devices Connected ***");
		num_devices_after_rescan = 0;
	}
	else {
		device_list->selectItem(0);
		num_devices_after_rescan = device_list->getNumItems();
	}

	return 1;
}

long
MainWindow::onReboot(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
	send_data_text->setText(s);

	//FXint cur_item = device_list->getCurrentItem();
	//FXint num_devices_before_reboot = device_list->getNumItems();
	Write_and_Check(4, 4);
	onDisconnect(NULL, 0, NULL);

	return 1;
}

size_t
MainWindow::getDataFromTextField(FXTextField *tf, uint8_t *buf, size_t len)
{
	const char *delim = " ,{}\t\r\n";
	FXString data = tf->getText();
	const FXchar *d = data.text();
	size_t i = 0;
	
	// Copy the string from the GUI.
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	
	// For each token in the string, parse and store in buf[].
	char *token = strtok(str, delim);
	while (token) {
		char *endptr = NULL;
		long int val = strtol(token, &endptr, 16); // hex!
		buf[i++] = val;
		if (i > len) {
			FXMessageBox::error(this, MBOX_OK, "Invalid length", "Data field is too long.");
			break;
		}
		token = strtok(NULL, delim);
	}
	
	free(str);
	return i;
}

long
MainWindow::Read(int show_len)
{
	memset(buf, 0, sizeof(buf));
	FXString s;

	int res = hid_read(connected_device, buf, in_size); // nonblocking, must read full length (because of Windows, µC sends full length)!
	
	if (res < 0) {
#ifdef _WIN32
		FXnchar* error = (wchar_t*)(hid_error(connected_device));
#else
		FXwchar* error = (wchar_t*)(hid_error(connected_device));
#endif
		FXMessageBox::error(this, MBOX_OK, "Read error", "%ls", error);
		debug_messages_text->appendText("\nread error: ");
		debug_messages_text->appendText(error);
		debug_messages_text->appendText("\n");
		debug_messages_text->setBottomLine(INT_MAX);
		onRescan(NULL, 0, NULL);
		return -1;
	} else {
		if (res == 0)
			return 0;

		s.format("Received %d bytes:\n", res);
		for (int i = 0; i < show_len; i++) {
			FXString t;
			t.format("%02x ", buf[i]);
			s += t;
		}
		if (buf[0] == REPORT_ID_KBD){
			s += "from remote control";
			if (buf[1] || buf[3]) {
				s += ", modifier: ";
				s += get_modifier_from_hex(buf[1]);
				s += ", key: ";
				s += get_key_from_hex(buf[3]);
			} else {
				s += ", release";
			}
		}
		if (buf[0] == REPORT_ID_IR){
			s += "from remote control";
		}
		s += "\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::onReadIR(FXObject *sender, FXSelector sel, void *ptr)
{
	int read, release = 0;
	read = Read(7);
	if(read == -1)
		return -1;
	else if (read == 0)
		return 0;

	FXString s, t, v;

	if (buf[0] == REPORT_ID_IR) {
		// Repeat Counter
		if(ReceiveActive) {
			if (buf[6] == IRMP_FLAG_NEW) {
				RepeatCounter = 0;
			} else if (buf[6] == IRMP_FLAG_REPETITION) {
				RepeatCounter++;
			} else if (buf[6] == IRMP_FLAG_RELEASE) {
				release = 1;
			}
			FXString u;
			if (!release) {
				u.format("RepeatCounter: %d \n", RepeatCounter);
				debug_messages_text->appendText(u);
			} else
				debug_messages_text->appendText("Release\n");
			debug_messages_text->setBottomLine(INT_MAX);
			receive_ir_button->setBackColor(FXRGB(255,23,23));
			g_main_window->repaint();
			getApp()->addTimeout(this, ID_RED_TIMER, 50 * timeout_scalar /*50ms*/); // three refreshes at 60Hz
		}

		// show received IR
		s = "";
		t.format("%02x", buf[1]);
		s += t;
		protocol_text->setText(s);

		s = ""; t = "";
		t.format("%02x", buf[3]);
		s += t; t = "";
		t.format("%02x", buf[2]);
		s += t;
		address_text->setText(s);

		s = ""; t = "";
		t.format("%02x", buf[5]);
		s += t; t = "";
		t.format("%02x", buf[4]);
		s += t;
		command_text->setText(s);

		s = ""; t = "";
		t.format("%02x", buf[6]);
		s += t;
		flag_text->setText(s);

		if (!release) {
			//translate by map and show
			int k = 0;
			t = protocol_text->getText();
			t += address_text->getText();
			t += command_text->getText();
			t += "00";
			s = "translated:";
			eeprom_map_text->killHighlight();
			for(int i = 0; i < active_lines; i++) {
				if(map[i*2] == t) {
					s += " ";
					s += map[i*2+1];
					k++;
					eeprom_map_text->setHighlight(mapbeg[i], mapbeg[i+1] - mapbeg[i] - 1);
					eeprom_map_text->setCursorPos(mapbeg[i]);
				}
			}
			if(k > 1)
				s += ", WARNING: multiple entries!";
			s += "\n";
			debug_messages_text->appendText(s);
			debug_messages_text->setBottomLine(INT_MAX);
		}
	}

	if (buf[0] == REPORT_ID_KBD && (buf[1] || buf[3])) {
		// show received KBD
		s = get_modifier_from_hex(buf[1]);
		modifier_text->setText(s);
		s = get_key_from_hex(buf[3]);
		key_text->setText(s);
	}

	return 1;
}

long
MainWindow::onReceiveIR(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!ReceiveActive) {
		get_alarm_button->disable();
		set_alarm_button->disable();
		commit_eeprom_button->disable();
		disconnect_button->disable();
		set_eeprom_button->disable();
		get_caps_button->disable();
		get_eeprom_button->disable();
		getraw_eeprom_button->disable();
		get_irdata_button->disable();
		get_key_button->disable();
		get_macro_button->disable();
		get_repeat_button->disable();
		get_wakeup_button->disable();
		open_button->disable();
		send_data_button->disable();
		set_irdata_button->disable();
		set_key_button->disable();
		set_macro_button->disable();
		set_repeat_button->disable();
		setbyremote_irdata_button->disable();
		setbyremote_keyboard_irdata_button->disable();
		setbyremote_macro_button->disable();
		setbyremote_wakeup_button->disable();
		set_wakeup_button->disable();
		reset_alarm_button->disable();
		reboot_button->disable();
		rescan_button->disable();
		reset_eeprom_button->disable();
		reset_irdata_button->disable();
		reset_key_button->disable();
		reset_macro_button->disable();
		reset_repeat_button->disable();
		reset_wakeup_button->disable();
		save_button->disable();
		send_ir_button->disable();
		upgrade_button->disable();
		/* consume IR */
		int read;
		read = Read(7);
		while(read > 0)
			read = Read(7);
		// timer on
		getApp()->addTimeout(this, ID_READIR_TIMER, 5 * timeout_scalar /*5ms*/);
		ReceiveActive = 1;
		receive_ir_button->setBackColor(FXRGB(255,207,207));
		receive_ir_button->setBaseColor(FXRGB(0,0,255));
		receive_ir_button->setShadowColor(makeShadowColor(FXRGB(0,0,255)));
		g_main_window->repaint();
		FXString s;
		s = "receive IR data by pressing buttons on the remote control\n";
		s += "stop receive mode by pressing receive mode button again\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		RepeatCounter = 0;
	} else {
		// timer off
		getApp()->removeTimeout(this, ID_READIR_TIMER);
		getApp()->removeTimeout(this, ID_RED_TIMER);
		eeprom_map_text->killHighlight();
		get_alarm_button->enable();
		set_alarm_button->enable();
		commit_eeprom_button->enable();
		disconnect_button->enable();
		set_eeprom_button->enable();
		get_caps_button->enable();
		get_eeprom_button->enable();
		getraw_eeprom_button->enable();
		get_irdata_button->enable();
		get_key_button->enable();
		get_macro_button->enable();
		get_repeat_button->enable();
		get_wakeup_button->enable();
		open_button->enable();
		send_data_button->enable();
		set_irdata_button->enable();
		set_key_button->enable();
		set_macro_button->enable();
		set_repeat_button->enable();
		setbyremote_irdata_button->enable();
		setbyremote_keyboard_irdata_button->enable();
		setbyremote_macro_button->enable();
		setbyremote_wakeup_button->enable();
		set_wakeup_button->enable();
		reset_alarm_button->enable();
		reboot_button->enable();
		rescan_button->enable();
		reset_eeprom_button->enable();
		reset_irdata_button->enable();
		reset_key_button->enable();
		reset_macro_button->enable();
		reset_repeat_button->enable();
		reset_wakeup_button->enable();
		save_button->enable();
		send_ir_button->enable();
		upgrade_button->enable();
		ReceiveActive = 0;
		receive_ir_button->setBaseColor(storedBaseColor);
		receive_ir_button->setShadowColor(storedShadowColor);
		receive_ir_button->setBackColor(storedBackColor);
		g_main_window->repaint();
	}

	return 1;
}

long
MainWindow::Write(int out_len)
{
	FXString s;
	memset(bufw, 0, sizeof(bufw));
	FXint send_data_text_len = getDataFromTextField(send_data_text, bufw, sizeof(bufw));
	if(out_len > send_data_text_len)
		out_len = send_data_text_len;

	FXlong time = FXThread::time() / 1000000; // ms
	uint32_t timestamp = (uint32_t)(time - starttime);

	bufw[43] = timestamp >> 24;
	bufw[44] = (timestamp >> 16) & 0xFF;
	bufw[45] = (timestamp >> 8) & 0xFF;
	bufw[46] = timestamp & 0xFF;

	int res = hid_write(connected_device, bufw, out_len); // may write arbitrary length
	if (res < 0) {
#ifdef _WIN32
		FXnchar* error = (wchar_t*)(hid_error(connected_device));
#else
		FXwchar* error = (wchar_t*)(hid_error(connected_device));
#endif
		FXMessageBox::error(this, MBOX_OK, "Write error", "%ls", error);
		debug_messages_text->appendText("\nwrite error: ");
		debug_messages_text->appendText(error);
		debug_messages_text->appendText("\n");
		debug_messages_text->setBottomLine(INT_MAX);
		onRescan(NULL, 0, NULL);
		return -1;
	} else {
		s.format("Sent %d bytes:\n", res);
		for (int i = 0; i < out_len; i++) {
			FXString t;
			t.format("%02x ", bufw[i]);
			s += t;
		}
		s += "\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::Write_and_Check(int out_len, int show_len)
{
	FXString s;
	int read, count = 0;
	long retVal = 1;
	s = "";

#if (0)
	// before writing first empty buffers and read away old stuff
	read = Read(show_len);
	if(read  == -1) {
		s += "W&C first Read(): -1\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		return -1;
	}

	while ((buf[0] == REPORT_ID_CONFIG_IN) && count < 200) {
		read = Read(show_len);
		if(read == -1) {
			s = "W&C loop Read(): -1\n";
			debug_messages_text->appendText(s);
			debug_messages_text->setBottomLine(INT_MAX);
			return -1;
		}
		s = "cleared read buffer\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		count++;
		FXThread::sleep(3000000); // 3ms
	}
#endif

	if(Write(out_len) == -1)
		return -1;

	FXThread::sleep(3000000); // 3ms

	read = Read(show_len);
	if(read  == -1)
		return -1;

	//TODO consider making Write_and_Check a background thread in order to stay responsive to user interactions, otherwise the user just has to wait during "set by remote"
	while ((buf[0] == REPORT_ID_KBD || buf[0] == REPORT_ID_IR || read == 0) && count < 5500 ) { // over 5 sec for "set by remote", Read() is nonblocking
		//printf("buf[0] %d, read %d, loop %d\n", buf[0], read, count);
		FXThread::sleep(1000000); // 1ms
		read = Read(show_len);
		if(read == -1)
			return -1;
		count++;
	}

#if (0)
	while(buf[3] != CMD_CAPS  && (buf[43] != bufw[43] || (buf[44] != bufw[44]) || (buf[45] != bufw[45]) || (buf[46] != bufw[46])) && count < 200) {
		s += "*****************WRONG TIMESTAMP*********************\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);

		FXThread::sleep(3000000); // 3ms
		read = Read(show_len);
		if(read == -1) {
			s += "W&C loop Read(): -1\n";
			debug_messages_text->appendText(s);
			debug_messages_text->setBottomLine(INT_MAX);
			return -1;
		}
		count++;
	}
#endif

	if((buf[0] == REPORT_ID_CONFIG_IN) && (buf[1] == STAT_SUCCESS) && (buf[2] == bufw[2]) && (buf[3] == bufw[3])) {
		s += "************************OK***************************\n";
		retVal = 1;
	} else {
		s += "**********************ERROR**************************\n";
		retVal = -1;
	}
	debug_messages_text->appendText(s);
	debug_messages_text->setBottomLine(INT_MAX);

	return retVal;
}

long
MainWindow::onSendData(FXObject *sender, FXSelector sel, void *ptr)
{
	Write_and_Check(64, 64);

	return 1;
}

long
MainWindow::onSetWakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	t.format("%d ", select_wakeup->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE);
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(11, 4);

	return 1;
}

long
MainWindow::onSetMacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
t.format("%d ", select_macro_nr->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO);
	s += t;
	t.format("%d ", select_macro_slot->getCurrentItem());
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(12, 4);

	return 1;
}

long
MainWindow::onSetIrdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA);
	s += t;
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(11, 4);

	onGetIrdata(NULL, 0, NULL);

	if(eeprom_map_text->isModified())
		onApply(NULL, 0, NULL);
	int i = 0;
	FXint pos = eeprom_map_text->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	eeprom_map_text->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += "00";
	eeprom_map_text->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(mapbeg[i]);
	eeprom_map_text->setModified(1);

	return 1;
}

long
MainWindow::onSetKey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_KEY);
	s += t;
#if (FOX_MINOR >= 7)
	t.fromUInt(get_hex_from_key((key_text->getText())),16);
#else
	t = FXStringVal(get_hex_from_key((key_text->getText())),16);
#endif
#if (FOX_MINOR >= 7)
	u.fromUInt(get_hex_from_key((modifier_text->getText())),16);
#else
	u = FXStringVal(get_hex_from_key((modifier_text->getText())),16);
#endif
	s += t;
	s += " ";
	s += u;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(7, 4);

	onGetKey(NULL, 0, NULL);

	int i = 0;
	FXint pos = eeprom_map_text->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(eeprom_map_text->isModified())
		onApply(NULL, 0, NULL);
	eeprom_map_text->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	eeprom_map_text->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(mapbeg[i-1]);
	eeprom_map_text->setModified(1);

	return 1;
}

long
MainWindow::onSetRepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REPEAT);
	u.format("%x ", select_repeat->getCurrentItem());
#if (FOX_MINOR >= 7)
	t.format("%x %x", repeat_text->getText().toUInt() & 0xFF, (repeat_text->getText().toUInt() >> 8) & 0xFF );
#else
	t.format("%x %x", FXUIntVal(repeat_text->getText(), 10) & 0xFF, (FXUIntVal(repeat_text->getText(), 10) >> 8) & 0xFF);
#endif
	s += u;
	s += " ";
	s += t;
	send_data_text->setText(s);

	Write_and_Check(7, 4);

	return 1;
}

long
MainWindow::onSetByRemoteWakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	debug_messages_text->appendText(s);
	debug_messages_text->setBottomLine(INT_MAX);
	getApp()->repaint();
	t.format("%x ", select_wakeup->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_WAKE_REMOTE);
	s += t;
	send_data_text->setText(s);

	if(Write_and_Check(5, 4) == -1){
		s = "wakeup timeout\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		return -1;
	}

	onGetWakeup(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onSetByRemoteMacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	debug_messages_text->appendText(s);
	debug_messages_text->setBottomLine(INT_MAX);
	getApp()->repaint();
	t.format("%d ", select_macro_nr->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_MACRO_REMOTE);
	s += t;
	t.format("%d ", select_macro_slot->getCurrentItem());
	s += t;
	send_data_text->setText(s);

	getApp()->repaint();

	if(Write_and_Check(6, 4) == -1){
		s = "macro timeout\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		return -1;
	}

	onGetMacro(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onSetByRemoteIrdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, p, a, c;
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	if(template_mode){
		s = "we are in loop mode, in order to exit wait for timeout or press same button again\n";
		s += "enter IR data by pressing a button on the remote control within 5 sec\n";
		setbyremote_kbd_irdata_text->setText("press button on remote");
		setbyremote_kbd_irdata_text_2->setText("or wait until timeout");
	} else {
		s = "enter IR data by pressing a button on the remote control within 5 sec\n";
	}
	debug_messages_text->appendText(s);
	debug_messages_text->setBottomLine(INT_MAX);
	getApp()->repaint();
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA_REMOTE);
	s += t;
	send_data_text->setText(s);

	getApp()->repaint();

	if(Write_and_Check(5, 4) == -1){
		s = "irdata timeout\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		if(template_mode){
			setbyremote_kbd_irdata_text->setText("");
			setbyremote_kbd_irdata_text_2->setText("");
		}
		return -1;
	}

	onGetIrdata(NULL, 0, NULL);

	// is this already in eeprom map?
	for( int i = 0; i < active_lines; i++) {
		eeprom_map_text->extractText(p, mapbeg[i], 2);
		eeprom_map_text->extractText(a, mapbeg[i]+2, 4);
		eeprom_map_text->extractText(c, mapbeg[i]+6, 4);
#if (FOX_MINOR >= 7)
		if((i != line_text->getText().toInt() - 1) &&
#else
		if((i != FXIntVal(line_text->getText(), 10) - 1) &&
#endif
		   (protocol_text->getText() == p) &&
		   (address_text->getText() == a) &&
		   (command_text->getText() == c)) {
			s = "warning: irdata ";
			s += p;
			s += a;
			s += c;
			s += "00";
			s += " already in eeprom map line ";
#if (FOX_MINOR >= 7)
			t.fromUInt(i + 1, 10);
#else
			t = FXStringVal(i + 1, 10);
#endif
			s += t;
			s += "\n";
			debug_messages_text->appendText(s);
			debug_messages_text->setBottomLine(INT_MAX);
			if(template_mode){
				setbyremote_kbd_irdata_text->setText("");
				setbyremote_kbd_irdata_text_2->setText("");
			}
			return -1;
		}
	}

	int i = 0;
	FXint pos = eeprom_map_text->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(eeprom_map_text->isModified())
		onApply(NULL, 0, NULL);
	eeprom_map_text->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += "00";
	eeprom_map_text->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(mapbeg[i]);
	eeprom_map_text->setModified(1);

	if(template_mode)
		getApp()->addTimeout(this, ID_SETBYREMOTE_IRDATA_TIMER, 300 * timeout_scalar /* 300 ms*/); // avoid bounces

	return 1;
}

long
MainWindow::onGetWakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%x", select_wakeup->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_WAKE);
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 10);

	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGetMacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", select_macro_nr->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_MACRO);
	s += t;
	t.format("%d", select_macro_slot->getCurrentItem());
	s += t;
	send_data_text->setText(s);

	Write_and_Check(6, 10);

	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);

	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGetIrdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_IRDATA);
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 10);
	
	s = "";
	t.format("%02x", buf[4]);
	s += t;
	protocol_text->setText(s);
		
	s = "";
	t.format("%02x", buf[6]);
	s += t;
	t.format("%02x", buf[5]);
	s += t;
	address_text->setText(s);

	s = "";
	t.format("%02x", buf[8]);
	s += t;
	t.format("%02x", buf[7]);
	s += t;
	command_text->setText(s);

	s = "";
	t.format("%02x", buf[9]);
	s += t;
	flag_text->setText(s);

	return 1;
}

long
MainWindow::onGetKey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, v;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_KEY);
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 6);

	s = get_key_from_hex(buf[5]);
	if (s == "error"){
		t = "invalid hid_modifier ";
#if (FOX_MINOR >= 7)
		v.fromUInt(buf[5],16);
#else
		v = FXStringVal(buf[5],16);
#endif
		t += v;
		FXMessageBox::error(this, MBOX_OK, t.text(), "got invalid hid_modifier");
	}
	modifier_text->setText(s);

	s = get_key_from_hex(buf[4]);
	if (s == "error"){
		t = "invalid hid_key ";
#if (FOX_MINOR >= 7)
		v.fromUInt(buf[5],16);
#else
		v = FXStringVal(buf[5],16);
#endif
		t += v;
		FXMessageBox::error(this, MBOX_OK, t.text(), "got invalid hid_key");
	}
	key_text->setText(s);

	return 1;
}

long
MainWindow::onGetRepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_REPEAT);
	u.format("%x ", select_repeat->getCurrentItem());
	s += u;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(5, 6);

#if (FOX_MINOR >= 7)
	t.fromUInt(*((uint16_t*)&buf[4]),10);
#else
	t = FXStringVal(*((uint16_t*)&buf[4]),10);
#endif
	repeat_text->setText(t);

	return 1;
}

long
MainWindow::onGetCaps(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s, t, u;
	int jump_to_firmware, romtable;
	jump_to_firmware = 0;
	romtable = 0;
	uC = "";
	for(int i = 0; i < 20; i++) { // for safety stop after 20 loops
		s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_CAPS);
#if (FOX_MINOR >= 7)
		t.fromInt(i,16);
		s += t;
#else
		s += FXStringVal(i,16);
#endif
		s += " ";
		send_data_text->setText(s);

		Write_and_Check(5, i == 0 ? 9 : in_size);

		if (!i) { // first query for slots and depth
			irdatanr = buf[4];
			s.format("number of irdata: %u\n", buf[4]);
			macroslots = buf[9];
			t.format("number of macros: %u\n", buf[9]);
			s += t;
			macrodepth = buf[10];
			t.format("macro depth: %u\n", buf[10]);
			s += t;
			wakeupslots = buf[6];
			t.format("number of wakeups: %u\n", buf[6]);
			s += t;
			t.format("hid in report count: %u\n", in_size);
			s += t;
			t.format("hid out report count: %u\n", out_size);
			s += t;
		} else {
			if (!jump_to_firmware) { // queries for supported_protocols
				s = "protocols: \n";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						debug_messages_text->appendText(s);
						debug_messages_text->setBottomLine(INT_MAX);
						jump_to_firmware = 1;
						goto again;
					}
					t.format("%u ", buf[k]);
					protocols += t;
					u = protocol[buf[k]];
					s += t;
					s += u;
					s += "\n";
				}
			} else { // queries for firmware
				s = "firmware: ";
				for (int k = 4; k < in_size; k++) {
					if (!buf[k]) { // NULL termination
						s += "\n";
						debug_messages_text->appendText(s);
						debug_messages_text->setBottomLine(INT_MAX);
						return 1;
					}
					if (buf[k] == 42) { // * separator
						romtable = 1;
						firmware += "   uC: ";
						s += "*";
					} else {
						t.format("%c", buf[k]);
						firmware += t;
						if(romtable)
							uC += t;
						s += t;
					}
				}
			}
		}
		s += "\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
again:	;
	}
	return 1;
}

long
MainWindow::onGetAlarm(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_ALARM);
	send_data_text->setText(s);

	Write_and_Check(4, 8);

	unsigned int alarm = *((uint32_t *)&buf[4]);

	FXString t;	
	s = "";
	t.format("%u", alarm/60/60/24);
	s += t;
	days_text->setText(s);
		
	s = "";
	t.format("%d", (alarm/60/60) % 24);
	s += t;
	hours_text->setText(s);

	s = "";
	t.format("%d", (alarm/60) % 60);
	s += t;
	minutes_text->setText(s);

	s = "";
	t.format("%d", alarm % 60);
	s += t;
	seconds_text->setText(s);

	return 1;
}

long
MainWindow::onSetAlarm(FXObject *sender, FXSelector sel, void *ptr)
{
	unsigned int setalarm = 0;
	FXString u = "";
#if (FOX_MINOR >= 7)
	setalarm += 60 * 60 * 24 * days_text->getText().toUInt();
	setalarm += 60 * 60 * hours_text->getText().toUInt();
	setalarm += 60 * minutes_text->getText().toUInt();
	setalarm += seconds_text->getText().toUInt();
#else
	setalarm += 60 * 60 * 24 * FXUIntVal(days_text->getText(), 10);
	setalarm += 60 * 60 * FXUIntVal(hours_text->getText(), 10);
	setalarm += 60 * FXUIntVal(minutes_text->getText(), 10);
	setalarm += FXUIntVal(seconds_text->getText(), 10);
#endif
	if(setalarm < 2) {
		setalarm = 2;
		u = "set alarm to 2 in order to prevent device or program hangup\n";
	 }

	FXString s;
	FXString t;
	const char *z = " ";
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_ALARM);
#if (FOX_MINOR >= 7)
	t.fromUInt(setalarm,16);
#else
	t = FXStringVal(setalarm, 16);
#endif
	int len = t.length();
	for (int i = 0; i < 8 - len; i++) {
		t.prepend("0");
	}
	t.insert(2, " ");
	t.insert(5, " ");
	t.insert(8, " ");
	s += t.section(z, 3, 1);
	s += " ";
	s += t.section(z, 2, 1);
	s += " ";
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(8, 4);

	debug_messages_text->appendText(u);
	debug_messages_text->setBottomLine(INT_MAX);
	
	return 1;
}

long
MainWindow::onResetWakeup(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d", select_wakeup->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_WAKE);
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 4);

	return 1;
}

long
MainWindow::onResetMacro(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	t.format("%d ", select_macro_nr->getCurrentItem());
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_MACRO);
	s += t;
	t.format("%d ", select_macro_slot->getCurrentItem());
	s += t;
	send_data_text->setText(s);

	Write_and_Check(6, 4);

	return 1;
}

long
MainWindow::onResetIrdata(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_IRDATA);
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 4);

	onGetIrdata(NULL, 0, NULL);

	int i = 0;
	FXint pos = eeprom_map_text->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(eeprom_map_text->isModified())
		onApply(NULL, 0, NULL);
	eeprom_map_text->removeText(mapbeg[i-1], map[(i-1)*2].length());
	s = protocol_text->getText();
	s += address_text->getText();
	s += command_text->getText();
	s += flag_text->getText();
	eeprom_map_text->insertText(mapbeg[i-1], s);
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(mapbeg[i]);
	eeprom_map_text->setModified(1);

	return 1;
}

long
MainWindow::onResetKey(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
#if (FOX_MINOR >= 7)
	t.format("%x ", line_text->getText().toUInt() - 1);
#else
	t.format("%x ", FXUIntVal(line_text->getText(), 10) - 1);
#endif
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_KEY);
	s += t;
	send_data_text->setText(s);

	Write_and_Check(5, 4);

	onGetKey(NULL, 0, NULL);

	int i = 0;
	FXint pos = eeprom_map_text->getCursorPos();
	while(mapbeg[i] <= pos) {
		i++;
	}
	if(eeprom_map_text->isModified())
		onApply(NULL, 0, NULL);
	eeprom_map_text->removeText(mapbeg[i-1]+map[(i-1)*2].length()+1, map[(i-1)*2+1].length());
	s = modifier_text->getText();
	s += "|";
	s += key_text->getText();
	eeprom_map_text->insertText(mapbeg[i-1]+map[(i-1)*2].length()+1, s);
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(mapbeg[i]);
	eeprom_map_text->setModified(1);

	return 1;
}

long
MainWindow::onResetRepeat(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t, u;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_REPEAT);
	u.format("%x ", select_repeat->getCurrentItem());
	s += u;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(5, 4);

	return 1;
}

long
MainWindow::onResetAlarm(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_ALARM);
	send_data_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onSendIR(FXObject *sender, FXSelector sel, void *ptr)
{
	FXString s;
	FXString t;
	const char *z = " ";
	int len;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_EMIT);
	t = protocol_text->getText();
	len = t.length(); // don't put this into the for loop!!!
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	t = address_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = command_text->getText();
	len = t.length();
	for (int i = 0; i < 4 - len; i++)
		t.prepend("0");
	t.insert(2, " ");
	s += t.section(z, 1, 1);
	s += " ";
	s += t.section(z, 0, 1);
	s += " ";
	t = flag_text->getText();
	len = t.length();
	for (int i = 0; i < 2 - len; i++)
		t.prepend("0");
	s += t;
	s += " ";
	send_data_text->setText(s);

	Write_and_Check(10, 4);

	return 1;
}

void MainWindow::print_output(const char* format, ...)
{
	FXString message;
	va_list ap;
	va_start(ap, format);
	message.vformat(format, ap);
	debug_messages_text->appendText(message);
	debug_messages_text->setBottomLine(INT_MAX);
	va_end(ap);
	getApp()->repaint();
}

void MainWindow::open_device(void)
{
	struct libusb_device *dev, **devs;
	struct libusb_config_descriptor *config;
	int ret;
	ret = libusb_get_device_list(NULL, &devs);
	if(ret < 0) {
		print_output("error getting device list(): %s\n", libusb_error_name(ret));
		return;
	}

	for (int i=0; (dev=devs[i]) != NULL; i++) {
		struct libusb_device_descriptor desc;
		if(libusb_get_device_descriptor(dev, &desc) < 0) {
			print_output("couldn't get device descriptor\n");
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
			print_output("found %s in boot mode at %d:%d\n", model, libusb_get_bus_number(dev), libusb_get_device_address(dev));
			break;
		}
	}

	libusb_free_device_list(devs, 1);

	if(libusb_get_active_config_descriptor(dev, &config) != LIBUSB_SUCCESS) {
		print_output("couldn't get config descriptor\n");
		return;
	}
	ret = libusb_open(dev, &dev_handle);
	if(ret < 0) {
		print_output("error opening device: %s\n", libusb_error_name(ret));
		goto error;
	}
	//print_output("opened device\n");
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
		//print_output("found interface\n");
		ret = libusb_claim_interface(dev_handle, interface);
		if (ret != LIBUSB_SUCCESS) {
			print_output("error claiming m_interface: %s\n", libusb_error_name(ret));
			libusb_close(dev_handle);
			dev_handle = NULL;
			goto error;
		}
		//print_output("claimed interface\n");
	}
error:
	libusb_free_config_descriptor(config);
}

uint8_t * MainWindow::get_firmware(const char *firmwarefile, int *firmwareSize)
{
	FILE *fpFirmware;
	uint8_t *fw_buf;

	fpFirmware = fopen (firmwarefile, "rb");
	if(fpFirmware == NULL) {
		print_output("error opening firmware file: %s\n", firmwarefile);
		return NULL;
	} else {
#ifdef WIN32
		FXCP1252Codec codec;
		FXString utfstring=codec.mb2utf(firmwarefile);
		print_output("opened firmware file %s\n", utfstring.text());
#else
		print_output("opened firmware file %s\n", firmwarefile);
#endif
	}

	if((fseek(fpFirmware, 0, SEEK_END) != 0) || ((*firmwareSize = ftell(fpFirmware)) < 0) ||
							(fseek(fpFirmware, 0, SEEK_SET) != 0)) {
		print_output("error determining firmware size\n");
		fclose(fpFirmware);
		return NULL;
	}

	fw_buf = (uint8_t*)malloc(*firmwareSize);
	if (fw_buf == NULL) {
		fclose(fpFirmware);
		print_output("error allocating memory\n");
		return NULL;
	}

	if(fread(fw_buf,*firmwareSize,1,fpFirmware) != 1) {
		print_output("read firmware error\n");
		fclose(fpFirmware);
		free(fw_buf);
		return NULL;
	} else {
		print_output("read %d bytes of firmware\n", *firmwareSize);
	}

	fclose(fpFirmware);
	return fw_buf;
}

int MainWindow::picoflash(char const* firmwarefile)
{
	int offset;
	int firmwareSize;
	uint8_t *fw_buf;
	int ret;
	uint32_t sram_end = 0;

	print_output("===  Pico Firmware Upgrade  ===\n");

	if(!(fw_buf = get_firmware(firmwarefile, &firmwareSize)))
		return -1;

	uint8_t read_buf[firmwareSize];

	ret = libusb_init(NULL);
	if(ret < 0) {
		print_output("Error initializing libusb: %s\n", libusb_error_name(ret));
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
		print_output("Progress: %d%%\n", MIN((offset+4096)*100/firmwareSize, 100));
		fflush(stdout);
	}

	if (!memcmp(fw_buf, read_buf, firmwareSize))
		print_output("===  verify successful  ===\n");

	if (strcmp(model,"RP2040"))
		sram_end = SRAM_END_RP2040;
	else if (strcmp(model,"RP2350"))
		sram_end = SRAM_END_RP2350;

	picoboot_reboot(dev_handle, 0, sram_end, 500);
	picoboot_exclusive_access(dev_handle, 0);

	libusb_release_interface(dev_handle, 0);
	libusb_close(dev_handle);
	libusb_unref_device(device);
	libusb_exit(NULL);
	free(fw_buf);

	print_output("===  firmware upgrade successful  ===\n");
	fflush(stdout);

	return 0;
}

long
MainWindow::onUpgrade(FXObject *sender, FXSelector sel, void *ptr)
{
		const FXchar patterns[]="All Files (*)\nFirmware Files (*.bin)";
		FXString s, v, Filename, FilenameText;
		FXFileDialog open(this,"Open a firmware file");
		open.setPatternList(patterns);
		open.setCurrentPattern(1);
		if(open.execute()){
			Filename = open.getFilename();
			FXint pos = Filename.rfind(PATHSEP);
			FXint endpos = Filename.length();
			FXint suffix_length = open.getCurrentPattern() ? 4 : 0;
			FXString Firmwarename = Filename.mid(pos + 1, endpos - pos - 1 - suffix_length);
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Really upgrade?","Old Firmware: %s\nNew Firmware: %s", firmware1.text(),  Firmwarename.text())) return 1;

			cur_item = device_list->getCurrentItem();
			num_devices_before_upgrade = device_list->getNumItems();
			s.format("%x %x %x %x", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_REBOOT);
			send_data_text->setText(s);
			if(connected_device)
				Write_and_Check(4, 4);
			onDisconnect(NULL, 0, NULL);
			v = "The Pico is going to be switched into mass storage device mode and will then be flashed.\n";
			v += "This takes a minute, please wait until reconnect.\n\n";
			debug_messages_text->appendText(v);
			debug_messages_text->setBottomLine(INT_MAX);
			getApp()->repaint();
#if (0)
			char sys[512];
#ifdef WIN32
			FXThread::sleep(5000000000); // 5 s
			FXString pwd = FX::FXSystem::getCurrentDirectory();
			v = pwd;
			//v += "\\picotool load -v -x ";
			v += "\\picoflash ";
			v += Filename;
			debug_messages_text->appendText(v);
			debug_messages_text->setBottomLine(INT_MAX);
			FXCP1252Codec codec;
			v =codec.utf2mb(v); // on Windows file encoding is cp1252, needed for umlaut
#else
			FXThread::sleep(2000000000); // 2 s works, 1 s is too fast
			//v = "/usr/local/bin/picotool load -v -x ";
			FXString pwd = FX::FXSystem::getCurrentDirectory();
			v = pwd;
			//v = "/usr/local/bin/picotool load -v -x ";
			v += "/picoflash ";
			v += Filename;
			debug_messages_text->appendText(v);
			debug_messages_text->setBottomLine(INT_MAX);
#endif
			sprintf(sys, "%s", v.text()); // system needs const char*
			int status = system(sys);
			v = "\n\nstatus: ";
			v += status ? "error" : "OK";
			debug_messages_text->appendText(v);
			debug_messages_text->setBottomLine(INT_MAX);
			if (status != 0) return 0;

			v = "\n\n=== Firmware Upgrade successful ===\n";
			debug_messages_text->appendText(v);
			debug_messages_text->setBottomLine(INT_MAX);
		}
#else
#ifdef WIN32
			FXThread::sleep(3000000000); // 3 s
			v = Filename;
			FXCP1252Codec codec;
			v =codec.utf2mb(v); // on Windows file encoding is cp1252, needed for umlaut
			picoflash(v.text());
#else
			FXThread::sleep(2000000000); // 2 s works, 1 s is too fast
			picoflash(Filename.text());
#endif
			FXThread::sleep(2000000000); // 2 s
			onRescan(NULL, 0, NULL);
			FXThread::sleep(5000000000); // 5 s, enough time to see the result
			onConnect(NULL, 0, NULL);
		}
#endif

	return 1;
}

long
MainWindow::onOpen(FXObject *sender, FXSelector sel, void *ptr)
{
	if(eeprom_map_text->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),"%s",tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	long loaded = 0;
	FXint size = 0;
	FXint n;
	FXFileDialog open(this,"Open a map file");
	open.setPatternList(patterns);
	open.setCurrentPattern(1);
	if(open.execute()){
		eeprom_map_text->setText(NULL,0);
		FXString file=open.getFilename();
		FXFile textfile(file,FXFile::Reading);
		// Opened file?
		if(textfile.isOpen()){
			FXchar *text;

			// Get file size
			size=textfile.size();

			// Make buffer to load file
			if(allocElms(text,size)){

				// Set wait cursor
				getApp()->beginWaitCursor();

				// Read the file
				n=textfile.readBlock(text,size);
				if(0<=n){

					// Set text
					eeprom_map_text->setText(text,n);

					// Success
					loaded=1;

					template_mode = 1;
				}

				// Kill wait cursor
				getApp()->endWaitCursor();

				// Free buffer
				freeElms(text);
			}
		}

		eeprom_map_text->setModified(0);
		FXString u;
		FXString v;
		u = "opened: ";
		v = open.getFilename().text();
		u += v;
		u += "\n";
		u += "opened size: ";
#if (FOX_MINOR >= 7)
		v.fromInt(size, 10);
#else
		v = FXStringVal(size, 10);
#endif
		u += v;
		u += "\n";
		u += "entered loop mode for 'set by remote - irdata'\n";
		debug_messages_text->appendText(u);
		debug_messages_text->setBottomLine(INT_MAX);
		onApply(NULL, 0, NULL);
    }

	return loaded;
}

long
MainWindow::onSave(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nmap Files (*.map)";
	FXFileDialog save(this,"save the map file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".map") && (save.getCurrentPattern() == 1))
			file += ".map";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		eeprom_map_text->setModified(0);
		FXString u;
		u = "save eeprom map to ";
		u += file;
		u += "\n";
		debug_messages_text->appendText(u);
		debug_messages_text->setBottomLine(INT_MAX);
    }

	return 1;
}

long
MainWindow::onSaveLog(FXObject *sender, FXSelector sel, void *ptr){
	const FXchar patterns[]="All Files (*)\nlog Files (*.log)";
	FXFileDialog save(this,"save the log file");
	FXString file;
	save.setPatternList(patterns);
	save.setCurrentPattern(1);
	if(save.execute()){
		file=save.getFilename();
		if(compare(file.right(4), ".log") && (save.getCurrentPattern() == 1))
			file += ".log";
		if(FXStat::exists(file)){
			if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,tr("Overwrite Document"),tr("Overwrite existing document: %s?"),file.text())) return 1;
		}
		if(!saveLogFile(file)){
			getApp()->beep();
			FXMessageBox::error(this,MBOX_OK,tr("Error Saving File"),tr("Unable to save file: %s"),file.text());
			return 1;
		}
		FXString u;
		u = "save debug messages to ";
		u += file;
		u += "\n";
		debug_messages_text->appendText(u);
		debug_messages_text->setBottomLine(INT_MAX);
    }

	return 1;
}

long 
MainWindow::saveFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=eeprom_map_text->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from editor
			eeprom_map_text->getText(text,size);

			// Write the file
			n=textfile.writeBlock(text,size);
			if(n==size){

				// Success
				saved=1;
			}

			// Kill wait cursor
			getApp()->endWaitCursor();

			// Free buffer
			freeElms(text);

			FXString u;
			FXString v;
			u = "saved size: ";
#if (FOX_MINOR >= 7)
			v.fromInt(size, 10);
#else
			v = FXStringVal(size, 10);
#endif
			u += v;
			u += "\n";
			debug_messages_text->appendText(u);
			debug_messages_text->setBottomLine(INT_MAX);
		}
	}
	return saved;
}

long
MainWindow::saveLogFile(const FXString& file){
	FXFile textfile(file,FXFile::Writing);
	long saved=0;

	// Opened file?
	if(textfile.isOpen()){
		FXchar *text; FXint size,n;

		// Get size
		size=debug_messages_text->getLength();

		// Alloc buffer
		if(allocElms(text,size+1)){

			// Set wait cursor
			getApp()->beginWaitCursor();

			// Get text from debug messages field
			debug_messages_text->getText(text,size);

			// Write the file
			n=textfile.writeBlock(text,size);
			if(n==size){

				// Success
				saved=1;
			}

			// Kill wait cursor
			getApp()->endWaitCursor();

			// Free buffer
			freeElms(text);

			FXString u;
			FXString v;
			u = "saved size: ";
#if (FOX_MINOR >= 7)
			v.fromInt(size, 10);
#else
			v = FXStringVal(size, 10);
#endif
			u += v;
			u += "\n";
			debug_messages_text->appendText(u);
			debug_messages_text->setBottomLine(INT_MAX);
		}
	}
	return saved;
}

uint8_t
MainWindow::get_hex_from_key(FXString s){
	for(int i=0; i < lines; i++) {
		if(!compare(mapusb[i].key, s)) {
			return mapusb[i].usb_hid_key;
		}
	}
	FXString t;
	t = "invalid key ";
	t += s;
	FXMessageBox::error(this, MBOX_OK, t.text(), "only keyboard keys are possible");
	return 0xFF;
}

FXString
MainWindow::get_key_from_hex(uint8_t hex){
	for(int i=0; i < lines; i++) {
		if(hex == mapusb[i].usb_hid_key) {
			return mapusb[i].key;
		}
	}
	return "error";
}

uint8_t
MainWindow::get_hex_from_modifier(FXString s){
	for(int i=0; i < 10; i++) {
		if(!compare(modifier[i].key, s)) {
			return modifier[i].usb_hid_key;
		}
	}
	FXString t;
	t = "invalid key ";
	t += s;
	FXMessageBox::error(this, MBOX_OK, t.text(), "only keyboard keys are possible");
	return 0xFF;
}

FXString
MainWindow::get_modifier_from_hex(uint8_t hex){
	for(int i=0; i < 10; i++) {
		if(hex == modifier[i].usb_hid_key) {
			return modifier[i].key;
		}
	}
	return "error";
}

long
MainWindow::onGetEeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(eeprom_map_text->isModified()){
		if(FXMessageBox::question(this,MBOX_YES_NO,tr("map was changed"),"%s",tr("Discard changes to map?"))==MBOX_CLICKED_NO) return 1;
	}
	eeprom_map_text->setText(NULL,0);
	for(int i = 0; i < irdatanr; i++) {
	    FXString u;
#if (FOX_MINOR >= 7)
	    FXString v;
	    v.fromUInt(i + 1,10);
	    u += v;
#else
	    u += FXStringVal(i + 1,10);
#endif
	    line_text->setText(u);
	    FXThread::sleep(3000000); // 3ms
	    onGetIrdata(NULL, 0, NULL);
	    FXString s;
	    FXString t;
	    s = protocol_text->getText();
	    t = address_text->getText();
	    s += t;
	    t = command_text->getText();
	    s += t;
	    s += flag_text->getText();
	    s += " ";
	    FXThread::sleep(3000000); // 3ms
	    onGetKey(NULL, 0, NULL);
	    s += modifier_text->getText();
	    s += "|";
	    s += key_text->getText();
	    if(i < irdatanr - 1)
		s += "\n";
	    eeprom_map_text->appendText(s);
	}
	protocol_text->setText("");
	address_text->setText("");
	command_text->setText("");
	flag_text->setText("");
	key_text->setText("KEY_");
	modifier_text->setText("ff");
	onApply(NULL, 0, NULL);
	eeprom_map_text->setCursorPos(0);
	eeprom_map_text->setModified(0);
	template_mode = 0;
	debug_messages_text->appendText("stopped loop mode for 'set by remote - irdata'\n");
	debug_messages_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onSetEeprom(FXObject *sender, FXSelector sel, void *ptr){

	int res = onApply(NULL, 0, NULL); // make sure to apply all changes to eeprom map before flashing
	if(!res)
		return 0;

	FXString nr, nrp;
	for(int i = 0; i < active_lines; i++) {
		if(i >= irdatanr) {
			nr = "too many lines\n";
			debug_messages_text->appendText(nr);
			debug_messages_text->setBottomLine(INT_MAX);
		return 1;
		}

#if (FOX_MINOR >= 7)
		nrp.fromUInt(i + 1,10);
#else
		nrp = FXStringVal(i + 1,10);
#endif
		line_text->setText(nrp);

#if (FOX_MINOR >= 7)
		nr.fromUInt(i,16);
#else
		nr = FXStringVal(i,16);
#endif

		FXString s, u, t, v;
		eeprom_map_text->extractText(u, mapbeg[i], map[i*2].length());
		//if(compare(u, "ffffffffffff")) { // flash only if not ffffffffffff
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_IRDATA);
			s += nr;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i], 2); // TODO check if number
			s += u;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i]+4, 2);
			s += u;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i]+2, 2);
			s += u;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i]+8, 2);
			s += u;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i]+6, 2);
			s += u;
			s += " ";
			eeprom_map_text->extractText(u, mapbeg[i]+10, 2);
			s += u;
			send_data_text->setText(s);

			FXThread::sleep(3000000); // 3ms
			Write_and_Check(11, 4);
		//}

		eeprom_map_text->extractText(u, mapbeg[i] + map[i*2].length() + 1, map[i*2+1].length());
		// remove #comment from KEY_X|KEY_Y#comment
		const char *z = "#";
		u = u.section(z, 0, 1);
		// split KEY_X|KEY_Y
		const char *y = "|";
#if (FOX_MINOR >= 7)
		t.fromUInt(get_hex_from_key(u.section(y, 1, 1)),16); // TODO include line in error message
		v.fromUInt(get_hex_from_key(u.section(y, 0, 1)),16);
#else
		t = FXStringVal(get_hex_from_key(u.section(y, 1, 1)),16);
		v = FXStringVal(get_hex_from_key(u.section(y, 0, 1)),16);
#endif
		//if(compare(t, "0") || compare(v, "0") ) { // flash only if not ff|ff or empty
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_KEY);
			s += nr;
			s += " ";
			s += t;
			s += " ";
			s += v;
			s += " ";
			send_data_text->setText(s);

			FXThread::sleep(3000000); // 3ms
			Write_and_Check(7, 4);
		//}
	}

	//onGetEeprom();

	return 1;
}

long
MainWindow::onResetEeprom(FXObject *sender, FXSelector sel, void *ptr){
	if(FXMessageBox::question(this,MBOX_YES_NO,tr("reset eeprom"),"%s",tr("really reset eeprom?"))==MBOX_CLICKED_NO) return 1;

	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_RESET, CMD_EEPROM_RESET);

	send_data_text->setText(s);

	Write_and_Check(4, 4);

	onGetEeprom(NULL, 0, NULL);

	return 1;
}

long
MainWindow::onCommitEeprom(FXObject *sender, FXSelector sel, void *ptr){
	FXString s;
	s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_SET, CMD_EEPROM_COMMIT);

	send_data_text->setText(s);

	Write_and_Check(4, 4);

	return 1;
}

long
MainWindow::onGetrawEeprom(FXObject *sender, FXSelector sel, void *ptr){
	FXString s, t, u;
	for(int k = 31; k >= 0; k--) { // FLASH_SECTOR_SIZE * nr_sectors / size
		for(int l = 0; l < 16; l++) { // size / 32
			s.format("%x %x %x %x ", REPORT_ID_CONFIG_OUT, STAT_CMD, ACC_GET, CMD_EEPROM_GET_RAW);
#if (FOX_MINOR >= 7)
			t.fromInt(k,16);
			s += t;
#else
			s += FXStringVal(k,16);
#endif
			s += " ";
#if (FOX_MINOR >= 7)
			t.fromInt(l,16);
			s += t;
#else
			s += FXStringVal(l,16);
#endif
			s += " ";
			send_data_text->setText(s);

			Write_and_Check(6, 36);

			for (int i = 4; i < 36; i++) { // 32
				t.format("%02x", buf[i]);
				u += t;
			}
		}
		u += "\n";
	}
	debug_messages_text->appendText(u);
	debug_messages_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onApply(FXObject *sender, FXSelector sel, void *ptr){
	// fill map[] and mapbeg[]
	const char *delim = " \t\r\n"; // Space, Tab, CR and LF
	FXString data = eeprom_map_text->getText();
	const FXchar *d = data.text();
	size_t k = 0;
	size_t sz = strlen(d);
	char *str = (char*) malloc(sz+1);
	strcpy(str, d);
	char *token = strtok(str, delim);
	memset(mapbeg, 0, sizeof(mapbeg));
	int count = 0;
	while (token) {
		map[k] = token;
		count += map[k].length() + 1;
		if(k%2){ //uneven
			mapbeg[(k+1)/2] = count;
		} else { // even
			if(map[k].length() != 12){ // IRMP_DATA
				FXString s, t;
#if (FOX_MINOR >= 7)
				t.fromUInt(k/2+1,10);
#else
				t = FXStringVal(k/2+1,10);

#endif
				s = "map line ";
				s += t;
				s += " invalid";
				FXMessageBox::error(this, MBOX_OK, s.text(), "irdata length needs to be 12");
				return 0;
			}
		}
		token = strtok(NULL, delim);
		k++;
	}
	free(str);
	active_lines = k / 2;

	FXString u;
	FXString v;
	u = "map mapbeg: \n";
	for(int i = 0; i < active_lines; i++) {
		v = map[i*2];
		u += v;
		u += " ";
		v = map[i*2+1];
		u += v;
		u += " ";
#if (FOX_MINOR >= 7)
		v.fromInt(mapbeg[i], 10);
#else
		v = FXStringVal(mapbeg[i],10);
#endif
		u += v;
		u += "\n";
	}
	//debug_messages_text->appendText(u);
	//debug_messages_text->setBottomLine(INT_MAX);

	return 1;
}

long
MainWindow::onClearLog(FXObject *sender, FXSelector sel, void *ptr)
{
	debug_messages_text->setText("");

	return 1;
}

long
MainWindow::onReadirTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	if(onReadIR(NULL, 0, NULL) != -1)
		getApp()->addTimeout(this, ID_READIR_TIMER, 5 * timeout_scalar /*5ms*/);

	return 1;
}

long
MainWindow::onRedTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	receive_ir_button->setBackColor(FXRGB(255,207,207));
	g_main_window->repaint();

	return 1;
}

long MainWindow::onSelectWakeup(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,select_wakeup->getCurrentItem()));
	return 1;
  }

long MainWindow::onSelectMacroNr(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,select_macro_nr->getCurrentItem()));
	return 1;
  }

long MainWindow::onSelectMacroSlot(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,select_macro_slot->getCurrentItem()));
	return 1;
  }

long MainWindow::onSelectRepeat(FXObject*,FXSelector sel,void* ptr){
	FXTRACE((1,"%s: %d (%d)\n",FXSELTYPE(sel)==SEL_COMMAND?"SEL_COMMAND":"SEL_CHANGED",(FXint)(FXival)ptr,select_repeat->getCurrentItem()));
	return 1;
  }


long MainWindow::onDevlistDoubleclicked(FXObject *sender, FXSelector sel, void *ptr){
	check_eeprom_changed();
	onDisconnect(NULL, 0, NULL);
	onConnect(NULL, 0, NULL);
	return 1;
}

long
MainWindow::onMacTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
#ifdef __APPLE__
	check_apple_events();
	
	getApp()->addTimeout(this, ID_MAC_TIMER,
		50 * timeout_scalar /*50ms*/);
#endif

	return 1;
}

FXString
MainWindow::get_key_from_event_code(uint32_t code){
	for(int i=0; i < fxkey_lines; i++) {
		if(code == fxkey_map[i].fx_key) {
			return fxkey_map[i].key;
		}
	}
	return "error";
}

long
MainWindow::onKeyPress(FXObject *sender, FXSelector sel, void *ptr)
{
	FXEvent *event = (FXEvent*)ptr;
	FXString s;

	// first reset
	key_text->setText("KEY_");
	modifier_text->setText("ff");

#if (FOX_MINOR >= 7)
		//s.fromUInt(event->code, 16);
#else
		//s = FXStringVal(event->code,16);
#endif

	if(0xFFE0 < event->code && event->code < 0xFFEF){
		modifier_text->setText(get_key_from_event_code(event->code));
		got_modifier = 1;
		//debug_messages_text->appendText(s);
		//debug_messages_text->appendText(" ");
		debug_messages_text->appendText("got modifier ");
		debug_messages_text->appendText(modifier_text->getText());
		debug_messages_text->appendText("\n");
		debug_messages_text->setBottomLine(INT_MAX);
		getApp()->repaint();
	} else {
		key_text->setText(get_key_from_event_code(event->code));
		got_key = 1;
		//debug_messages_text->appendText(s);
		//debug_messages_text->appendText(" ");
		debug_messages_text->appendText("got key ");
		debug_messages_text->appendText(key_text->getText());
		debug_messages_text->appendText("\n");
		debug_messages_text->setBottomLine(INT_MAX);
		getApp()->repaint();
	}

	return 1;
}

long
MainWindow::onSetByRemoteKeyboardIrdata(FXObject *sender, FXSelector sel, void *ptr)
{
	if (!setbyremote_kbd_irdata_active) {
		setbyremote_kbd_irdata_active = 1;
		setbyremote_keyboard_irdata_button->setBackColor(FXRGB(255,207,207));
		setbyremote_keyboard_irdata_button->setBaseColor(FXRGB(0,0,255));
		setbyremote_keyboard_irdata_button->setShadowColor(makeShadowColor(FXRGB(0,0,255)));
		modifier_text->setText("ff");
		key_text->setText("KEY_EDIT");
		got_key = 0;
		got_modifier = 0;
		address_text->disable();
		get_alarm_button->disable();
		set_alarm_button->disable();
		command_text->disable();
		commit_eeprom_button->disable();
		days_text->disable();
		device_list->disable();
		disconnect_button->disable();
		flag_text->disable();
		set_eeprom_button->disable();
		get_caps_button->disable();
		get_eeprom_button->disable();
		getraw_eeprom_button->disable();
		get_irdata_button->disable();
		get_key_button->disable();
		get_macro_button->disable();
		get_repeat_button->disable();
		get_wakeup_button->disable();
		hours_text->disable();
		debug_messages_text->disable();
		key_text->disable();
		line_text->disable();
		eeprom_map_text->disable();
		minutes_text->disable();
		select_macro_nr->disable();
		modifier_text->disable();
		select_macro_slot->disable();
		open_button->disable();
		send_data_button->disable();
		send_data_text->disable();
		set_irdata_button->disable();
		set_key_button->disable();
		set_macro_button->disable();
		set_repeat_button->disable();
		setbyremote_irdata_button->disable();
		setbyremote_kbd_irdata_text_2->disable();
		setbyremote_kbd_irdata_text->disable();
		setbyremote_macro_button->disable();
		protocol_text->disable();
		setbyremote_wakeup_button->disable();
		set_wakeup_button->disable();
		reset_alarm_button->disable();
		receive_ir_button->disable();
		reboot_button->disable();
		repeat_text->disable();
		rescan_button->disable();
		reset_eeprom_button->disable();
		reset_irdata_button->disable();
		reset_key_button->disable();
		reset_macro_button->disable();
		reset_repeat_button->disable();
		select_repeat->disable();
		reset_wakeup_button->disable();
		save_button->disable();
		seconds_text->disable();
		send_ir_button->disable();
		upgrade_button->disable();
		select_wakeup->disable();

		FXString s;
		s = "entered keyboard + irdata mode\n";
		s += "press the button again in order to stop\n";
		s += "while waiting for irdata you can't leave until irdata reception or timeout (firmware is waiting 5 sec for IR reception)\n";
		debug_messages_text->appendText(s);
		debug_messages_text->setBottomLine(INT_MAX);
		setbyremote_kbd_irdata_text->setText("press modifier or key on keyboard");
		setbyremote_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();

		getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
	} else {
		getApp()->removeTimeout(this, ID_KBD_TIMER);
		setbyremote_kbd_irdata_active = 0;
		setbyremote_keyboard_irdata_button->setBaseColor(storedBaseColor);
		setbyremote_keyboard_irdata_button->setShadowColor(storedShadowColor);
		setbyremote_keyboard_irdata_button->setBackColor(storedBackColor);
		setbyremote_kbd_irdata_text->setText("");
		setbyremote_kbd_irdata_text_2->setText("");

		address_text->enable();
		get_alarm_button->enable();
		set_alarm_button->enable();
		command_text->enable();
		commit_eeprom_button->enable();
		connect_button->disable();
		days_text->enable();
		device_list->enable();
		disconnect_button->enable();
		disconnect_button->enable();
		flag_text->enable();
		set_eeprom_button->enable();
		get_caps_button->enable();
		get_eeprom_button->enable();
		getraw_eeprom_button->enable();
		get_irdata_button->enable();
		get_key_button->enable();
		get_macro_button->enable();
		get_repeat_button->enable();
		get_wakeup_button->enable();
		hours_text->enable();
		debug_messages_text->enable();
		key_text->enable();
		key_text->setText("KEY_");
		last_key = "";
		last_modifier = "";
		line_text->enable();
		eeprom_map_text->enable();
		minutes_text->enable();
		select_macro_nr->enable();
		modifier_text->enable();
		modifier_text->setText("ff");
		select_macro_slot->enable();
		open_button->enable();
		send_data_button->enable();
		send_data_text->enable();
		set_irdata_button->enable();
		set_key_button->enable();
		set_macro_button->enable();
		set_repeat_button->enable();
		setbyremote_irdata_button->enable();
		setbyremote_kbd_irdata_text_2->enable();
		setbyremote_kbd_irdata_text->enable();
		setbyremote_macro_button->enable();
		protocol_text->enable();
		setbyremote_wakeup_button->enable();
		set_wakeup_button->enable();
		reset_alarm_button->enable();
		receive_ir_button->enable();
		reboot_button->enable();
		repeat_text->enable();
		rescan_button->enable();
		reset_eeprom_button->enable();
		reset_irdata_button->enable();
		reset_key_button->enable();
		reset_macro_button->enable();
		reset_repeat_button->enable();
		select_repeat->enable();
		reset_wakeup_button->enable();
		save_button->enable();
		seconds_text->enable();
		send_ir_button->enable();
		upgrade_button->enable();
		select_wakeup->enable();

		debug_messages_text->appendText("stopped keyboard + irdata mode\n");
		debug_messages_text->setBottomLine(INT_MAX);
	}

	return 1;
}

long
MainWindow::onKbdTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	if(got_modifier){
		got_modifier = 0;

		setbyremote_kbd_irdata_text->setText("press key on keyboard");
		setbyremote_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();
	}

	if(got_key){
		got_key = 0;

		/* if same modifier and key continue
		** (one button press on the remote may send several IR codes, which triggers the corresponding key
		** and under Windows our config app receives keys send by our device, so ignore those)
		*/
		if((modifier_text->getText() == last_modifier) && (key_text->getText() == last_key)){
			debug_messages_text->appendText("same key, ignored\n");
			debug_messages_text->setBottomLine(INT_MAX);
			getApp()->repaint();
			if(setbyremote_kbd_irdata_active)
				getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
			return 1;
		} else {
			if(modifier_text->getText() != "error" && key_text->getText() != "error"){
				onSetKey(NULL, 0, NULL);
				last_modifier = modifier_text->getText();
				last_key = key_text->getText();
			} else {
				debug_messages_text->appendText("invalid modifier/key, try again\n");
				debug_messages_text->setBottomLine(INT_MAX);
				getApp()->repaint();
				if(setbyremote_kbd_irdata_active)
					getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);
				return 1;
			}
		}

		setbyremote_kbd_irdata_text->setText("press button on remote");
		setbyremote_kbd_irdata_text_2->setText("or wait until timeout");
		getApp()->repaint();

		/* exit, if timeout or multiple entries */
		if(onSetByRemoteIrdata(NULL, 0, NULL) == -1) {
			onSetByRemoteKeyboardIrdata(NULL, 0, NULL);
			return 1;
		}

		modifier_text->setText("ff");
		key_text->setText("KEY_EDIT");

		setbyremote_kbd_irdata_text->setText("press modifier or key on keyboard");
		setbyremote_kbd_irdata_text_2->setText("or stop");
		getApp()->repaint();
	}

	if(setbyremote_kbd_irdata_active)
		getApp()->addTimeout(this, ID_KBD_TIMER, 100 * timeout_scalar /* 100 ms*/);

	return 1;
}

long
MainWindow::onSetByRemoteIrdataTimeout(FXObject *sender, FXSelector sel, void *ptr)
{
	onSetByRemoteIrdata(NULL, 0, NULL);

	return 1;
}

int main(int argc, char **argv)
{
	FXApp app("IRMP Pico Configuration", "");
	app.init(argc, argv);
	g_main_window = new MainWindow(&app);
	app.create();
	app.run();
	return 0;
}
