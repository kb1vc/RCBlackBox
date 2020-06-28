// Battery retainer for FT power pods.

$fn=32; // round things are drawn in 32 segments

// dimensions in inches
WallThickness = 0.075;
InnerWidth = 1.625;
InnerLength = 3.0;
OuterWidth = InnerWidth + 2 * WallThickness;
OuterLength = InnerLength + 2 * WallThickness;
InnerHeight = 0.45 + 0.2 + 0.08;

OuterHeight = InnerHeight + 2 * WallThickness;

MountHoleOffsetW = 0.025 + WallThickness;
MountHoleOffsetL = 0.25 + WallThickness;
ConnSideWallL = 1.35;

PedestalHeight = 0.2;
PinHeight = PedestalHeight + 0.15;
PinDia = 0.08;
PedestalDia = 0.2;

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

SwitchL = 2.7;
SwitchW = 1.5;
LED_L = 2.2;
LED_W = 1.5;

SwitchDia = 0.2625;
LED_Dia = 0.22;

SlotW = 0.5;
SlotStartL = InnerLength * 0.15;
SlotL = InnerLength * 0.75;

SlotStartW = 0.7; 

module Slot() {
   cylinder(d=SlotW, h = 3*WallThickness);
   translate([SlotL, 0, 0]) cylinder(d=SlotW, h = 3 * WallThickness);
   translate([0, -0.5 * SlotW, -2 * WallThickness])
     cube([SlotL, SlotW, 10 * WallThickness]);
}

module ReliefSlots() {
  translate([SlotStartL, SlotStartW, -WallThickness])
    Slot();
}

CameraSlotL = 0.45;
CameraWid = 0.35;

module Walls() {
  difference() {
    cube([OuterLength, OuterWidth, OuterHeight]);
    union() {
      translate([WallThickness, WallThickness, WallThickness * 0.9]) cube([InnerLength, InnerWidth, OuterHeight]);
    }
  }
}

module BasePlate() {
  union() {
    cube([OuterLength, OuterWidth, WallThickness]);
  }
}

module TopSlot() {
  translate([0, WallThickness, -0.01]) cube([0.3, InnerWidth, WallThickness + 0.02]);
}

scale([25.4,25.4,25.4]) {
  difference() {
     union() {
      BasePlate();
      Walls();
     }
     union() {
       translate([-2*WallThickness, OuterWidth - (CameraSlotL + CameraWid), OuterHeight - CameraWid + 0.01])
         cube([10*WallThickness, CameraWid, CameraWid]);
         for(i = [0:1:5]) {
           translate([WallThickness * 3 + i * 0.5, 0, 0])
  	     TopSlot();
	 }
     }
   }
 }