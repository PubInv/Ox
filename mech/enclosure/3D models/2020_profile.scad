/*
***INFO***
Author: slurked
Email: slurked@starlid.com
Version: 1.0
License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0)



***TO INSTALL***
To use, place this file in your OpenSCAD libraries directory. If you don't know where this is on your computer, go to the file menu and select "Show Library Folder" or check https://en.wikibooks.org/wiki/OpenSCAD_User_Manual/Libraries


***USAGE EXAMPLE***
This generates a 75mm tall 2020 extrusion:

    use <2020_profile.scad>

    linear_extrude(height=75) 2020_profile();


***NOTES***

Please be aware that there are many different variations in real world aluminum extrusions. Differences in tolerances, minor details like corner bevels, and even major variations in the interior cross section are common. This particular model is very basic with square edges.

This is a trivial item, so I have elected to release it into the public domain via the Creative Commons Zero license. No credit, attribution, or permission is needed from me to use this work. Although I would still enjoy hearing about what you put it to use for!

*/

module 2020_profile(){
difference(){
    
    square([20,20],center=true);
    square([17,17],center=true);

    square([40,5.26],center=true);
    rotate([0,0,90]) square([40,5.26],center=true);
}

difference(){
    union(){
        square([7.32,7.32],center=true); 
        rotate([0,0,45]) square([1.5,25],center=true);
        rotate([0,0,-45]) square([1.5,25],center=true);
    }
    circle(d=5,$fn=16);
}

translate([7.9975,7.9975,0]) square([4.005,4.005],center=true);
translate([-7.9975,7.9975,0]) square([4.005,4.005],center=true);
translate([7.9975,-7.9975,0]) square([4.005,4.005],center=true);
translate([-7.9975,-7.9975,0]) square([4.005,4.005],center=true);
}