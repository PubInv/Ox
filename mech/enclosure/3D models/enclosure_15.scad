include <2020_profile.scad>
include <aluminium_extrusion_plate_brackets.scad>
//total dimensions height 11" (279.4mm) , width 17" (431.8mm) , depth 42"(1066.8 mm) or 48" (1219.2mm)  24" (609.6mm)

//300mm 11.8"
//500mm 19.685"

//12 pieces in total to form a box
//(11" or(279.4mm)) - (2 x 20mm) = 239.4mm cut length
//(17" or (431.8mm)) -  (2 x 20mm) = 391.8mm cut length

//author Lawrence Kincheloe 2023

module 2020_mm(length =0){linear_extrude(height = length, center = true, convexity = 10,  slices = 20, scale = 1.0, $fn = 16){
2020_profile();
}}

//height
echo();
translate([-205.9,0,0])2020_mm(239.4); //
echo();
translate([205.9,0,0])2020_mm(239.4);
echo();
translate([-205.9,379.4,0])2020_mm(239.4); //
echo();
translate([205.9,379.4,0])2020_mm(239.4);


//width
echo();
translate([0,0,129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,0,-129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,379.4,129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,379.4,-129.7])rotate([0,90,0])2020_mm(391.8);

//Depth
translate([-205.9,189.7,129.7])rotate([90,0,0])2020_mm(399.4);
translate([-205.9,189.7,-129.7])rotate([90,0,0])2020_mm(399.4);
translate([205.9,189.7,129.7])rotate([90,0,0])2020_mm(399.4);
translate([205.9,189.7,-129.7])rotate([90,0,0])2020_mm(399.4);

//mount plate

//translate([0,100,-129.7])rotate([0,90,0])2020_mm(391.8); //front support beam
translate([0,179.4,-129.7])rotate([0,90,0])2020_mm(391.8); //middle support
//translate([0,359.4,-129.7])rotate([0,90,0])2020_mm(391.8); //rear beam
//translate([-215.9,90,-119.7])cube([431.8,279.4,6]); //11"x17" sheet
translate([-215.9,10,-119.7])cube([431.8,361,6]); //large sheet option

//support bracket mount plate
//middle support
translate([205.9,179.4,-139.7])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,179.4,-139.7])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//middle mount plate

//translate([0,100,0])rotate([0,90,0])2020_mm(391.8); //front support

//translate([0,379.4,0])rotate([0,90,0])2020_mm(391.8); //back support
translate([0,179.4,0])rotate([0,90,0])2020_mm(391.8); //middle support
translate([205.9,189.7,0])rotate([90,0,0])2020_mm(359.4);
translate([-205.9,189.7,0])rotate([90,0,0])2020_mm(359.4);
translate([-215.9,90,10])cube([431.8,279.4,3]);

//support bracket middle mount plate
translate([215.9,379.4,0])rotate([90,0,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,379.4,0])rotate([0,270,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,0,0])rotate([90,0,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,0,0])rotate([90,270,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
//middle support
translate([205.9,179.4,-10])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,179.4,-10])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );



//support bracket top
translate([205.9,379.4,139.7])rotate([0,0,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,379.4,139.7])rotate([0,0,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,139.7])rotate([0,0,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,139.7])rotate([0,0,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket bottom
translate([205.9,379.4,-139.7])rotate([0,180,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,379.4,-139.7])rotate([0,180,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,-139.7])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,-139.7])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket rear
translate([205.9,379.4,-139.7])rotate([0,180,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,379.4,-139.7])rotate([0,180,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,-139.7])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,-139.7])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket left
translate([-215.9,379.4,-129.7])rotate([90,180,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,379.4,129.7])rotate([90,270,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,0,-129.7])rotate([90,90,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,0,129.7])rotate([90,0,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket right
translate([215.9,379.4,-129.7])rotate([-90,90,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,379.4,129.7])rotate([-90,180,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,0,-129.7])rotate([-90,0,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,0,129.7])rotate([-90,270,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );



//support bracket back
translate([205.9,389.4,-129.7])rotate([-90,0,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,389.4,-129.7])rotate([-90,90,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,389.4,129.7])rotate([-90,270,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,389.4,129.7])rotate([-90,180,00])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );