<?xml version="1.0" encoding="UTF-8" ?>
                             
<!-- Skritp zur Generierung einer openHAB-items-Konfiguration (HabHcan.items) aus der openHCAN installation.xml.  -->
<!-- Hinweis: Ein Item speichert je einen Zustand im angegebenen Datentyp. -->

	<xsl:stylesheet version="1.0"  
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />
	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
// Enthaltene Items: 
// o LAMPE:    Powerport zur Ansteuerung einer Lampe. 
// o SONSTIGE: Powerport zur Ansteuerung eines sonstigen Elements. Z. B. einer Steckdose. 
// o TASTER:   Tasterbetaetigung, eines an einen Powerport konfigurierten Tasters. 
// 
//                                  openHAB   openHCAN
// o Input  von LAMPE und SONSTIGE: ON/OFF &lt;- POWER_GROUP_STATE_REPLAY (13), POWER_GROUP_STATE_INFO (17)
// o Output von LAMPE und SONSTIGE: ON/OFF -&gt; POWER_GROUP_ON (10), POWER_GROUP_OFF (11)
// o Output von LAMPE und SONSTIGE: REQ    -&gt; POWER_GROUP_STATE_QUERY (12) 
// o Input  von TASTER:             TOGGLE &lt;- TASTER_DOWN (1)
 
		<xsl:apply-templates select="bereich/raum/board/powerport"/>
		<xsl:apply-templates select="bereich/board/powerport"/> <!-- Z.B. Musikanlage im EG -->
</xsl:template>

<xsl:template match="powerport">
		<xsl:choose>
		<xsl:when test="(@typ = 'lampe')">
Switch LAMPE_<xsl:value-of select="@name" /> "<xsl:value-of select="@stt" />licht" &lt;light&gt; (<xsl:value-of select="substring-before(@name,'__')" />, Lampen) ["Lighting"] {mqtt="&gt;[bpiBroker:cb&lt;:command:ON:ON/<xsl:value-of select="@gruppe" />], &gt;[bpiBroker:cb&lt;:command:OFF:OFF/<xsl:value-of select="@gruppe" />], &lt;[bpiBroker:cb&gt;:state:ON:<xsl:value-of select="@gruppe" />/ON], &lt;[bpiBroker:cb&gt;:state:OFF:<xsl:value-of select="@gruppe" />/OFF]", autoupdate="true"}
		</xsl:when>
		<xsl:otherwise>	

//Switch SONSTIGE_<xsl:value-of select="@name" /> (<xsl:value-of select="substring-before(@name,'__')" />, Sonstige) {mqtt="&gt;[bpiBroker:cb&lt;:command:ON:ON/<xsl:value-of select="@gruppe" />], &gt;[bpiBroker:cb&lt;:command:OFF:OFF/<xsl:value-of select="@gruppe" />], &lt;[bpiBroker:cb&gt;:state:ON:<xsl:value-of select="@gruppe" />/ON], &lt;[bpiBroker:cb&gt;:state:OFF:<xsl:value-of select="@gruppe" />/OFF]", autoupdate="true"}
Switch SONSTIGE_<xsl:value-of select="@name" /> "<xsl:value-of select="@stt" />" &lt;switch&gt; (<xsl:value-of select="substring-before(@name,'__')" />, Sonstige) ["Switchable"] {mqtt="&gt;[bpiBroker:cb&lt;:command:ON:ON/<xsl:value-of select="@gruppe" />], &gt;[bpiBroker:cb&lt;:command:OFF:OFF/<xsl:value-of select="@gruppe" />], &lt;[bpiBroker:cb&gt;:state:ON:<xsl:value-of select="@gruppe" />/ON], &lt;[bpiBroker:cb&gt;:state:OFF:<xsl:value-of select="@gruppe" />/OFF]", autoupdate="true"}
		</xsl:otherwise>
		</xsl:choose>

// String TASTER_<xsl:value-of select="@name" />  {mqtt="&lt;[bpiBroker:cb&gt;:state:TOGGLE:HES/1/<xsl:value-of select="@gruppe" />.*/.*]", autoupdate="false"}
</xsl:template>


</xsl:stylesheet>

