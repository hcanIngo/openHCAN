<?xml version="1.0" encoding="UTF-8" ?>
<!-- Skritp zur Generierung einer openHAB-sitemap-Konfiguration (HabHcan.sitemap) aus der openHCAN installation.xml. -->
<xsl:stylesheet version="1.0"
	xmlns:xsl = "http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" />  <!-- ggf. iso-8859-1 -->
 	<xsl:strip-space elements="*"/>

<xsl:template match="/haus">
// ------- GENERATED FROM installation.xml (c) Ingo Lages -----------------
sitemap hcan label="" {
Frame {
	<xsl:for-each select="bereich">
		Text label="<xsl:value-of select="@name" />" icon="<xsl:value-of select="@icon" />" {
		<xsl:apply-templates select="raum"/>
		<xsl:apply-templates select="board"/> <!-- Z.B. Musikanlage im EG -->
		}
	</xsl:for-each>
}
<!-- Auswertung des Attribut: "sammel": -->
Frame {
 	<xsl:for-each select="bereich">
		<xsl:if test="count(board/rolladen | raum/board/rolladen) > 0">
		Text label="<xsl:value-of select="@name" /> Rolladen" icon="<xsl:value-of select="@icon" />" {
			<xsl:for-each select="board/rolladen | raum/board/rolladen">
				<xsl:if test="(@sammel != '-')">
				Frame label="<xsl:value-of select="substring-after(@name,'__')" />: " {  
					// Switch item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter" mappings=["STOP"="S","DOWN"="▼", "UP"="▲"/*, 60="☀", 83="Ξ"*/]
					Switch item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter"
					// Slider item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter"
				}
				</xsl:if>
			</xsl:for-each>
		}
		</xsl:if>
 	
 		<xsl:if test="count(board/heizung | raum/board/heizung) > 0">
		Text label="<xsl:value-of select="@name" /> Heizungen" icon="<xsl:value-of select="@icon" />" {
	 		<xsl:for-each select="board/heizung | raum/board/heizung">
				<xsl:if test="(@sammel != '-')">
				Frame label="<xsl:value-of select="substring-after(@name,'__')" />: " {
					Selection item=HEIZUNG_<xsl:value-of select="@name" /> label="Heiz-Modus" icon="heating" mappings=["0"="aus", "1"="therm (3 Std.)", "2"="auto", "3"="✳ (3 Std.)", "4"="✳ (11 Std.)"]
					Switch item=SOLLTEMP_<xsl:value-of select="@name" /> label="[soll: %d °C]" icon="temperature" mappings=["1"="−", "2"="+"]
				}
				</xsl:if>
			</xsl:for-each>
		}
		</xsl:if>
	</xsl:for-each>
}

<!-- }  Endeklammer der Gesamtsitemap (dyn.+ statische Anteile) erfolgt im Makefile -->
</xsl:template>

<xsl:template match="raum">
Frame {
	Text label="<xsl:value-of select="@name" />" icon="<xsl:value-of select="@icon" />" {		
<!-- Frame nur setzen, wenn eines der Devices gefunden wurde -> kein leerer "Frame {}" -->
	<xsl:if test="count(powerport | board/powerport | reedkontakt | board/reedkontakt | tempsensor | board/tempsensor) > 0">
		Frame {
		<xsl:apply-templates select="powerport"/>
		<xsl:apply-templates select="reedkontakt"/>
		<xsl:apply-templates select="tempsensor"/>	
		
		<xsl:apply-templates select="board/powerport"/>
		<xsl:apply-templates select="board/reedkontakt"/>
		<xsl:apply-templates select="board/tempsensor"/>	
		}
    </xsl:if>
		
		<xsl:apply-templates select="heizung"/>
		<xsl:apply-templates select="rolladen"/>
		
		<xsl:apply-templates select="board/heizung"/>
		<xsl:apply-templates select="board/rolladen"/>
	}
}
</xsl:template>

<xsl:template match="board">
	<xsl:for-each select="heizung">
		<xsl:call-template name="heizungHatTempsensor" />
	</xsl:for-each>
	
	<xsl:if test="count(powerport | reedkontakt | tempsensor) > 0">
		Frame {
		<xsl:apply-templates select="powerport"/>
		<xsl:if test="count(heizung) = 0">
			<xsl:apply-templates select="tempsensor"/>
		</xsl:if>
		<xsl:apply-templates select="reedkontakt"/>
		}
	</xsl:if>
	
	<xsl:apply-templates select="rolladen"/>
</xsl:template>

<xsl:template name="heizungHatTempsensor">
		<xsl:apply-templates select="heizung"/>
		<xsl:apply-templates select="tempsensor"/>
</xsl:template>


<xsl:template match="powerport">
		<xsl:choose>
		<xsl:when test="(@typ = 'lampe')">
			Switch item=LAMPE_<xsl:value-of select="@name" /> label="<xsl:value-of select="substring-after(@name,'__')" /> [MAP(de.map):%s]"
		</xsl:when>
		<xsl:otherwise>
			Switch item=SONSTIGE_<xsl:value-of select="@name" /> label="<xsl:value-of select="substring-after(@name,'__')" /> [MAP(de.map):%s]" icon="socket"
		</xsl:otherwise>
		</xsl:choose>
</xsl:template>
  
<xsl:template match="reedkontakt">
	Text item=REEDKONTAKT_<xsl:value-of select="@name" /> label="<xsl:value-of select="substring-after(@name,'__')" /> [MAP(de.map):%s]"
</xsl:template>

<xsl:template match="tempsensor">
	Text item=TEMPERATUR_<xsl:value-of select="@name" /> label="Temperatur [%d °C]" icon="temperature"
</xsl:template>


<xsl:template match="rolladen">
	Frame label="<xsl:value-of select="substring-after(@name,'__')" />: " {                <!-- Patch fuer Habdroid: U+2008 (punktbreites Leerzeichen vor [%d %%]) --> 
		// Switch item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter" mappings=["STOP"="S","DOWN"="▼", "UP"="▲"/*, 60="☀", 83="Ξ"*/]
		// Switch item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter"
		Slider item=ROLLADEN_<xsl:value-of select="@name" /> label=" [%d %%]" icon="rollershutter"
	} 
</xsl:template>

<xsl:template match="heizung">
	Frame label="<xsl:value-of select="substring-after(@name,'__')" />: " {
		Selection item=HEIZUNG_<xsl:value-of select="@name" /> label="Heiz-Modus<!-- [%d]-->" icon="heating" mappings=["0"="aus", "1"="therm (3 Std.)", "2"="auto", "3"="✳ (3 Std.)", "4"="✳ (11 Std.)"]
		<!-- Switch visibility=[HEIZUNG_<xsl:value-of select="@name" />!=0] item=SOLLTEMP_<xsl:value-of select="@name" /> label="[%d °C, soll: %d °C]" icon="temperature"  mappings=["1"="−", "2"="+"]-->
		<!-- Ggf. Setpoint  statt switch mappings=["1"="−", "2"="+"] -->
		<!-- Setpoint item=SOLLTEMP_<xsl:value-of select="@name" /> label="[soll: %d °C]" icon="temperature"  minValue=10 maxValue=28 step=0.5  -->
		Switch item=SOLLTEMP_<xsl:value-of select="@name" /> label="[soll: %d °C]" icon="temperature" mappings=["1"="−", "2"="+"]
	}
</xsl:template>

</xsl:stylesheet>
