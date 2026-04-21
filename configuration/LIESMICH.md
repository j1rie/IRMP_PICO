## Die fünf Wege
1. REPORT_ID_IR -> irmplircd → lircd2uinput bzw. lircd-uinput → /dev/input/eventX → eventlircd → Kodi/VDR  
2. REPORT_ID_KBD -> /dev/input/eventX → eventlircd → Kodi/VDR  
3. REPORT_ID_IR -> vdr-plugin-irmp -> VDR  
4. REPORT_ID_KBD -> vdr-plugin-irmp4kbd -> VDR  
5a. REPORT_ID_KBD -> /dev/input/eventX → softhddevcie → VDR (mit X)  
5b. REPORT_ID_KBD -> /dev/input/eventX → VDR (ohne X)  

*Es darf nur einer der fünf Wege aktiviert sein.*  

Für den ersten Weg braucht man eine map für irmplircd.  
Für den zweiten Weg braucht man eine kbd.map und eine .evmap.  
Für den vierten Weg müssen die Tasten im Empfänger angelernt werden.  
Für den dritten Weg und den vierten Weg müssen einmalig die Tasten im VDR angelernt werden.  
Der dritte Weg ist, wenn man nur den VDR bedienen will, der einfachste.

Für Kodi braucht man bei Weg drei und vier eine kbd.map fürs eeprom und eine passende keymap für Kodi.  

Der fünfte Weg wird nicht empfohlen.

## /dev/irmp_pico
Die udev Regel '70-irmp.rules' wird in das udev rules Verzeichnis (z.B. /etc/udev/rules.d/) kopiert.  

## Fehlersuche
Kommen mit irmpconfig_gui im receive mode Tasten an?  
yaVDR: Was zeigt journalctl -u vdr -f?  
VDR*ELEC: VDR stoppen (systemctl stop vdropt) und vdr manuell auf der Konsole starten /usr/local/bin/start_vdr.sh
andere: VDR stoppen (systemctl stop vdr) und vdr manuell auf der Konsole starten vdr -Pirmp ...

## Autorepeat vom Kernel
Wenn die automatische Wiederholung des Kernels stört, kann man diese mit evrepeat, kbdrate oder xset (oder ir-keytable auf älteren Systemen) ändern. Sie sollte größer als das release timeout sein, damit sie nicht stört.  
Wenn das nicht hilft, kann man den Kernel Treiber hid_irmp verwenden. Man aktiviert das Modul unter Device drivers → HID support → Special HID drivers → IRMP USB-HID-keyboard support.  
Es kann sein, dass man "rmmod hid_irmp", "rmmod hid_generic" und "modprobe hid_irmp" zu /etc/init.d/boot.local hinzufügen muss (taucht in dmesg "irmp configured" auf?).

Grund:
Eine Tastatur sendet beim Drücken Key-Press und beim Loslassen Key-Release und die Wiederholung wird nicht in der Tastatur, sondern vom Autorepeat im Kernel erzeugt. Nach Press kommen so lange Autorepeats bis zum Release.  
Eine Fernbedienung sendet periodisch ein Signal, kennt aber keinen Key-Release. Deswegen wird der Release nach einem Timeout vom Empfänger erzeugt.  
Wenn die Firmware den Release gleich nach dem Press generiert, werden lange Tastendrücke nicht als Wiederholung, sondern als neue Tastendrücke erkannt.  
Wenn die Firmware abwartet, ob noch was kommt, kommt der Release möglicherweise zu spät, denn der Autorepeat im Kernel hat eventuell in der Zwischenzeit Wiederholungen erzeugt. Dann gibt es Nachlauf.  

evrepeat -d 250 -p 100 /dev/irmp_pico_event  
kbdrate -r 2  -d 1000  
DISPLAY=:0 xset r rate 1000 200  
DISPLAY=:0 xset q  
TODO: Wenn das funktioniert, führe es in der udev-Regel aus per RUN.  

Für vdr-plugin-irmp und vdr-plugin-irmp4kbd ist das nicht relevant.

## Wurde der Computer vom Empfänger gestartet?
Man kann protokollieren, wann der Empfänger den Computer gestartet hat.  
Bei jedem Start durch den Empfänger sendet er eine konfigurierbare Zeit lang sekündlich KEY_REFRESH. Der erste wird in die Logdatei /var/log/started_by_IRMP_PICO geschrieben. Dazu wird von irexec oder triggerhappy log_KEY_REFRESH.sh aufgerufen.  
Wenn kurz nach den Bootmeldungen (je nach Distribution/var/log/boot.msg o.ä.) ein Eintrag im Logfile (/var/log/started_by_IRMP_PICO) landet, weiß man, dass der Computer vom Empfänger gestartet wurde.  
Wenn der Eintrag im Logfile älter ist als die Bootmeldungen, wurde per Einschalter am Computer oder per Timer gestartet.
Konfiguration über irmpconfig -> s -> x -> 90.  
Zum Testen muss der PC aus sein und vom Empfänger gestartet werden.

