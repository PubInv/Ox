# Public Invention's Ox Project is an open source hardware design for an oxygen concentrator for use by field hospitals around the world. This design concept aims to design an oxygen concentrator that can be manufactured locally while overcoming challenges posed by human resources, hospital location (geographically), infrastructure and logistics; in addition, this project attempts minimum documentation of their design standards necessary for local approval whilst tackling regulatory #requirements for medical devices. Copyright (C) 2021 Robert Read, Ben Coombs, and Darío Hereñú.
#
# This program is free Firmware/Hardware designs: you can redistribute, use, study it and/or modify it under the terms of the CERN Open Hardware License Version 2 as published here: https://ohwr.org/cern_ohl_s_v2.txt
#
# This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the CERN Open Hardware License Version 2 for more details.
# You should have received a copy of the CERN Open Hardware License Version 2 along with this program.  If not, see https://ohwr.org/cern_ohl_s_v2.txt.
#
# For some reason 'sudo' is required before the commands when using Mac OSX

pio-run-mac:
	cd firmware \
	sudo pio run -e native -v \
	&& .pio/build/native/program -v

pio-run-linux:
	cd firmware \
	pio run -e native \
	&& .pio/build/native/program


# Build and run PlatformIO native tests in a Docker container.
docker-test-native:
	sudo docker build --tag pioc .
	docker run --rm -it --name pi pioc bash -c 'cd firmware && platformio lib install && platformio lib list && pio test -e native'

# Build and run PlatformIO uno tests in a Docker container.
docker-test-uno:
	docker build --tag pioc .
	docker run --rm -it --name pi pioc bash -c 'cd firmware && platformio lib install && platformio lib list && pio test -e uno'

# Build and run src/main.cpp in a Docker container
docker-pio-run:
	docker build --tag pioc .
	docker run --rm -it --name pi pioc bash -c 'cd firmware && platformio lib install && platformio lib list && pio run -e native && .pio/build/native/program'

# Build and run on FeatherESP32 and start serial monitor
pio-run-esp32:
	cd firmware \
	&& pio run -e featheresp32 -t upload \
	&& pio device monitor
