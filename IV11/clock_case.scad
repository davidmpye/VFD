$fn=50;

material_thickness = 3; //in mm

outer_x = 225 + 2  + material_thickness * 4;
outer_y = 80 + 2  + material_thickness * 4;
outer_z = 30 + 2*material_thickness; //30 inside


x_edge_notches = 6;		//Number of notches along the X side of the box
x_edge_notch_len = 20;

y_edge_notches = 2;		//Number of notches along the Y side of box
y_edge_notch_len = 25;

z_edge_notches = 2;    
z_edge_notch_len = 5;

z_edge_gap_between_notches = ( ((outer_z-2*material_thickness) - (z_edge_notch_len * z_edge_notches)) / z_edge_notches);
//space INSIDE box is as follows:

//X is outer_x  minus 4x material thickness
//Y is outer_y minux 4xmaterial thickness
//Z is outer_z minus 2x material_thickness



long_edge_gap_between_notches = (outer_y - (material_thickness*4) -  (y_edge_notch_len * y_edge_notches)) / y_edge_notches  ;
short_edge_gap_between_notches = (outer_x - (material_thickness*2) -  (x_edge_notch_len * x_edge_notches)) / x_edge_notches;


module top_face() {
radius = 3;
   difference() {
		//square([outer_x,outer_y]);
		hull() {

    		translate([(radius/2), (radius/2), 0])
    		circle(r=radius);

   		translate([(outer_x)-(radius/2), (radius/2), 0])
    		circle(r=radius);

    		translate([(radius/2), (outer_y)-(radius/2), 0])
    		circle(r=radius);

   		 translate([(outer_x)-(radius/2), (outer_y)-(radius/2), 0])
   		 circle(r=radius);		
		}


		for (a=[0:y_edge_notches-1]) {
			translate([material_thickness,  material_thickness*2 + ((long_edge_gap_between_notches * a ) + (a * y_edge_notch_len) +  (0.5 * long_edge_gap_between_notches)) ,0]  ) square([material_thickness,y_edge_notch_len]);
   	}
	
		for (a=[0:y_edge_notches-1]) {
			translate([outer_x - ( material_thickness*2),  (2*material_thickness + (long_edge_gap_between_notches * a ) + (a * y_edge_notch_len) +  (0.5 * long_edge_gap_between_notches)) ,0]  ) square([material_thickness,y_edge_notch_len]);
		}

	for (a=[0:x_edge_notches-1]) {
			
translate([material_thickness + x_edge_notch_len + 0.5*short_edge_gap_between_notches + x_edge_notch_len*a + a*short_edge_gap_between_notches,material_thickness,0]) rotate([0,0,90]) square([material_thickness, x_edge_notch_len]);
		}

	for (a=[0:x_edge_notches-1]) {
			
translate([material_thickness + x_edge_notch_len + 0.5*short_edge_gap_between_notches + x_edge_notch_len*a + a*short_edge_gap_between_notches,outer_y - (material_thickness*2),0]) rotate([0,0,90]) square([material_thickness, x_edge_notch_len]);
		}

	}
}

module long_side_face() {
	union() {
		square([outer_y-(material_thickness*4), outer_z - (material_thickness*2)]);
		//lugs for the long edge
		for (a=[0:y_edge_notches-1]) {
			translate([((long_edge_gap_between_notches * a ) + (a * y_edge_notch_len) +  (0.5 * long_edge_gap_between_notches)),-material_thickness,0]) square([y_edge_notch_len, outer_z]);
		}

			}
		for (a=[0:z_edge_notches - 1 ]) {
			translate([-material_thickness,(0.5 * z_edge_gap_between_notches) + (z_edge_notch_len * a) + (a*z_edge_gap_between_notches),0]) square([material_thickness, z_edge_notch_len]);

			translate([outer_y - 4*material_thickness,(0.5 * z_edge_gap_between_notches) + (z_edge_notch_len * a) + (a*z_edge_gap_between_notches),0]) square([material_thickness, z_edge_notch_len]);
		}



}

