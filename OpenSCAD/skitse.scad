
// platform
translate([0,350,185]) {
	difference() {
	 cube([400,600,140],center=true);
	 translate([0,0,8]) cube([400-8,600-8,140-2],center=true);
//	translate([0,300,-2])	cylinder(r=150,h=200,center=true);
	translate([0,300,-2]) cube([300,50,150],center=true);
	}
}
// left tube
translate([-250-25,0,0]) {
	rotate([-90,0,0]) cylinder(r=110/2,h=800);
	translate([0,950,0]) rotate([90,0,0]) cylinder(r1=20,r2=110/2,h=150);
//	translate([0,-150,0]) rotate([-90,0,0]) cylinder(r1=20,r2=110/2,h=150);
	hull() {
		translate([0,0,0]) rotate([90,0,0]) cylinder(r=110/2,h=1);
		translate([0,-149,-50+20]) rotate([90,0,0]) cylinder(r=20,h=1);
	}

}
// right tube
translate([250+25,0,0]){
  rotate([-90,0,0]) cylinder(r=110/2,h=800);
	translate([0,950,0]) rotate([90,0,0]) cylinder(r1=20,r2=110/2,h=150);
//	translate([0,-150,0]) rotate([-90,0,0]) cylinder(r1=20,r2=110/2,h=150);
	hull() {
		translate([0,0,0]) rotate([90,0,0]) cylinder(r=110/2,h=1);
		translate([0,-149,-50+20]) rotate([90,0,0]) cylinder(r=20,h=1);
	}
}
// crossmembers
translate([0,100,(110/2)+50]) cube([290*2,40,20],center=true);
translate([-250-25,100,(110/2)+20]) cube([30,40,50],center=true);
translate([250+25,100,(110/2)+20]) cube([30,40,50],center=true);

translate([0,550,(110/2)+50]) cube([290*2,40,20],center=true);
translate([-250-25,550,(110/2)+20]) cube([30,40,50],center=true);
translate([250+25,550,(110/2)+20]) cube([30,40,50],center=true);


// front boom 
rotate([0,40,0]) translate([-200,750,20]) cylinder(r=10,h=450,center=true);
rotate([0,-40,0]) translate([200,750,20]) cylinder(r=10,h=450,center=true);

translate([0,750,250]) rotate([0,90,0]) cylinder(r=20,h=20,center=true);

// water
translate([0,0,-450]) color("blue",0.4) cube([6000,6000,1000],center=true);
