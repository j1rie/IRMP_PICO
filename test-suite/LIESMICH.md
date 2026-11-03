# Testen
test_snd_rcv_all_loop.sh sendet 200 Mal eine Reihe von Infrarot-Codes, und irmpconfig empfängt diese im test Modus und schreibt sie in Dateien.
compare_testfiles.sh vergleicht diese Dateien mit einer Referenzdatei und zeigt die Unterschiede an.

test_snd_rcv_all_loop2_shortTimings.sh sendet 256 Mal die kurzen Timings und test_snd_rcv_all_loop2_longTimings.sh sendet 256 Mal die langen Timings, und irmpconfig empfängt diese im test2 Modus und schreibt sie in eine Datei.

test_snd_rcv_all_loop3_shortTimings.sh sendet 20.000 Mal die kurzen Timings und test_snd_rcv_all_loop3_longTimings.sh sendet 20.000 Mal die langen Timings, und irmpconfig empfängt diese im test3 Modus und schreibt sie in eine Datei.

Fehlerfreie Testergebnisse erhält man nur unter guten Bedingungen.
Z.B. wird im Moment des Einschalten eines Monitors mit CCFL Backlight der Test gestört.
Eine Energiesparlampe auf Leuchtröhrenbasis ist noch störender.
Auch der Abstand und der Winkel zwischen der sendenden LED und dem TSOP beeinflussen das Ergebnis. Zu nah an einem starken Sender zu sein, ist ungünstig.
Bei identischen TSOPs unter schwierigen Bedingungen gingen bei einem mehr und andere Protokolle verloren als beim anderen.

Für die langen Protokolle habe ich einen TSOP4838 verwendet, für die kurzen einen TSOP 34338.

Zu Testzwecken kann anstelle eines TSOPs ein Draht verwendet werden.
Dadurch werden Fehler durch den TSOP ausgeschlossen und andere Fehler aufgedeckt.
