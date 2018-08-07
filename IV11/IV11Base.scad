//Uses Nophead's polyhole.
module polyhole(h, d) {
    n = max(round(2 * d),3);
    rotate([0,0,180])
        cylinder(h = h, r = (d / 2) / cos (180 / n), $fn = n);
}

$fn=80;
total_height = 15;
neopixel_width = 6.25;
tube_dia = 20;
wall_thickness = 1;
base_thickness = 12;
num_pins = 12;
pin_diameter = 12;
pin_hole_dia = 1.8;

//Half with lugs.
difference() {
  cylinder(r=(tube_dia/2) + wall_thickness, h = total_height); 
  translate([0,0,base_thickness]) cylinder(r=tube_dia/2, h = 100);  
  translate([-0.01,0,0]) cylinder(d=12.5, h=100); 
    
  //translate([-neopixel_width/2,-neopixel_width/2,-0.1]) cube([neopixel_width, neopixel_width,100]);
  for (i=[0:num_pins-1]) rotate([0,0,(360/num_pins/2) + 360/num_pins * i]) translate([pin_diameter/2,0,-0.1]) polyhole(100, pin_hole_dia);
      
  
  //chop it in half.
 translate([0,-25,0]) cube([50,50,50]);

# translate([-3,-10,0]) cube([5,20,5]);
}

//Half without lugs!
translate([10,0,0]) rotate([0,0,180]) difference() {
  cylinder(r=(tube_dia/2) + wall_thickness, h = total_height); 
  translate([0,0,base_thickness]) cylinder(r=tube_dia/2, h = 100);  
  translate([-0.01,0,0]) cylinder(d=12.5, h=100); 
    
  //translate([-neopixel_width/2,-neopixel_width/2,-0.1]) cube([neopixel_width, neopixel_width,100]);
  for (i=[0:num_pins-1]) rotate([0,0,(360/num_pins/2) + 360/num_pins * i]) translate([pin_diameter/2,0,-0.1]) polyhole(100, pin_hole_dia);
      
    difference() {
        translate([0,-25,0]) cube([50,50,50]);
        translate([-2,-10,0]) cube([5,20,5]);
    }

}



