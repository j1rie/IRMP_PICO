# Testen
test_snd_rcv_all_loop.sh sendet 200 Mal eine Reihe von Infrarot-Codes, und irmpconfig
empfängt diese im Testmodus und schreibt sie in Dateien.
compare_testfiles.sh vergleicht diese Dateien mit einer Referenzdatei und zeigt die Unterschiede an.

Fehlerfreie Testergebnisse erhält man nur unter guten Bedingungen.
Tageslicht ist am besten, eine schwache Standardglühbirne ist bereits schlechter und eine starke LED-Lampe ist noch schlechter.
Auch der Abstand und der Winkel zwischen der sendenden LED und dem TSOP beeinflussen das Ergebnis. Zu nah an einem starken Sender zu sein, ist ungünstig.
Bei identischen TSOPs unter schwierigen Bedingungen gingen bei einem mehr und andere Protokolle verloren als beim anderen.

Um alle Protokolle empfangen zu können, muss der IR-Empfänger in der Lage sein, kurze Bursts/Pausen zu dekodieren. Ich habe einen TSOP 34338 verwendet.

Zu Testzwecken kann anstelle eines TSOPs ein Draht verwendet werden.
Dadurch werden Fehler durch den TSOP ausgeschlossen und andere Fehler aufgedeckt.

# Erkennungsqualität
Je kürzer die Timings des Protokolls sind, desto schlechter ist die Erkennung aufgrund von durch den TSOP verursachten Artefakten.  
A1TVBox ist am am schlechtesten.  
Lego ist am zweitschlechtestem.  
Recs80(Ext) hat manchmal Aussetzer.  
Der Rest funktioniert gut, wenn ein guter TSOP verwendet wird.
