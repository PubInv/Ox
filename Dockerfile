FROM ubuntu:latest

# Public Invention's Ox Project is an open source hardware design for an oxygen concentrator for use by field hospitals around the world. This design concept aims to design an oxygen concentrator that can be manufactured locally while overcoming challenges posed by human resources, hospital location (geographically), infrastructure and logistics; in addition, this project attempts minimum documentation of their design standards necessary for local approval whilst tackling regulatory #requirements for medical devices. Copyright (C) 2021 Robert Read, Ben Coombs, and Darío Hereñú.

# This program is free Firmware/Hardware designs: you can redistribute, use, study it and/or modify it under the terms of the CERN Open Hardware License Version 2 as published here: https://ohwr.org/cern_ohl_s_v2.txt

# This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# See the CERN Open Hardware License Version 2 for more details.
# You should have received a copy of the CERN Open Hardware License Version 2 along with this program.  If not, see https://ohwr.org/cern_ohl_s_v2.txt.


# Set the working directory.
WORKDIR /app

# Install all dependencies - Python, PyLint, PyTest, GCC etc
RUN apt-get -yq update && apt-get upgrade -yq \
&& apt-get install python3.7 -y \
#&& ln -s /usr/bin/python3 /usr/bin/python \
&& apt-get install pylint -y \
&& apt-get install -y --no-install-recommends bzip2 curl unzip \
&& apt-get install -yq ca-certificates \
&& apt-get install -y build-essential \
&& apt-get install inotify-tools -y \
&& apt-get install python3-pip -y \
&& pip3 install pandas validator_collection pyyaml tabulate \
&& pip3 install -U pytest \
&& apt install git -y

# Install Arduino CLI and PlatformIO CLI
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh \
&& curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py \
&& python3 get-platformio.py

# Set path so PlatformIO works in Bash
ENV PATH=${PATH}:/app/bin:~/.platformio/penv/bin

# Copy all local files to the docker container
COPY . .
