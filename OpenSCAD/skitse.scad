// left
translate([-250-25,0,0]) rotate([-90,0,0]) cylinder(r=25,h=1000);
// right
translate([250+25,0,0]) rotate([-90,0,0]) cylinder(r=25,h=1000);
// rear
translate([0,-25,0]) rotate([0,90,0]) cylinder(r=25,h=500,center=true);

//middle boom
translate/