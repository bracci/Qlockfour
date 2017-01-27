/*
   Configuration
   Die Compiler-Einstellungen der Firmware an einer zentralen Stelle.

  *********************************************************************************************************************
    QLOCKFOUR NodeMCU
    Eine Firmware der Selbstbau-QLOCKTWO.
  *********************************************************************************************************************

  QLOCKFOUR NodeMCU ist eine Firmware fuer die Selbstbau-QLOCKTWO.
  Sie gleicht die Zeit einmal am Tag per NTP mit einem Zeitserver im Internet ab. Auf der Web-Seite kann man die Uhr
  steuern, konfigurieren und Infos abrufen.
  Einher geht die Firmware mit dem BOARD_NODEMCU bestehend aus NodeMCU/ESP8266 und einem DS3231-RTC-Modul.
  Informationen zum schnellen Aufbau liegen im Verzeichnis. Siehe _BOARD_NodeMCU.*
  
  Ein Video gibt es hier: https://www.youtube.com/watch?v=X_I5B-dErzE&feature=youtu.be
  Die Firmware gibt es hier: https://github.com/ch570512/Qlockfour

  Je nach Schalter (USE_...) und Parameter (ABC_...) in der Konfiguration stehen verschiedene Funktionen der Uhr
  zur Verfuegung.
  
  Generell blinkt das Display bei der Moeglichkeit eine Zeit einzustellen. Ausserdem wird der Vormittag durch AM, der
  Nachmittag durch PM in der originalen Front angezeigt. Bei Sprachen die AM und PM nicht enthalten, leuchtet der erste
  Buchstabe fuer AM, der zweite Buchstabe fuer PM.

  *** Standard Modi ***

  Zeitanzeige: Der Standardmodus der Uhr. Er zeigt die Zeit an. :)
  Anzeige AM/PM: Zeigt an, ob es vormittags (AM) oder nachmittags (PM) ist. (USE_STD_MODE_AMPM)
  Sekunden: Anzeige der Sekunden. (USE_STD_MODE_SECONDS)
  Datum: Anzeige des aktuellen Tages und Monats. (USE_STD_MODE_DATE)
  Temperatur: Anzeige der gemessenen Temperatur. (USE_STD_MODE_TEMP)
  Helligkeit: H+ und M+ druecken um die LEDs im manuellen Modus heller oder dunkler zu stellen.
              Wird nicht angezeigt, wenn die automatische Helligkeitsregelung eingeschaltet ist.

  *** Erweiterte Modi ***

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

  *** sonstige Schalter und Parameter ***

  WLAN_SSID:
  WLAN_PASS:
  NTP_SERVER:
  UTC_OFFSET:
  LED_TEST_INTRO:      Laesst alle LEDs nach dem Start der Uhr fuer 3 Sekunden leuchten.
  NONE_TECHNICAL_ZERO: Zeigt die Null ohne den diagonalen Strich.
  IR_LETTER_OFF:       Schaltet die LED hinter dem IR-Sensor dauerhaft ab. Das verbessert den IR-Empfang.
                       Hier das K vor Uhr: letzte Zeile (matrix[9]), achter Buchstabe (0b1111111011111111).
  TEMP_OFFSET:         Gibt an, um wieviel Grad die gemessene Temperatur (+ oder -) korrigiert werden soll.
  
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
  REMOTE_HX1838:
  
  LDR_MIN_PERCENT:     Minimale Helligkeit der LEDs in Prozent.
  LDR_MAX_PERCENT:     Maximale Helligkeit der LEDs in Prozent.
  LDR_HYSTERESE:       Verzoegerung mit der die Helligkeit angepasst wird.
  LDR_CHECK_RATE:      Geschwindigkeit mit der die Helligkeit angepasst wird.

  SERIAL_SPEED:        Geschwindigkeit der seriellen Schnittstelle fuer die serielle Konsole.
  DEBUG:               Gibt technische Informationen in der seriellen Konsole aus.
  DEBUG_TIME:          Gibt die aktuelle Zeit aus.
  DEBUG_MATRIX:        Rendert die Ausgabe der Matrix fuer die deutsche Front in der seriellen Konsole.
  DEBUG_SET_DEFAULTS:  Schreibt die Default-Werte bei jedem Start in den EEPROM.

  *** Important Info ***

  In order to get the firmware to compile, you have to install the following libraries:
  https://github.com/adafruit/Adafruit_NeoPixel
  https://github.com/ch570512/LPD8806
  https://github.com/ch570512/LPD8806RGBW
  https://github.com/markszabo/IRremoteESP8266

  *********************************************************************************************************************
  *** Vielen Dank an alle, insbesondere an Christian, Manuel und Andreas, auf deren Arbeit, Zeit und
  *** Einfallsreichtum diese Version der Firmware aufbaut.
  *********************************************************************************************************************
*/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/******************************************************************************
  Standard Menu
******************************************************************************/

