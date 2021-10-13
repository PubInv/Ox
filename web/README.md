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

# Ox Web Interface

Ox uses a web GUI inspired by Vent Display. Popular desktop GUI frameworks such as Qt and wxWidgets were evaluated and it was determined that a web GUI would be better fit the requirements of the project. Similar to Vent Display, the Ox web GUI aims to provide engineering, medical and device management functionality.

## Design
In the spirit of simplicity, the frontend uses vanilla JavaScript, HTML5 and CSS3. The backend is also JavaScript using NodeJS with the popular minimalist Express framework. NPM is used for package management. Code should conform to the latest standards and best practices such as ECMA script. The [Mozilla MDN](https://developer.mozilla.org/en-US/docs/Web#web_technology_references) is the default reference material.

[TimescaleDB](https://www.timescale.com/) is used to store PIRDS data. It is an extension to PostgreSQL for time-series data and is fully compatible with the Postgres ecosystem.

## Development

### Environment
Ox Web is developed on Ubuntu with VS Code. It is compatible with [VS Code Remote Development](https://code.visualstudio.com/docs/remote/remote-overview) so that it can be developed and operated on any low-cost computer such as a Raspberry Pi, in a container or on WSL.

### Quickstart

Setup and run Ox Web:

1. Install packages: `npm install`
1. Create database: `docker run -d --name timescaledb -p 5432:5432 -e POSTGRES_PASSWORD=password timescale/timescaledb:latest-pg12-oss`
1. Perform database schema migration: `npx sequelize db:migrate`
1. Start server: `npm run serve`
1. Push dummy signal into database: `python3 scripts/signal.py`
1. Go to `http://localhost:3020` and you should see a sine wave appear in the plot.