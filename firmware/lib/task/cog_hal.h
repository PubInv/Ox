/*
  cog_hal.h -- configuration specifically for the Stage2 HAL of the high-oxygen experiment

  Copyright 2023, Robert L. Read

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
*/

#ifndef COG_HAL_H
#define COG_HAL_H

#include <machine.h>
#include <abstract_ps.h>


class COG_HAL : public MachineHAL {
public:
  const static int NUM_FANS = 1;
  SanyoAceB97 _fans[NUM_FANS];
  const int NUM_HEATERS = 1;
  const static int NUM_STACKS = 1;
  AbstractPS* _stacks[NUM_STACKS];
  bool init() override;
  void _updateFanPWM(float unitInterval);
  const int HEATER_PINS[1] = {51};
};

#endif
