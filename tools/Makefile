include ../ARCH.inc

TARGET = hcan-backup-config hcan-discover hcan-restore-config

.PHONY: all install

all: 
	make setEnvironment
	make install

install:
	sudo install $(TARGET) /usr/bin

# Umgebungsvariable fuer telican setzen:
setEnvironment:
	cp ./bash_profile ~/.bash_profile
	cp ./bash_profile ~/.xsessionrc
	@find ~/.xsessionrc -type f -exec sed -i "s/127.0.0.1/$(IP_HI)/g" {} \;
	@find ~/.bash_profile -type f -exec sed -i "s/127.0.0.1/$(IP_HI)/g" {} \;	
	@#less ~/.bash_profile
	@#less ~/.xsessionrc
	

clean:
	cd /usr/bin/; sudo rm -f $(TARGET)
