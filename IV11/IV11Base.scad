//Uses Nophead's polyhole.
module polyhole(h, d) {
    n = max(round(2 * d),3);
    rotate([0,0,180])
        cylinder(h = h, r = (d / 2) / cos (180 / n), $fn = n);
}

$fn=80;
total_height = 6;
neopixel_width = 6.25;
tube_dia = 22;
wall_thickness = 1;
base_thickness = 4;
num_pins = 12;
pin_diameter = 12;
pin_hole_dia = 0.8;

difference() {
  cylinder(r=(tube_dia/2) + wall_thickness, h = total_height); 
  translate([0,0,base_thickness]) cylinder(r=tube_dia/2, h = 100);  
  translate([-neopixel_width/2,-neopixel_width/2,-0.1]) cube([neopixel_width, neopixel_width,100]);
  for (i=[0:num_pins-1]) rotate([0,0,360/num_pins * i]) translate([pin_diameter/2,0,-0.1]) polyhole(100, pin_hole_dia);
}
