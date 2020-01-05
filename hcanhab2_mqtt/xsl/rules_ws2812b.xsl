<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration aus der openHCAN installation.xml.  -->
	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
		<xsl:apply-templates select="bereich/raum/board/ws2812b"/>
		<xsl:apply-templates select="bereich/board/ws2812b"/>
</xsl:template>

<xsl:template match="ws2812b">

rule "RGB-Band <xsl:value-of select="@stt" />: Gruppe=<xsl:value-of select="@gruppe" />"
  when
    Item RgbLicht_<xsl:value-of select="@gruppe" />_hsb received command
  then
  	if (receivedCommand instanceof HSBType)
  	{
    	val float red   = (receivedCommand as HSBType).red   * 2.55
    	val float green = (receivedCommand as HSBType).green * 2.55
    	val float blue  = (receivedCommand as HSBType).blue  * 2.55
	  	val String strRgb = String::format("%s/%s/%.0f,%.0f,%.0f", RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_andereLEDs.state, RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_welcheLEDs.state, red, green, blue)
	  	RgbLicht_<xsl:value-of select="@gruppe" />_rgb.sendCommand(strRgb)
	  	//RgbLicht_<xsl:value-of select="@gruppe" />_hsb.sendCommand(receivedCommand) // color-picker Informationen speichern -> ! Endlosschleife !
  	}
  	else if (receivedCommand == ON)
  	{ 
    	val float red   = 0
    	val float green = 255
    	val float blue  = 0
	  	val String strRgb = String::format("%s/%s/%.0f,%.0f,%.0f", RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_andereLEDs.state, RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_welcheLEDs.state, red, green, blue)
	  	RgbLicht_<xsl:value-of select="@gruppe" />_rgb.sendCommand(strRgb)
  	}
  	else if (receivedCommand == OFF)
  	{
		RgbLicht_<xsl:value-of select="@gruppe" />_rgb.sendCommand("1/1/0,0,0")
  	}
end
</xsl:template>

</xsl:stylesheet>

