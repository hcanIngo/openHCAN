/*
 * Quelle: http://www.rn-wissen.de/index.php/Gleitender_Mittelwert_in_C
 */

#ifndef FLOATINGAVERAGE_H_
#define FLOATINGAVERAGE_H_

#include <inttypes.h>

// Ueber wieviele Werte soll der gleitende Mittelwert berechnet werden?
// Zulaessige Werte 1..255
#define SIZE_OF_AVG  30

// Datentyp, ueber den der gleitende Mittelwert berechnet werden soll.
typedef uint16_t tFloatAvgType;

// Wird nur intern fuer die Durchschnittsberechnung benutzt.
// Muss Zahlen fassen koennen, die SIZE_OF_AVG mal groesser als tFloatAvgType sind.
typedef uint32_t tTempSumType; //10-Bit-ADC = 1023 * 60 = 61380

// Die Struktur, in der die Daten zwischengespeichert werden
typedef struct
 {
	tFloatAvgType aData[SIZE_OF_AVG];
	uint8_t indexNextValue;
 } tFloatAvgFilter;


// Initialisiert das Filter mit einem Startwert.
void initFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		  tFloatAvgType i_DefaultValue);

// Schreibt einen neuen Wert in das Filter.
void addToFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		   tFloatAvgType i_ui16NewValue);

// Berechnet den Durchschnitt aus den letzten SIZE_OF_AVG eingetragenen Werten.
tFloatAvgType getOutputValue(tFloatAvgFilter * io_pFloatAvgFilter);

#endif /* FLOATINGAVERAGE_H_ */
