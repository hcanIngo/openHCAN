INDENT_OPTS = -orig -ci4

all: eds.xml hcan-protocol.xml validate hcan-def.h hcan_transport.h hcan_transport.cc eds-structs.h oap-def.h hcan-protocol-referenz.txt

install:
	test -d /usr/share/hcan || sudo "mkdir /usr/share/hcan"
	sudo /usr/bin/install hcan-protocol.xml /usr/share/hcan
	sudo /usr/bin/install eds.xml /usr/share/hcan
	sudo /usr/bin/install oap.xml /usr/share/hcan
	sudo /usr/bin/install installation.dtd /usr/share/hcan

install-wiki-doc: hcan-protocol-referenz.txt
	scp hcan-protocol-referenz.txt extweb:/var/lib/dokuwiki/data/pages/hcan-protocol-referenz.txt

validate: 
	xalan -validate -in hcan-protocol.xml -xsl mk_hcan-def_h.xsl > /dev/null

hcan-def.h: mk_hcan-def_h.xsl hcan-protocol.xml
	xalan -in hcan-protocol.xml -xsl mk_hcan-def_h.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > hcan-def.h
	cp hcan-def.h ../include

oap-def.h: mk_oap_def.xsl oap.xml
	xalan -in oap.xml -xsl mk_oap_def.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > oap-def.h
	cp oap-def.h ../include

hcan_transport.h: mk_transport_h.xsl hcan-protocol.xml
	xalan -in hcan-protocol.xml -xsl mk_transport_h.xsl  | \
	indent $(INDENT_OPTS) | sed 's/,)/)/g' > hcan_transport.h
	cp hcan_transport.h ../libhcan++

hcan_transport.cc: mk_transport_cc.xsl hcan-protocol.xml
	xalan -in hcan-protocol.xml -xsl mk_transport_cc.xsl  | \
	indent $(INDENT_OPTS) | sed 's/,)/)/g' > hcan_transport.cc
	cp hcan_transport.cc ../libhcan++

eds-structs.h: mk_eds_structs.xsl eds.xml
	xalan -in eds.xml -xsl mk_eds_structs.xsl -indent 0 | \
	indent $(INDENT_OPTS) | sed 's/SED_DELETE/ /g' > eds-structs.h
	cp eds-structs.h ../include

hcan-protocol-referenz.txt: mk_hcan-protocol_wiki.xsl hcan-protocol.xml
	xalan -in hcan-protocol.xml -xsl mk_hcan-protocol_wiki.xsl \
	> hcan-protocol-referenz.txt

clean:
	rm -f hcan-def.h hcan_transport.h hcan_transport.cc \
	frame_message_description.h eds_desc.cc eds-structs.h

.PHONY: validate

