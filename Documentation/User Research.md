# Design Document
## Public Invention Oxygen Concentrator


### 1.0 User Needs Research

User research has been collected from the community at www.helpfulengineering.org, www.pubinv.org and one-on-one interviews.

#### 1.1 Countries

##### 1.1.1. Kenya

- Currently locally distributing oxygen cylinders from bottling facilities.
- Difficulty tracking and distributing oxygen cylinders due to theft.
- Difficulty purchasing and importing large shipments of medical aid.
- Low financial and technical resources.
- Lack of trained medical staff.
- Lack of basic medical facilities.

##### 1.1.2. Guatemala
- Lack of basic resources such as water and staff

##### 1.1.3. Afghanistan
- Home care patients currently take oxygen cylinders to fill stations.
- Cylinders cost of $200USD each, which is expensive for average citizen.
- Currently ~1500 cylinders per day in the entire country, 1 patient needs 4 cylinder per day 5-6 days
- People present to specialist hospitals at late stages (stigma), now 42 hospitals (none have central O2 supply), level of realibility of skillset poor - high fatalities
- Infection rates are very high
- What equipment?
- Cost?
- How would we use it?
- What concentration or level of O2 will they supply?
- What mechanism can we use to intro the tech into the country?
- Two approaches - central facilities for hospitals (20 person ward) or individual CPAP
- Cryogenic facilities - fill tanks, fill cryo tanks, industrial, not practical for rural areas
(estimates of 1 metric tonne for 20 beds per day)
- Pressure swing/membrane technology - these consume a lot of power
- Secure powersupply or allow intermittent power supply
- Containerised high pressure cylinders - backup
- Specify powersupply, geography, size of institution
- Afghanistan hospital (Kabul - dedicated 75 beds for COVID) 100 beds in another, most vary between 20-50 beds or smaller in provincial
- bring your own cylinder ($250 per cylinder, $7 per fill)
- oxygen is industrial grade not medical grade (possibly affecting level of treatment)
- what regulatory standards do we have to meet? Perhaps none - little but requests for proposals may reference international standards.
- Current: is industrial O2 with no oversight.
- Cost proportionate to level of standard of filter (perhaps target somewhere midway)
- Hospitals - 10-20 beds up to 2,3-400 beds
- Hospitals in provences (~50)
- Maybe target 20 people
- Oxygen balloon
- Don't speak English, understand content etc. eg. they don't understand error messages
- Price point needs to be competitive.
- Bring in finished items attract customs duties
- How much can you produce in-country?
- What are the costs? Long, short term viability
- 15-20L/min for high flow oxygen therapy
- 1-3 day period with supplimental oxygen. 10-15PLM high flow oxygen.
- Goal is keep people out of hospitals.

##### 1.1.4. Indonesia
- High temperatures and humidity
- Demand but no research has been conducted by open source teams

##### 1.1.5. India
- High temperatures
- Manufacturing capacity
- Engineering capacity
- Challenges of rural locations, infrastructure, training remain

##### 1.1.6 Nepal
- Power supply spikes and interuptions
- Language barriers
- Lack of infrastructure
- Floors are not flat
- Car battery might be used
- Dust on the floor gets sucked into ground level intakes; put the units on stands/tables
- Long life ceramic bearing fans
- Modular components that are easy to replace (plug and play)
- Humidity causes mold build up on HEPA filters

#### 1.2 Proposed Generic Solution

The proposed generic solution is to mail essential components such as the electronics, valves and zeolite. The larger and heavier materials such as aluminum plates and cylinders can be sourced locally and manufactured using basic workshop tools such as drill presses and lathes - simple 3 axis CNC machining could be used in countries with the capability to manufacture at scale.

The proposed design connects to a standard engineering workshop-style pneumatic ring main. It is assumed that a locally sourced compressor will be used to supply a group of oxygen concentrators - this is likely to be a diesel compressor from a local mechanic or engineering workshop. This will save shipping the heaviest component, the oil-less compressor, from overseas suppliers - these compressors also draw a lot of power which creates a fire risk with poor electrical infrastructure.

An industrial compressor will not be oil-less, as such the air must first be filtered to become medical grade air, before the oxygen concentration process begins. Modular air filtration units are proposed to be flexible to the users needs - these could be per unit or per group of units. The assumption is a filter setup will be more compact, lighter to ship and be more robust and long lasting compared to compressors.

The electrical demand must be low and adapt to a wide range of voltages, from a 12V car battery to a 480V power spike. Electricity is not a given and availability is irregular. Therefore the electronics and valves should work at 12V.

The shipped device must be lightweight and compact to minimize the burden on logistics.

The device must be robust to survive the supply chain, including bumpy roads and being dropped from trucks.

A multi-lingual touch screen with a visual interface to using icons and language agnostic visual design to communicate visually to the end user.

Maintenance periods must be as long as possible, with tool-less servcing and frequently used spare parts included within the device, eg. washers, fuses etc.

#### 1.3 Questions Remaining

1. What is the flow rate required? Have received 5,10, 15 and 20LPM as the target. 20LPM is the target for this device, although this could be as low as 5LPM for a portable 12V at-home device, or multiple 100LPM+ units for a small clinic. Currently keeping the design flexible to accomodate changing user needs.
2. What is the FiO2 required? Current target is minimium 90% O2, ideally 95% O2. Costs raise disproportionally beyond 95% O2 as further steps to remove Argon are required.
3. What is the distribution mechanism of the devices? Carried by hand, bicycles, cars, trucks?
4. What are the local manufacturing capabilities? Currently assumed to be a car mechanics workshop, up to basic engineering facilities to service local industries.
5. Some countries could have great capabilities for production at scale, for example India. What are these capacities? Potential for partnerships?
6. What local universities and governing bodies could support local research and development of the proposed generic design?
7. How would local manufacture and testing work in practice? Keeping in mind the need for 'clean room' conditions.
8. Our current best research indicates the need for a bedside unit connected to a remote air supply providing 20LPM for high flow nasal canular oxygen therapy. This needs to be confirmed as medically impactful.
9. Demand for oxygen concentrators for invasive ventilation has not been analysed.
10. What documentation would need to be produced for local health authorities? Currently working to FDA requirements.
11. What languages would the documenation need to be translated to?
12. What manufacturing guides would be required? Can paper or 3D printed jigs/templates be made and distributed to aid manufacturing?
13. What bureaucratic and security concerns are there? Large containers may be held up at customs and are targets for theft.
14. How would servicing work? What service facilities or training is required for maintenance of the devices?
15. What is the demand for rural care vs hospital care? Stigma and population distrition creates rural demand, however hospitals have better infrastructure, staff and presumably shorter distance to engineering workshops.
16. What is the current state of the electrical wiring in the hospital and the load that can be placed on it?
17. How can we make the cost competitive with existing solutions?
18. How can we overcome stigma? What local words, symbols, colors are taboo and should be avoided in our design?
19. What international manufacturers need to be contacted? Multiple vendors have been identified and outreach has begun to supply electronics, valves and zeolite.
20. What local manufacturers, assemblers, service agents need to be contacted?