Man kann dies im Shutdown-Skript von VDR verwenden, um zu verhindern, dass der Computer beim ersten Drücken der Power-Taste heruntergefahren wird, wenn er bereits läuft. Siehe Beispielskript.
Das ist nützlich, wenn eine Logitech Fernbedienung mit Makros auf einen Tastendruck hin den VDR und alle anderen Geräte einschaltet. Wenn der VDR durch einen Timer gestartet wurde, würde er sonst aus gehen, wenn man alle anderen Geräte einschalten will. Durch eine Abfrage im shutdown-Skript kann man das vermeiden (siehe das Beispielskript vdrshutdown).  
log_KEY_REFRESH.sh wird z.B. von triggerhappy oder irexec aufgerufen.  
Für triggerhappy wird irmp_pico.conf nach /etc/triggerhappy/triggers.d/ kopiert  
vdr-plugin-irmp schreibt direkt in /var/log/started_by_IRMP_PICO.

## Automatisches starten und stoppen von eventlircd beim Booten oder beim Anschliessen/Entfernen des Geräts
Die udev Regel '98-eventlircd.rules' wird in das udev rules Verzeichnis (/etc/udev/rules.d/) kopiert,  
die evmap '03_1209_4446.evmap' wird in das evmap Verzeichnis (/usr/etc/eventlircd.d/) kopiert,  
der systemd service 'eventlircd.service' wird in das services Verzeichnis (/etc/systemd/system/) kopiert,  
der systemd service 'eventlircd.socket' wird in das services Verzeichnis (/etc/systemd/system/) kopiert,  
die systemd-tmpfiles Konfigurationsdatei 'eventlircd.conf' wird in das tmpfiles Verzeichnis (/etc/tmpfiles.d/) kopiert,  
und es wird "systemctl enable eventlircd.service eventlircd.socket" einmal ausgeführt.  
'03_1209_4446.evmap' funktioniert unter yaVDR.

## Verwende nicht softhddevice für die Fernbedienung.
Es wird empfohlen, stattdessen vdr-plugin-irmp oder vdr-plugin-irmp4kbd zu verwenden, da die Fernbedienungsfunktion von softhddevice nicht so präzise ist.  
Die Fernbedienungsfunktion von softhddevice wird mit dem Parameter -N abgeschaltet.  
Für softhddevice-drm-gles und vaapivideo braucht man vdr mit --no-kbd.  

## Mit softhddevice, ohne eventlircd: keysyms finden
Softhddevice gibt X11 Tastendrücke als 'XKeySym' an VDR weiter. Um sie zu finden, startet man xev in einem xterm mit dem Fokus auf dem Testfenster.  
Drückt man dann eine Taste auf der Fernbedienung, werden die entsprechenden keysym's angezeigt.    
Zum Beispiel ist eine Taste als 'KEY_I' konfiguriert, was das keysym 'i' ergibt, das auf 'Info' abgebildet ist. Siehe kbd.map und remote.conf.

## Mit softhddevice, ohne eventlircd: softhddevice fortsetzen
Im Suspend mit SuspendClose=1 gibt softhddevice keine X11 Tastendrücke weiter.  
Um aus dem Suspend weiterzumachen, braucht man z.B. triggerhappy. Siehe irmp_pico.conf und 70-irmp.rules.

## Wiederholrate der Fernbedienung testen
Wenn man irmpconfig -> y startet und lange dieselbe Taste gedrückt hält, bekommt man eine Statistik, wie oft welcher Zeitabstand zwischen den Signalen der Fernbedienung gemessen wurde.  
Normalerweise sollten da nur ein oder zwei Zahlen stehen, und im Fall von Aussetzern auch noch ca. die doppelten Zahlen.  
Falls die Zahlen stärker als 3% streuen, muss man die automatische Releaseerkennung deaktivieren und bei repeat-release statt 0 einen Wert etwas höher als den längsten nicht verdoppelten Wert einsetzen.

## Support Anfragen
Bitte folgende Daten bereitstellen:  
- Sämtliche Parameter von VDR und Plugins  
- Debugausgaben des Plugins (falls irmp oder irmp4kbd verwendet)  
- Ausgabe von irmpconfig → y (1 Minute Tasten gedrückt halten)  
- Ausgabe von irmpconfig → m (verschiedene Tasten drücken, manchmal auch länger)  
