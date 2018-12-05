$fn = 50;

module spacer() {
difference() {
    circle(d=8);
    circle(d=4);
    
    
}}



for (i = [0:10]){
    for (j = [0:6]) {
    translate([j*8.2,  i*8.2, 0]) spacer();
}
}