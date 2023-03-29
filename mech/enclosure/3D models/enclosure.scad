include <2020_profile.scad>
include <aluminium_extrusion_plate_brackets.scad>
//total dimensions height 11" (279.4mm) , width 17" (431.8mm) , depth 42"(1066.8 mm) or 48" (1219.2mm)  24" (609.6mm)

//300mm 11.8"
//500mm 19.685"

//12 pieces in total to form a box
//(11" or(279.4mm)) - (2 x 20mm) = 239.4mm cut length
//(17" or (431.8mm)) -  (2 x 20mm) = 391.8mm cut length



module 2020_mm(length =0){linear_extrude(height = length, center = true, convexity = 10,  slices = 20, scale = 1.0, $fn = 16){
2020_profile();
}}

//height
echo();
translate([-205.9,0,0])2020_mm(239.4); //
echo();
translate([205.9,0,0])2020_mm(239.4);
echo();
translate([-205.9,589.6,0])2020_mm(239.4); //
echo();
translate([205.9,589.6,0])2020_mm(239.4);


//width
echo();
translate([0,0,129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,0,-129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,589.6,129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,589.6,-129.7])rotate([0,90,0])2020_mm(391.8);

//Depth
translate([-205.9,294.8,129.7])rotate([90,0,0])2020_mm(609.6);
translate([-205.9,294.8,-129.7])rotate([90,0,0])2020_mm(609.6);
translate([205.9,294.8,129.7])rotate([90,0,0])2020_mm(609.6);
translate([205.9,294.8,-129.7])rotate([90,0,0])2020_mm(609.6);

//mount plate
echo();
translate([0,100,-129.7])rotate([0,90,0])2020_mm(391.8);
echo();
translate([0,359.4,-129.7])rotate([0,90,0])2020_mm(391.8);
translate([-215.9,90,-119.7])cube([431.8,279.4,6]);

//support bracket top
translate([205.9,589.6,139.7])rotate([0,0,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,589.6,139.7])rotate([0,0,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,139.7])rotate([0,0,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,139.7])rotate([0,0,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket bottom
translate([205.9,589.6,-139.7])rotate([0,180,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,589.6,-139.7])rotate([0,180,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,-139.7])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,-139.7])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket rear
translate([205.9,589.6,-139.7])rotate([0,180,270])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,589.6,-139.7])rotate([0,180,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,0,-139.7])rotate([0,180,90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,0,-139.7])rotate([0,180,180])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket left
translate([-215.9,589.6,-129.7])rotate([90,180,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,589.6,129.7])rotate([90,270,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,0,-129.7])rotate([90,90,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-215.9,0,129.7])rotate([90,0,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );

//support bracket right
translate([215.9,589.6,-129.7])rotate([-90,90,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,589.6,129.7])rotate([-90,180,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,0,-129.7])rotate([-90,0,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([215.9,0,129.7])rotate([-90,270,-90])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );



//support bracket back
translate([205.9,599.6,-129.7])rotate([-90,0,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,599.6,-129.7])rotate([-90,90,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([205.9,599.6,129.7])rotate([-90,270,0])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );
translate([-205.9,599.6,129.7])rotate([-90,180,00])aluminium_extrusion_bracket( shape = "L", support = "full", center = true );