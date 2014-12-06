$fn = 50;
/*
// bøjle
 difference() {
	translate([10,-60,0]) cube([6,120,80]);
	translate([5,0,40]) rotate([0,90,0]) cylinder(r=15/2,h=20);
}
 difference() {
	translate([-97,-60,0]) cube([10,120,80]);
	translate([-120,0,40]) rotate([0,90,0]) cylinder(r=4,h=50);
 }

*/

//color("gray")translate([-1,0,40]) rotate([0,-90,0]) {
difference() {
	// c = 2*PI*r <=> c = 2*3.14..*20 <=> c = 125.6mm
	// c2 = 126.6x2 <=> 251.2 = 2*3.14*r <=> r = 251.2/2*3.14 <=> r = 40 
	union() {
		translate([0,0,0])		cylinder(r1=20,r2=30,h=10,$fn=200);	
		translate([0,0,10]) cylinder(r=30,h=5,$fn=200);
		translate([0,0,15]) cylinder(r1=30,r2=15,h=10,$fn=200);
		translate([0,0,25]) cylinder(r=15,h=30,$fn=200);
		translate([0,0,55]) cylinder(r2=30,r1=15,h=10,$fn=200);
		translate([0,0,65]) cylinder(r=30,h=5,$fn=200);
	}

 // shaft
 // - motor side
 difference() {
	translate([0,0,-0.01]) 
		cylinder(r=4.15,h=15,$fn=100);
	translate([3.1,-5,-0.2]) cube([10,10,16]);
 }
 // shaft - ball bearing
	translate([0,0,62.5]) cylinder(r=21/2,h=8);
	translate([0,0,60]) cylinder(r=15/2,h=5);

	// tightening screw
	translate([0,0,6]) {
		rotate([0,90,0]) cylinder(r=1.8,h=41);
		rotate([0,90,90]) cylinder(r=1.8,h=41);

	}



	// debug
//	translate([-40/2,-40/2,-0.01]) { cube([42,42,20]); }

}

//}