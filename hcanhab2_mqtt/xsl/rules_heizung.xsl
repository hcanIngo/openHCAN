<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// Ende der Definition der globalen Variablen

	<xsl:apply-templates select="bereich/raum/board/heizung"/>
	<xsl:apply-templates select="bereich/board/heizung"/>
</xsl:template>

<xsl:template match="heizung">


//rule "Heizung Soll-Temperatur Vorgabe vom CAN-Bus erhalten"
//when
//	Item SOLLTEMP_<xsl:value-of select="@name" /> received update
//then 
//	sollTemp<xsl:value-of select="@name" /> = SOLLTEMP_<xsl:value-of select="@name" />.state as DecimalType
//	SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(sollTemp<xsl:value-of select="@name" />) // sitemap-Aktu.
//	SOLLTEMP_<xsl:value-of select="@name" />.sendCommand(sollTemp<xsl:value-of select="@name" />) // mqtt-cmd
//end


//rule "Heizung Soll-Temperatur in OpenHab veraendert"
//when
//	Item SOLLTEMP_<xsl:value-of select="@name" /> received command
//	// Item muss autoupdate="false" haben, sodass wenn der Button in der Sitemap gedrückt wird, das item nicht den Button-State annimmt!
//then 
//	var int tempSetpoint = (SOLLTEMP_<xsl:value-of select="@name" />.state as DecimalType).intValue
//	logInfo("IL-+", "tempSetpoint: " + tempSetpoint.toString)
//	logInfo("IL-+", "sollTemp<xsl:value-of select="@name" />: " + sollTemp<xsl:value-of select="@name" />.toString)
//	
//	if (tempSetpoint &gt; sollTemp<xsl:value-of select="@name" />) // wurde erhoeht (++)
//	{
//		if (tempSetpoint &lt; 17)
//		{
//			tempSetpoint = 20 // Sprung auf 20°C
//		}
//	}
//	else // veringert (--): 
//	{
//		
//	}
//	
//	sollTemp<xsl:value-of select="@name" /> = tempSetpoint
//	HEIZMODE_<xsl:value-of select="@name" />.sendCommand("3") // Therm
//	
//	if (sollTemp<xsl:value-of select="@name" />&lt;0)  sollTemp<xsl:value-of select="@name" /> = 0
//	if (sollTemp<xsl:value-of select="@name" />&gt;30) sollTemp<xsl:value-of select="@name" /> = 30
//	
//	// Den Item-State sofort anpassen (nicht das Pollen abwarten). Das Item muss auf autoupdate="false" stehen. 
//	// Sonst wuerde die Button-'+/-'-Reaktion zu langsam erfolgen. Funzt so, da es nicht zum rule-Aufruf SOLLTEMP_... kommt:
//	SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(tempSetpoint) // sitemap-Aktu.
//	//SOLLTEMP_<xsl:value-of select="@name" />.sendCommand(tempSetpoint) // mqtt-cmd  ??
//end

rule "Heizung OpenHab-Selektion veraendert"
when
	Item HEIZMODE_<xsl:value-of select="@name" /> received command
then
 	//logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: " + receivedCommand.toString)

 	if(receivedCommand == "1") { // Aus
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Aus")
		// HEIZMODE_<xsl:value-of select="@name" />.sendCommand("1")
		sollTemp<xsl:value-of select="@name" /> = 0
		SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(sollTemp<xsl:value-of select="@name" />) // sitemap-Aktu.
		return
	}
	else if(receivedCommand == "2") { // Auto
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Auto")
		// HEIZMODE_<xsl:value-of select="@name" />.sendCommand("2")
		return
	}
	else if(receivedCommand == "3") { // Therm
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Therm")
		sollTemp<xsl:value-of select="@name" /> = 20.5	
	}
	else if(receivedCommand == "4") { // ✳ (3 Std.)
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost kurz")
		// TODO Schnittstelle zu mqttPahoClient erweitern um Dauer
		sollTemp<xsl:value-of select="@name" /> = 10	
	}
	else if(receivedCommand == "5") { // ✳ lange
		logInfo("IL-HM", "HEIZMODE_<xsl:value-of select="@name" />: Frost lang")
		// TODO Schnittstelle zu mqttPahoClient erweitern um Dauer
		sollTemp<xsl:value-of select="@name" /> = 10	
	}	
	
	logInfo("IL@HM Solltemp angepasst ", "sollTemp<xsl:value-of select="@name" />: " + sollTemp<xsl:value-of select="@name" />.toString)
	
	//SOLLTEMP_<xsl:value-of select="@name" />.postUpdate(sollTemp<xsl:value-of select="@name" />) // sitemap-Aktu.
	SOLLTEMP_<xsl:value-of select="@name" />.sendCommand(sollTemp<xsl:value-of select="@name" />) // mqtt-cmd
end

</xsl:template>

</xsl:stylesheet>
