/*
 * Quelle: http://www.rn-wissen.de/index.php/Gleitender_Mittelwert_in_C
 */
#include "floatingAverage.h"

void initFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		  tFloatAvgType i_DefaultValue)
{
	uint8_t i;
	// Den Buffer mit dem Initialisierungswert fuellen:
	for (i = 0; i < SIZE_OF_AVG; ++i)
	{
		io_pFloatAvgFilter->aData[i] = i_DefaultValue;
	}
	// Der naechste Wert soll an den Anfang des Buffers geschrieben werden:
	io_pFloatAvgFilter->indexNextValue = 0;
}


void addToFloatAvg(tFloatAvgFilter * io_pFloatAvgFilter,
		   tFloatAvgType i_NewValue)
{
	// Neuen Wert an die dafuer vorgesehene Position im Buffer schreiben.
	io_pFloatAvgFilter->aData[io_pFloatAvgFilter->indexNextValue] =
		i_NewValue;
	// Der naechste Wert wird dann an die Position dahinter geschrieben.
	io_pFloatAvgFilter->indexNextValue++;
	// Wenn man hinten angekommen ist, vorne wieder anfangen (Ringspeicher).
	io_pFloatAvgFilter->indexNextValue %= SIZE_OF_AVG;
}


tFloatAvgType getOutputValue(tFloatAvgFilter * io_pFloatAvgFilter)
{
	tTempSumType tempSum = 0;

	// Durchschnitt berechnen:
	uint8_t i;
	for (i = 0; i < SIZE_OF_AVG; ++i)
	{
		tempSum += io_pFloatAvgFilter->aData[i];
	}
	// Der cast is OK, wenn tFloatAvgType und tTempSumType korrekt gewaehlt wurden.
	tFloatAvgType o_Result = (tFloatAvgType) (tempSum / SIZE_OF_AVG);
	return o_Result;
}
