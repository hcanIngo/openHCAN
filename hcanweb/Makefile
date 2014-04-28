include ../ARCH.inc

TARGET = hcanweb

install:
	make clean
	
	@#make install_libhcanServer
	@#make install_C1612Server
	
	make not_min
	@#make min
	make install_hcanweb
	make clean

clean:
	make clean_hcanweb
	make clean_C1612Server
	cd ./client/app/javascript; rm -f hcanweb.min.js
	cd ./client/app/javascript; rm -f installationBsp.xml
	cd ./client/jsSource; rm -f combined.js

clean_hcanweb:
	sudo rm -f /usr/share/javascript/*.js
	sudo rm -f /usr/share/css/*.css
	sudo rm -f /usr/share/apache2/icons/*.css
	sudo rm -f /usr/share/apache2/icons/48/*.png

clean_C1612Server:	
	cd server/C1612server; make clean

install_C1612Server:
	cd server/C1612server; make load adr=166

install_hcanweb:
	sudo cp -r --remove-destination ./client/installation.xml /var/www/
	sudo chmod 755 /var/www/*.xml
	
	@# /var/www (Dokumentenroot des Apache2 unter Ubuntu 12.04): 
	sudo cp -r --remove-destination ./client/app/*.html /var/www/ # Apache2: http://localhost/index.html
	sudo cp -r --remove-destination ./client/app/*.xml /var/www/
	sudo chmod 755 /var/www/*.html
	sudo chmod 755 /var/www/*.xml

	# Achtung im Folgenden nicht /usr/share/apache2/... sondern /usr/share/...	
	test -d /usr/share/javascript || sudo mkdir /usr/share/javascript
	test -d /usr/share/javascript/images || sudo mkdir /usr/share/javascript/images
	test -d /usr/share/javascript/images/icons-png || sudo mkdir /usr/share/javascript/images/icons-png
	test -d /usr/share/javascript/images/my48 || sudo mkdir /usr/share/javascript/images/my48
	
	sudo cp -r --remove-destination ./client/app/javascript/*.js /usr/share/javascript/
	sudo cp -r --remove-destination ./client/app/javascript/*.css /usr/share/javascript/
	sudo cp -r --remove-destination ./client/app/javascript/images/icons-png/*.png /usr/share/javascript/images/icons-png/
	sudo cp -r --remove-destination ./client/app/javascript/images/*.gif /usr/share/javascript/images/
	sudo cp -r --remove-destination ./client/app/javascript/images/my48/*.png /usr/share/javascript/images/my48
	
	sudo chmod 755 /usr/share/javascript/*.js
	sudo chmod 755 /usr/share/javascript/*.css
	sudo chmod 755 /usr/share/javascript/images/*.png
	sudo chmod 755 /usr/share/javascript/images/*.gif
	sudo chmod 755 /usr/share/javascript/images/my48/*.png
	
	sudo /etc/init.d/apache2 restart	
	@# epiphany-browser "http://localhost/Einstellungen.html"
	epiphany-browser "http://localhost/index.html"
	@# firefox "http://localhost/index.html"
	@# chromium-browser "http://localhost/index.html"
	
install_libhcanServer:
	cd ./server/libhcanServer; make clean; make all;
	sudo install ./server/libhcanServer/binary_$(ARCH)/$(TARGET) /usr/lib/cgi-bin/$(TARGET)
#	@# install ./server/libhcanServer/binary_$(ARCH)/$(TARGET) /usr/lib/cgi-bin/$(TARGET) # Apache2: /cgi-bin/hcanweb

min:
	cd ./client/jsSource; cat *.js > combined.js
	cd ./client/jsSource; /usr/bin/yui-compressor combined.js -o ../app/javascript/hcanweb.min.js
	cd ./client/jsSource; sudo rm combined.js
	#
	cd ./client; cp installation.xml ./app/installationBsp.xml

not_min:
	cd ./client/jsSource; cat *.js > combined.js
	cd ./client/jsSource; sudo cp -r --remove-destination combined.js ../app/javascript/hcanweb.min.js
	#cd ./client/jsSource; sudo rm combined.js
	#
	cd ./client; cp installation.xml ./app/installationBsp.xml

zip: 
	@echo -----client-zip erstellen---------
	@echo Bsp.:  make zip ver=151
	@echo ----------------------------------
	make version
	# 
	@#make not_min
	make min
	cd ./client/app; /usr/bin/zip -r /home/il/Dropbox/HCAN/Hausautomatisierung_bak/hcanweb_Ajax_bak/app_$(ver).zip ./
	make clean

version:
	cd ./client/jsSource; echo 'var globalVersion = "hcanweb - Version 1.0 ('`date +%Y-%m-%d`')";' > appVersion.js

start_local_client:
	@#epiphany-browser "./client/index.html" &
	firefox "./client/index.html" &

shlibhcanServer: # Um im Terminal die Ausgaben des libhcanServer anzuschauen
	./server/libhcanServer/binary_$(ARCH)/$(TARGET)