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
	sudo docker build --tag pioc .
	docker run --rm -it --name pi pioc bash -c 'cd firmware && platformio lib install && platformio lib list && pio run -e native && .pio/build/native/program'
