#
# Public Invention's Ox Project is an open source hardware design for an oxygen
# concentrator for use by field hospitals around the world. This team aims to
# design an oxygen concentrator that can be manufactured locally while overcoming
# challenges posed by human resources, hospital location (geographically),
# infrastructure and logistics; in addition, this project attempts the minimum
# documentation expected of their design for international approval whilst
# tackling regulatory requirements for medical devices. Copyright (C) 2021
# Robert Read, Ben Coombs, and Darío Hereñú.
#
# This program includes free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# See the GNU Affero General Public License for more details.
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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

# Build and run on Arduino Mega 2560 and start serial monitor
pio-run-mega:
	cd firmware \
	&& pio run -e megaatmega2560 -t upload \
	&& pio device monitor

# Build and run on Arduino Due
pio-run-due:
	cd firmware \
	&& pio run -e due -t upload \
	&& pio device monitor


pio-run-native:
	cd firmware \
	&& pio run -e native
# Not working for some reason so in the firmware directory just run:
# pio run -e native && ./.pio/build/native/program

pio-clear-cache:
	cd firmware \
	&& rm -rf .pio

# These tests are not working well right now
ribbonfish_tests:
	cd firmware \
	&& pio test -vvv -e due_ribbonfish -f "test_*"

test_stack_power:
	cd firmware \
	&& pio test -v -e due_ribbonfish -f "test_stack_*"
	&& pio device monitor --filter direct

test_parse_script:
	cd firmware \
	&& pio test -v -e due_ribbonfish -f "test_parse_*"
	&& pio device monitor --filter direct

# DANGER! THE INPUT DOESN'T WORK FROM THIS
# MAKEFILE. BUT, if you make this target,
# and, then break it, and then "make run_monitor"
# that makes sense
test_SanyoAceB97:
	cd firmware \
	&& pio test -v -e due_ribbonfish -f "test_Sanyo*"

test_heater_ramp:
	cd firmware \
	&& pio test -v -e due_ribbonfish -f "test_heater_ramp"

stage2_heater:
	cd firmware \
	&& pio test -v -e due_ribbonfish -f "test_stage2_heater"

run_monitor:
	cd firmware \
	&& pio device monitor --filter=direct --baud=115200


pio-run-due_ribbonfish:
	cd firmware \
	&& pio run -e due_ribbonfish -t upload \
	&& pio device monitor --filter=direct --baud=115200
