// Battery retainer for FT power pods.

$fn=32; // round things are drawn in 32 segments

Epsilon = 0.001;
// dimensions in inches
WallThickness = 0.075;
InnerWidth = 1.25;
InnerLength = 3.0;
OuterWidth = InnerWidth + 2 * WallThickness;
OuterLength = InnerLength + 2 * WallThickness;
InnerHeight = 0.3;
// InnerHeight = 1.25;
OuterHeight = InnerHeight + 2 * WallThickness;

MountHoleOffsetW = 0.025 + WallThickness;
MountHoleOffsetL = 0.25 + WallThickness;
ConnSideWallL = 1.0;
ConnSideWallOffset = 0.45;
ConnSideWallLift = 0.15;

PedestalHeight = 0.17;
PinHeight = PedestalHeight + 0.2;

PinDia = 0.065; 
PedestalDia = 0.225;

// dimensions in mm for a little while
mm_pinL = 58.0;
mm_pinW = 30.0 - 7.0;
mm_pinOffL = 3.5;
mm_pinOffW = 3.5;
// Now convert -- this is sad, as we're going to convert back at the bottom.
PinSpaceL = mm_pinL / 25.4;
PinSpaceW = mm_pinW / 25.4;
PinLocRL = (mm_pinOffL / 25.4) + MountHoleOffsetL;
PinLocRW = (mm_pinOffW / 25.4) + MountHoleOffsetW;

CornerPostOuter = 0.2;

SlotW = 0.5;
SlotStartL = InnerLength * 0.15;
SlotL = InnerLength * 0.75;

SlotStartW = 0.7;

module CornerPost(x, y)
{
  cph = 2 * InnerHeight;
  difference() {
    cube([x, y, cph]);
    translate([WallThickness, WallThickness, -Epsilon])
      cube([x, y, cph + 2 * Epsilon]);
  }
}

module CornerPosts() {
  translate([WallThickness, OuterWidth - WallThickness, WallThickness])
    rotate([0, 0, -90])
      CornerPost(CornerPostOuter * 0.5, CornerPostOuter);
  translate([OuterLength - WallThickness, OuterWidth - WallThickness, WallThickness])
    rotate([0, 0, 180])
      CornerPost(CornerPostOuter, CornerPostOuter);
  translate([OuterLength - WallThickness, WallThickness, WallThickness])
    rotate([0, 0, 90])
      CornerPost(CornerPostOuter, CornerPostOuter);
  // the whole post won't fit in this corner, as it fouls the camera
  translate([2 * WallThickness, WallThickness, WallThickness])
    CornerPost(CornerPostOuter,   WallThickness);
  
}


module Pedestal(dir) {
  union() {
    cylinder(d = PedestalDia, h = PedestalHeight);
    cylinder(d = PinDia, h = PinHeight);
  }
}

module MountingPins() {
  translate([PinLocRL, PinLocRW, 0]) {
    translate([0, 0, 0]) Pedestal(1);
    translate([PinSpaceL, 0, 0]) Pedestal(1);
    translate([PinSpaceL, PinSpaceW, 0]) Pedestal(0);
    translate([0, PinSpaceW, 0]) Pedestal(0);
  }
}

module Slot() {
   cylinder(d=SlotW, h = 10*WallThickness);
   translate([SlotL, 0, 0]) cylinder(d=SlotW, h = 10 * WallThickness);
   translate([0, -0.5 * SlotW, -2 * WallThickness])
     cube([SlotL, SlotW, 10 * WallThickness]);
}

module ReliefSlots() {
  translate([SlotStartL, SlotStartW, -WallThickness])
    Slot();
}


module Walls() {
  difference() {
    cube([OuterLength, OuterWidth, OuterHeight]);
    union() {
      translate([WallThickness, WallThickness, WallThickness * 0.9]) cube([InnerLength, InnerWidth, OuterHeight]);
      translate([WallThickness + ConnSideWallOffset, 5 * WallThickness, ConnSideWallLift])
        cube([ConnSideWallL, InnerWidth, OuterHeight]);
    }
  }
}


module BasePlate() {
  union() {
    translate([0, 0, WallThickness]) MountingPins();
    cube([OuterLength, OuterWidth, WallThickness]);
  }
}

scale([25.4,25.4,25.4]) {
  union() {
     CornerPosts();  
     difference() {
       union() {
         BasePlate();
         Walls();
       }
       union() {
         ReliefSlots();
       }
     }
  }
}