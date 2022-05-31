```
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.
This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.
See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

# Breath Sim

Breath Sim is a numerical, non-linear, realtime respiratory simulator that implements the model from Marconi et al. This model uses the popular concept of simulating human physiology using electrical circuits. The Boost numerical ODE solver library is used to implement the Runge-Kutta methods - the intial implementation uses the Dormand-Prince solver.

Silvia Marconi, Claudio De Lazzari,
In silico study of airway/lung mechanics in normal human breathing,
https://doi.org/10.1016/j.matcom.2020.05.014.
(https://www.sciencedirect.com/science/article/pii/S0378475420301737)