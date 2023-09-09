# NASA-COG

## Final Report

The final report delivered to NASA for this project can be [read here](https://drive.google.com/file/d/1jfJcSLfsybuC49I7llPhfTNI8ucRUW3h/view?usp=sharing).

## Introduction


This is a repo for a control system made for NASA to drive a Ceramic Oxygen Generation
technology they have developed. It was forked from ["The Ox"]() project of Public Invention run by Mr. Ben Coombs, which was in turn a fork of the VentOS project of Helpful Engineering let my Ben Coombs, Dr. Erich Schulz, and Robert L. Read.

However, the main reason we are starting with this fork is not that the Pressure Swing
Adsorption system of the Ox is a particularly relevant, but rather that the code
represents a nice platform of simple microcontroller control of related machines.

To wit, includes PlatformIO configuration, a state machine (finite automata),
a superloop architecutre, and a hardware abstraction layer.

This particular project of NASA will improve life support technology in space,
but may eventually greatly improve global health.

An unlisted [YouTube video](https://youtu.be/1nq-Mw7jZ5A) has been uploaded that shows this work in progress as of September, 2022.

## How to run

### Linux
The fundamental way to run this is to perform:  
> cd firmware  
> pio run -e native && ./.pio/build/native/program  
We are currently creating the [RibbonFish](https://github.com/PubInv/NASA-COG/blob/develop/RibbonFish.md) proof-of-concept.
This can be run on a Due by executing:  
> make pio-run-due_ribbonfish  
As this develops, it will use a specific Due hardware configuration,  
please see the [documentation](https://github.com/PubInv/NASA-COG/blob/develop/RibbonFish.md) for details to build your own.  

### Windows
With Visual Studio Code.  
Install the Plateform IO extention  
To open he project in Visual Studio Code.  
Open Project. Navigate to the firmware folder.   
Select the file platformio.ini  


## Annoyances

AFAIK, PlatformIO forces a directory tree depth of 5 levels.

This is not enough to organize the Hardware Abstraction Level they way I would prefer, forcing us to make some messy directories that are less than clear.

For example, to implement a Temperature Sensor, we would ideally have an
abstract (or virtual) implementation, a folder for real hardware,
and a folder for mock sensors.  I have no choice now but to jam
the mock sensors into the same directory with the real hardware and
use a naming convnention.

## The Command System

The NASA-COG system needs to allow the user to enter commands. The most obvious of these is "turn off".
Because the system should avoid thermal shocks, this is actually not simple.

We have previously used an [approach](https://github.com/PubInv/PIRCS-pubinv-respiration-control-standard) of using simple JSON objects to represent commands. We propose to use the same idea here, but obviously,
the command will be custom to the NASA-COG.

### Parameter Setting Commands
The general scheme is to use a JSON object like:

```JavaScript
{ "com": "T",
  "par": 7,
  "mod": "T",
  "val" : 732.3
  }
```

1. "com" is the command. For example, "T" can mean "set Temperature".
2. "par" is a parameter used to specify something about how to execute the command. For example, 7 could mean location 7, which might be between the 6th and 7th stack.
3. "mod" is a modifier of the command. For example, the "set Temperature" command could be used to
set the minimum, maximum, or the target temperature.
4. "val" is the value that is to be set.

For this problem space, we can specify some global parameters of the problem, some of which
involve safety. These can of course have default values "hard wired" into the code, but an
experimenter may wish to change these values (carefully).

1. The maximum rate of voltage change applied to a stack. Nominally we have defined this as 1 volt/second.
We have evidence that too rapid a voltage change (such as that caused by pulse-width modulation) can destroy
a stack.
2. The maximum rate of temperature change for a stack. Nominally we may say this as 2 degree K per minute,
which would allow the machine to reach operating temperature in 6 hours.
3. The maximum temperature difference across a stack in any dimenstion. We may not be able to control
this directly, but we can use it as a target.
4. The maximum voltage applied to a stack.
5. The maximum amperage applied to a stack.

### Event-like Commands

We will need other commands that represent discrete events in time. For example, each transition in the
state transition diagram below may be initiated by a command from the user (they may also occur
automatically due to an internal fault or even normal operation.)

![COG State Machine](https://user-images.githubusercontent.com/5296671/180069835-ce7cdff7-c445-45c1-967d-089810e837db.png)

We can implement particular commands with the same scheme.

For example, the simple object:

```JavaScript
{ "com": "C",
  "par": 0,
  "int": "s",
  "mod": "U",
  "val" : 0.0
  }
```

Could mean "Cool Down". The value "U" is used becasue the "mod" command is "unusued" in this case.

```JavaScript
{ "com": "W",
  "par": M,
  "int": "s",
  "mod": "U",
  "val" : 0.0
  }
```


In actual practice, these commands will be pasted into the serial port using either PlatformIO or the Arduino IDE.

Eventually, a user interface may be implemented that uses a knob or a nice webpage to enter commands.
However, by having each of these take only actions that can be implemented as such a command,
we bring discipline to the internal code.

## Actual Serial Commands

The serial port currently supports 6 one-character commands: w,c,e,a,i,o.
These mean:
1. warmup
2. cool down
3. emergency shutdown
4. acknowledge
5. idle
6. operate

Note: "acknowledge" means that the operator acknowledges that a critical
state has occurred and asserts it is okay to return to normal operation,
either because the problem as been removed or for some other reason.

Addtionally, the system supports "full commands", that look like this:

> { "com": "W", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

> { "com": "C", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

> { "com": "E", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

> { "com": "A", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

> { "com": "I", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

> { "com": "O", "par": "M", "int": "s", "mod": "U", "val" : 0.0 }

In fact, each "one character" command is a short hand for these commands.

The following format can be used to set parameters (P).
The three most important parameters are:
1. T : Temperature (in C) going into the stack
2. C : Current (in milliamps) going into the stack
3. F : Fan speed in RPM (we expect this to change to flow rate in SLM)
> { "com": "P", "par": "T", "int": "T", "mod": "U", "val" : 35 }

> { "com": "P", "par": "C", "int": "T", "mod": "U", "val" : 500 }

> { "com": "P", "par": "F", "int": "T", "mod": "U", "val" : 2000 }

These three commands set the:
1. Temperature into the stack to 35C
2. Set the Target current into the stack to 500 milliamps
3. Set the flow rate to 2000 ml/seconds




## License

In general we intend to follow the [Public Invention Licensed Guidelines](https://github.com/PubInv/PubInv-License-Guidelines) unless
NASA requests something different. If you are starting your own project, please
feel free to fork the license guidelines as a starting point if you like our
license policy.

This program includes free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

See the GNU Affero General Public License for more details. You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
