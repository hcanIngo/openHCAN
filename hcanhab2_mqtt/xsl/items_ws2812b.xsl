<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp. -->

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

Color  RgbLicht_<xsl:value-of select="@gruppe" />_hsb // HSB-Typ
String RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_welcheLEDs
String RgbLicht_<xsl:value-of select="@gruppe" />_Pattern_andereLEDs
String RgbLicht_<xsl:value-of select="@gruppe" />_rgb "<xsl:value-of select="@stt" />licht" &lt;light&gt; (<xsl:value-of select="substring-before(@name,'__')" />, Lampen) ["Lighting"] {mqtt="&gt;[bpiBroker:cb&lt;:command:*:C/<xsl:value-of select="@gruppe" />/${command}]"}

</xsl:template>


</xsl:stylesheet>

