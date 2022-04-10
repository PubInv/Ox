# Public Invention Oxygen Concentrator

## Introduction

Medical grade oxygen is an essential component of respiratory therapy. Typically this is provided by industrial scale cryogenic oxygen concentrators. The COVID-19 pandemic has highlighted the existing need for medical oxygen in field hospital settings around the world. These settings create a variety of challenges that need to be overcome to deliver oxygen to communities around the world.

Human resources are be limited both medically and technically. Medical staff communicate in a variety of local languages and may have little experience. Staff to patient ratios are much higher than is acceptable in a fully equipped hospital. Technical support staff face challenges such as lack of training and basic maintenance equipment.

Field hospital settings may be urban or rural. Environments such as high temperatures, high humidity and fine dust particles create engineering challenges.

Infrastructure and logistics create additional challenges. This is raised the requirement to design a device that can be manufactured locally, along with sufficient documentation to present to local regulatory authorities. Medical device standards must be met and this project aims to meet regulatory requirements.

[Download](docs/PubInvCon2021.pdf) the PIOC presentation from PubInvCon 2021!

Volunteers are needed for this project! Please contact Public Invention if you are interested in joining. Currently we are looking for electrical, mechanical and software engineers.

# Project Status

**2 February 2020**

Development of an test rig has been underway. Unfortunately the pressure sensors originally intended for this purpose were not easy to prototype with which has caused delays. New sensors have been identified for testing, along with additional gas sensors to gain further insight into the process. After much research PCB design for the PIOC will be starting this month.

**10 January 2020**

PIOC was taken apart to diagnose issues in the previous iteration.

During testing the spool valves started to stick. They were disassembled which revealed significant zeolite contamination! The valves were cleaned and reassembled. An attempt to use the valves without lubrication was unsuccessful so a thin coat of silicone oil was used to lubricate the spools. The spool valves will be replaced with poppet valves in the next iteration.

![valve-contamination](docs/img/valve_contamination.jpg)

A compact and low-cost intercooler was built from 1 meter (3 feet) of copper gas pipe and a 120mm 12V fan. The surface area of the copper that is exposed to the fan was maximized using a spiral shape. Droplets of water were observed after the intercooler which demonstrates that the water vapour is still present after the coalescing filter. The water is directed into a reservoir with a 2/2 valve on the end - this should be replaced with an automatic drain in the future to minimize maintenance.

![intercooler](docs/img/intercooler.jpg)

Various configurations of the equalizing valve have been tried and the low pressure of the circuit may allow for the 2 brass valves to be reduced to 1.

A power issue was encountered with the 12V power regulator which needs further investigation. Space for 2 additional valves has also been added to the circuit board.

Some small improvements have been made to the tanks; one end of the threaded rods is fixed to one end-plate to allow for easier assembly/disassembly, push-fit connections are being tested and a new filter design is being tested which uses a fine stainless steel mesh and improved foam filters to filter out the zeolite dust.

The latest build is shown below:

![build-004](docs/img/pioc-build-002.jpg)

**1 November 2020**

It's alive! Running for the first time PIOC reached 95% O2! VentMon is being used to monitor the O2 output, pressure and flowrate. Unfortunately the single stage water filter let enough water vapour past that after a few hours the concentration dropped to 35% - the water separator was half full with a small puddle underneath it. PIOC has been made by hand without much effort to be exacting as to test the idea of manufacturing in an automotive or small engineering workshop - tolerances are +-millimeters not fractions of a millimeter. Next steps are to dry out the zeolite and build a simple chiller and desiccant drier to remove any excess water vapour - some digital pressure sensors will then be installed to tune the system and test various configurations.

![build-002](docs/img/pioc_firstrun.jpg)
![build-002](docs/img/ventmon95o2.jpg)
![build-003](docs/img/pioc_circuit.jpg)

**22 October 2020**

The system is now mounted to the board! Some changes have been made during the build to simplify it - the sieves now use 1/4" NPT barb fittings as a step towards making an all metal "sealed for life" sieve that can be heated to 300C to regenerate the zeolite without opening the sieves. Standardizing the end plates also helps to make manufacturing easier and remove the need to drill and tap 3/8" BSPT which is costly and requires a drill press. The pneumatic circuit has been modified to shorten the overall height. The valves have been tested manually and the overall operation appears to be correct. Next steps are to finish the driver circuits and fill the sieves.

![build-001](docs/img/pioc-build-001.jpg)

**19 October 2020**

First build is almost ready for testing! Parts of the system have been tested independently. Next is to complete assembly for a full pressure test. Then fill the sieves with zeolite and start tuning the control system.

![mockup](docs/img/pioc-mockup.jpg)

## Project Structure

Please see subdirectories for relevant files and documentation.

## Thanks

Many thanks to [Project Apollo](https://github.com/oxycon/ProjectApollo), [Helpful Engineering](https://helpfulengineering.org/) and [EnAccess OpenO2](https://enaccess.org/projects/openo2/) for their ongoing collaboration.
