
$fn = 50;

module spacer(height) {
difference() {
    cylinder(d=8, h=height);
    translate([0,0,-0.1]) cylinder(d=4, h=height+1);


}}



for (i = [0:2]) {
    for (j = [0:1]) {
    translate([j*8.2,  i*8.2, 0]) spacer(24);
}
}


translate([17,0,0]) for (i = [0:2]) {
    for (j = [0:1]) {
    translate([j*8.2,  i*8.2, 0]) spacer(3);
}
}
