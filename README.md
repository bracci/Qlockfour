# QLOCKFOUR NodeMCU
### Eine Firmware der Selbstbau-QLOCKTWO.

QLOCKFOUR NodeMCU ist eine Firmware fuer die Selbstbau-QLOCKTWO.

Sie gleicht die Zeit einmal am Tag per NTP mit einem Zeitserver im Internet ab. Auf der Web-Seite kann man die Uhr steuern, konfigurieren und Infos abrufen. Updates sind OTA moeglich. Dazu im Arduino IDE den ESP als Port auswaehlen.

Einher geht die Firmware mit dem BOARD_NODEMCU bestehend aus NodeMCU/ESP8266 und einem DS3231-RTC-Modul. Informationen zum schnellen Aufbau liegen im Verzeichnis. Siehe _BOARD_NodeMCU.

Ein Video gibt es hier: https://www.youtube.com/watch?v=X_I5B-dErzE&feature=youtu.be
Die Firmware gibt es hier: https://github.com/ch570512/Qlockfour

Je nach Schalter (USE_...) und Parameter (ABC_...) in der Konfiguration stehen verschiedene Funktionen der Uhr
zur Verfuegung.
  
Generell blinkt das Display bei der Moeglichkeit eine Zeit einzustellen. Ausserdem wird der Vormittag durch AM, der
Nachmittag durch PM in der originalen Front angezeigt. Bei Sprachen die AM und PM nicht enthalten, leuchtet der erste
Buchstabe fuer AM, der zweite Buchstabe fuer PM.

