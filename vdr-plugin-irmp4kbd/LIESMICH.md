Dies ist ein Plugin fuer den Video Disk Recorder (VDR).

Verfasst von:                      Joerg Riechardt <J.Riechardt@gmx.de>

Projekt-Homepage:                  https://github.com/j1rie/IRMP_PICO/vdr-plugin-irmp

Aktuelle Version verfuegbar unter: https://github.com/j1rie/IRMP_PICO/vdr-plugin-irmp

Beschreibung: Das Plugin irmp4kbd sendet Tastatureingaben von einem IRMP Pico Geraet an VDR.

Die einfachste Methode zum Erstellen von Tastenbelegungen in remote.conf ist die Verwendung des Tasten-Anlernprozesses von VDR.

repeat_delay, repeat_period und repeat_release müssen auf 0 gesetzt werden.

Wenn man das vdr-plugin-softhddevice verwendet, muss die Fernbedienungsfunktion von softhddevice mit -P'softhddevice ... -N' deaktiviert werden.  
Für softhddevice-drm-gles und vaapivideo braucht man vdr mit --no-kbd.  