// Show AM/PM.
#define USE_STD_MODE_AMPM

// Show seconds.
#define USE_STD_MODE_SECONDS

// Show date.
#define USE_STD_MODE_DATE

// Show temperature.
#define USE_STD_MODE_TEMP

/******************************************************************************
  Extended Menu
******************************************************************************/

// MAIN/TIME/TEST titles.
#define USE_EXT_MODE_TITLES

// Languages.
#define ENABLE_LANGUAGE_DE
#define ENABLE_LANGUAGE_DE_MKF
#define ENABLE_LANGUAGE_D3
#define ENABLE_LANGUAGE_CH
#define ENABLE_LANGUAGE_EN
#define ENABLE_LANGUAGE_FR
#define ENABLE_LANGUAGE_IT
#define ENABLE_LANGUAGE_NL
#define ENABLE_LANGUAGE_ES

// Setup to disable "It is".
#define USE_EXT_MODE_IT_IS

// Setup date.
#define USE_EXT_MODE_DATE_MANUALLY

// Setup night on/off.
#define USE_EXT_MODE_NIGHT_OFF

// LED test.
#define USE_EXT_MODE_TEST

/******************************************************************************
  Settings
******************************************************************************/

// WLAN settings.
#define WLAN_SSID "MeineWLANSSID"
#define WLAN_PASS "HierMeinWLANKennwort"

// NTP-Server.
#define NTP_SERVER "pool.ntp.org"

// Offset from GMT/UTC.
#define UTC_OFFSET +1

// Turn on all LEDs for 3s on power-up.
#define LED_TEST_INTRO

// None technical zero.
//#define NONE_TECHNICAL_ZERO

// Turn off the letter containing the IR-Sensor (here: 10, 8).
//#define IR_LETTER_OFF matrix[9] &= 0b1111111011111111

// Temperature-Sensor.
#define TEMP_OFFSET 5

// Board.
#define BOARD_NODEMCU

// LED on board.
#define ENABLE_SQW_LED

// LED-Driver.
#define LED_DRIVER_NEOPIXEL
//#define LED_DRIVER_LPD8806
//#define LED_DRIVER_LPD8806RGBW

// LED-Layout.
#define LED_LAYOUT_MOODLIGHT
//#define LED_LAYOUT_CLT2

// IR-Remote.
//#define REMOTE_SPARKFUN
//#define REMOTE_MOONCANDLES
//#define REMOTE_LUNARTEC
//#define REMOTE_CLT2
//#define REMOTE_APPLE
#define REMOTE_PHILIPS
//#define REMOTE_HX1838

// LDR.
#define LDR_MIN_PERCENT 5
#define LDR_MAX_PERCENT 100
#define LDR_HYSTERESE 50
#define LDR_CHECK_RATE 75

// misc.
#define FIRMWARE_VERSION "qffw_20170126"

/******************************************************************************
  Debug to serial console.
******************************************************************************/

#define SERIAL_SPEED 57600    // Set speed for debuging console.
//#define DEBUG               // Switch on debug.
//#define DEBUG_TIME          // Shows the time every secound.
#define DEBUG_MATRIX        // Renders the matrix to console - German front - Works best with Putty.
//#define DEBUG_SET_DEFAULTS  // Sets the EEPROM to defauls on every startup.

#endif