### Standard Modi
```
Zeitanzeige: Der Standardmodus der Uhr. Er zeigt die Zeit an. :)
Anzeige AM/PM: Zeigt an, ob es vormittags (AM) oder nachmittags (PM) ist. (USE_STD_MODE_AMPM)
Sekunden: Anzeige der Sekunden. (USE_STD_MODE_SECONDS)
Datum: Anzeige des aktuellen Tages und Monats. (USE_STD_MODE_DATE)
Temperatur: Anzeige der gemessenen Temperatur. (USE_STD_MODE_TEMP)
Alarm: H+ und M+ druecken um den Alarm (in 5 Minuten-Schritten) fuer die naechsten 24 Stunden zu stellen. Der Alarm
       schaltet sich nach Ablauf der Fallback-Time automatisch ein. Er wird durch Druecken der Mode-Taste deaktiviert.
       (USE_STD_MODE_ALARM)
Helligkeit: H+ und M+ druecken um die LEDs im manuellen Modus heller oder dunkler zu stellen.
            Wird nicht angezeigt, wenn die automatische Helligkeitsregelung eingeschaltet ist.
```
### Erweiterte Modi
```
Titel MAIN: H+ und M+ druecken um direkt in die naechste oder vorhergehende Kategorie zu wechseln. (USE_EXT_MODE_TITLES)
Automatische Helligkeitsregelung ein/aus (A/M)
Effekt beim Zeitwechsel (TR NO/FD/SD/MX): kein Effekt, Fading, Sliding, Matrix (nur mit Farb-LEDs)
Farbe (C 0..18) oder Farbwechsel (CC 01/02): CC 01 siehe unten, CC 02 wechselt die Farbe im 5 Minutentakt.
Geschwindigkeit fuer fortdauernden Farbwechselmodus CC 01 (CR 00...10): 00 -> langsam, 10 -> schnell.
Sprache (DE/CH/EN/...): Die passende Sprache zur benutzten Front waehlen.
Ruecksprungverzoegerung (FB nn): Wie lange soll es dauern, bis z.B. aus der Sekundenanzeige wieder zurueck in die
                                 Zeitanzeige gewechselt wird. (0 = deaktiviert.)

Titel TIME: H+ und M+ druecken um direkt in die naechste oder vorhergehende Kategorie zu wechseln. (USE_EXT_MODE_TITLES)
"Es ist" anzeigen oder nicht (IT EN/DA) (USE_EXT_MODE_IT_IS)
Zeit einstellen: H+ und M+ druecken um die Zeit zu stellen. Die Sekunden springen mit jedem Druck auf Null.
Tag einstellen   (DD nn): H+ und M+ druecken um den aktuellen Tag einzustellen. (USE_EXT_MODE_DATE_MANUALLY)
Monat einstellen (MM nn): H+ und M+ druecken um den aktuellen Monat einzustellen.
Jahr einstellen  (YY nn): H+ und M+ druecken um das aktuelle Jahr einzustellen.
Nachtauschaltung        (N OF): H+ und M+ druecken um die Ausschaltzeit des Displays einzustellen. (USE_EXT_MODE_NIGHT_OFF)
Nachtwiedereinschaltung (N ON): H+ und M+ druecken um die Einschaltzeit des Displays einzustellen. Analog (N OFF).

Titel TEST: H+ und M+ druecken um direkt in die naechste oder vorhergehende Kategorie zu wechseln. (USE_EXT_MODE_TITLES)
LED-Test: Laesst einen senkrechten Streifen ueber das Display wandern. (USE_EXT_MODE_TEST)
```
### sonstige Schalter und Parameter
```
WLAN_SSID:
WLAN_PASS:
HOSTNAME:
OTA_PASS:
NTP_SERVER:
UTC_OFFSET:
LED_TEST_INTRO:      Laesst alle LEDs nach dem Start der Uhr fuer 3 Sekunden leuchten.
NONE_TECHNICAL_ZERO: Zeigt die Null ohne den diagonalen Strich.
TEMP_OFFSET:         Gibt an, um wieviel Grad die gemessene Temperatur (+ oder -) korrigiert werden soll.
MAX_BUZZ_TIME:       Nach wie vielen Minuten soll sich der Alarm automatisch abstellen?

BOARD_NODEMCU:       Bitte eine externe 5V Stromquelle verwenden da sonst evtl. der NodeMCU und/oder der USB-Port des
                     Computers wegen des hohen Stroms der LEDs durchbrennt. Dateien mit Informationen liegen im
                     Verzeichnis. Der Aufbau auf einer kleinen Lochrasterplatine ist relativ einfach moeglich.
ENABLE_SQW_LED:      Zeigt mit Hilfe der LED auf dem Board die Funktion der RTC an. Sie blinkt einmal pro Sekunde.

LED_DRIVER_NEOPIXEL: WS2812B-RGB-LED-Streifen.
LED_DRIVER_LPD8806:  LPD8806-RGB-LED-Streifen.
LED_DRIVER_LPD8806RGBW: LPD8806-RGBW-LED-Streifen.

LED_LAYOUT_MOODLIGHT: LED-Layout nach dem Moodlight von Christian. Waagerecht und Eck-LEDs am Ende des Stripes.
LED_LAYOUT_CLT2:      LED-Layout wie in der CLT2. Senkrecht und Eck-LEDs innerhalb des Stripes.

REMOTE_SPARKFUN:     Fernbedienung von Sparkfun.
REMOTE_MOONCANDLES:  Fernbedienung von Mooncandles.
REMOTE_LUNARTEC:     Fernbedienung von Lunartec.
REMOTE_CLT:          Fernbedienung der CLT2.
REMOTE_APPLE:        Alte kleine weisse Fernbedienung fuer den iPod von Apple:
                     Play: Standard Modi
                     Menu: Erweiterte Modi
                     Back: Ruecksprung zur Zeitanzeige
                     FF:   Display ein/aus
                     +:    Stunde+
                     -:    Minute+
REMOTE_PHILIPS:      Universal Fernbedienung Philips SRP1 101/10 mit dem Geraetecode 0815:
                     PROG+: Standard Modi
                     PROG-: Erweiterte Modi
                     POWER: Display ein/aus
                     MUTE:  Ruecksprung zur Zeitanzeige
                     VOL+:  Stunde+
                     VOL-:  Minute+
                     1:     Uebergang Fade
                     2:     Uebergang Matrix
                     3:     Uebergang Slide
                     4:     Farbe weiss
                     5:     Farbe rot
                     6:     Farbe gruen
                     7:     Farbe dunkelblau
                     8:     Farbe gelb
                     9:     automatischer Farbwechsel
                     A/V:   Helligkeit-
                     -/--:  Helligkeit+
                     0:     LDR ein/aus
REMOTE_HX1838:       Fernbedienung HX1838.

IR_LETTER_OFF_:      Schaltet die LED hinter dem IR-Sensor dauerhaft ab. Das verbessert den IR-Empfang.
                     Hier das K vor Uhr: Zehnte Zeile, achter Buchstabe. Wir fangen mit 1 an zu zaehlen.

LDR_MIN_PERCENT:     Minimale Helligkeit der LEDs in Prozent.
LDR_MAX_PERCENT:     Maximale Helligkeit der LEDs in Prozent.
LDR_HYSTERESE:       Verzoegerung mit der die Helligkeit angepasst wird.
LDR_CHECK_RATE:      Geschwindigkeit mit der die Helligkeit angepasst wird.

SERIAL_SPEED:        Geschwindigkeit der seriellen Schnittstelle fuer die serielle Konsole.
DEBUG:               Gibt technische Informationen in der seriellen Konsole aus.
DEBUG_TIME:          Gibt die aktuelle Zeit aus.
DEBUG_MATRIX:        Rendert die Ausgabe der Matrix fuer die deutsche Front in der seriellen Konsole.
DEBUG_SET_DEFAULTS:  Schreibt die Default-Werte bei jedem Start in den EEPROM.
```
### Important Info
```
In order to get the firmware to compile, you have to install the following libraries:
https://github.com/adafruit/Adafruit_NeoPixel
https://github.com/ch570512/LPD8806
https://github.com/ch570512/LPD8806RGBW
https://github.com/markszabo/IRremoteESP8266
```
### Vielen Dank an alle, insbesondere an Christian, Manuel und Andreas, auf deren Arbeit, Zeit und Einfallsreichtum diese Version der Firmware aufbaut.
