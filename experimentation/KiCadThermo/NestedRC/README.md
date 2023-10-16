## Ideas for Simulating Nested Structure of the Stack inside a Heat Exchanger using SPICE

Motivated by ideas such as found at: https://lpsa.swarthmore.edu/Systems/Thermal/SysThermalModel.html#:~:text=To%20model%20this%20system%20with,to%20the%20thermal%20system%20above.  

Lee has made a circuit model using a current source as a heat source and resistors and capacitors to represent the heat exchanged, then nesting another resistor and capacitor to represent the stack and added a currrent (sink) as a non linear load (O2 and heat out) for the PLANT as a the load of the ODECS.


### Schematic Viewer
Stale screen shot of schematic but with circuit blocks outlines.  
![image](https://github.com/ForrestErickson/NASA-COG/assets/5836181/d7302b8b-ab68-4fd0-838d-41e82ac632ad)


[Select this link for current schematic ***View on KiCanvas***.](https://kicanvas.org/?github=https%3A%2F%2Fgithub.com%2FPubInv%2FNASA-COG%2Fblob%2Fdevelop%2Fexperimentation%2FKiCadThermo%2FNestedRC%2FNestedRC.kicad_sch)  
_(Unfortunatly the KiCanvase view does not show the dashed outlines around circuit blocks.)_


**Notes**  
* The resistors have units DegreeC/Watt   
* The thermo capicatance should be in J/kg-K and should be Farad, however for the schematic capacitors are in uF and we have to enter as very large numbers of uF.  
* The current source are Watts.  
* The V out is actualy Degrees C.  

### Simulation results
![RiseTimes.png](foo.png)

### Limitations / Errors in Approximations

This simulation does not represent air flow. It is as if only thermoconducation through a solid was responsible for all heat flow.