module short_side_face() {
	difference() {
		union() {
			square([outer_x - (2*material_thickness), outer_z-(2*material_thickness)]);
			//lugs
			for (a=[0:x_edge_notches-1]) {
				translate([0.5*short_edge_gap_between_notches + x_edge_notch_len*a + a*short_edge_gap_between_notches,-material_thickness,0]) square([x_edge_notch_len,outer_z]);
			}
		}

		//Z notches
		for (a=[0:z_edge_notches - 1 ]) {
			translate([0,(0.5 * z_edge_gap_between_notches) + (z_edge_notch_len * a) + (a*z_edge_gap_between_notches),0]) square([material_thickness, z_edge_notch_len]);

			translate([outer_x - 3*material_thickness,(0.5 * z_edge_gap_between_notches) + (z_edge_notch_len * a) + (a*z_edge_gap_between_notches),0]) square([material_thickness, z_edge_notch_len]);
		}
	}
}

vfd_tube_dia = 24+2;

difference() {
	top_face();
    translate([2*material_thickness, 2*material_thickness, 0]) {
        for (a=[0:7]) {
            //VFD holes
            translate([1 + 14.0625 + (28.125 * a), 40,0]) circle(d=vfd_tube_dia);     
        }
        //the 6 M4 mounting holes.
        //the 6 M4 mounting holes.
        translate([5,5,0]) circle(d=4);
        translate([5,75,0]) circle(d=4);
        translate([225 - 5,5,0]) circle(d=4);
        translate([225 - 5,75,0]) circle(d=4);
        translate([225/2,5,0]) circle(d=4);
        translate([225/2,75,0]) circle(d=4);
    
       //LDR hole if wanted top mounted one
     //  translate([225/2 + 12,5,0]) circle(d=5);
         
       }    
}



translate([0,outer_y + 5,0]) difference() {
	top_face();
      translate([2*material_thickness + 1, 2*material_thickness + 1, 0]) {
       //the 6 M4 mounting holes.
        translate([5,5,0]) circle(d=4);
        translate([5,75,0]) circle(d=4);
        translate([225 - 5,5,0]) circle(d=4);
        translate([225 - 5,75,0]) circle(d=4);
        translate([225/2,5,0]) circle(d=4);
        translate([225/2,75,0]) circle(d=4);
      }


}


translate([material_thickness,outer_y*2 + 5 + material_thickness*2 + 3,0]) rotate(0,0,0) difference() {
    long_side_face();        
}

translate([material_thickness + outer_y,outer_y*2 + 5 + material_thickness*2 + 3,0]) rotate(0,0,0) long_side_face();

translate([material_thickness, outer_y*2 + outer_z + 10 + material_thickness*2 + 3 ,0]) difference() {
    short_side_face() ;
    //This takes a small nick out of the front face to make it easier to mount the LDR forward facing into room.
    translate([outer_x/2, outer_z - material_thickness - 3, 0]) circle(r=3);
}
    

translate([material_thickness, outer_y*2 + outer_z *2 + 15 + material_thickness*2 + 3 ,0]) difference() {
short_side_face();
      translate([2*material_thickness + 1, 2*material_thickness, -1]) {
            //the four switch holes.
            //m4
          
       translate([222.5 - 18.5 - 12.5*0 ,4, 0]) circle(d=5);
       translate([222.5 - 18.5 - 12.5*1 ,4, 0]) circle(d=5);
       translate([222.5 - 18.5 - 12.5*2 ,4, 0]) circle(d=5);
       translate([222.5 - 18.5 - 12.5*3 ,4 ,0]) circle(d=5);

      
      //The USB power connector.
      //14wide, 12 high.
      translate([45,2,0]) square([14,12]);   
          
          
      }
      
}
