// Battery retainer for FT power pods.

$fn=32; // round things are drawn in 32 segments

// dimensions in inches
WallThickness = 0.075;
InnerWidth = 1.625;
InnerLength = 3.0;
OuterWidth = InnerWidth + 2 * WallThickness;
OuterLength = InnerLength + 2 * WallThickness;
InnerHeight = 0.3;
// InnerHeight = 1.25;
OuterHeight = InnerHeight + 2 * WallThickness;

MountHoleOffsetW = 0.025 + WallThickness;
MountHoleOffsetL = 0.25 + WallThickness;
ConnSideWallL = 1.35;

PedestalHeight = 0.17;
PinHeight = PedestalHeight + 0.15;

PinDia = 0.072; 
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

GyroBracePosW = OuterWidth - 0.3;
GyroBracePosL = 1.;
GyroBraceL = 1.0;
module GyroBrace() {
  translate([GyroBracePosL, GyroBracePosW, 0])
    cube([GyroBraceL, WallThickness, InnerHeight]);
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
      translate([WallThickness, 5 * WallThickness, WallThickness * 0.9])
        cube([ConnSideWallL, InnerWidth, OuterHeight]);
    }
  }
}

module SwitchLED() {
    translate([SwitchL, SwitchW, -5 * WallThickness])
      cylinder(d=SwitchDia, h = 10 * WallThickness);
    translate([LED_L, LED_W, -5 * WallThickness])
      cylinder(d=LED_Dia, h = 10 * WallThickness);
}

module BasePlate() {
  union() {
    translate([0, 0, WallThickness]) MountingPins();
    cube([OuterLength, OuterWidth, WallThickness]);
  }
}

scale([25.4,25.4,25.4]) {
  difference() {
    union() {
      BasePlate();
      Walls();
      GyroBrace();
    }
    union() {
      SwitchLED();
      ReliefSlots();
    }
  }
}