#include "usercontrol.h"

userControl::userControl(robotChasis *robot, bool dM){
  simp = robot;
  driverMode = dM;
  simp->set_drive_break_type(coast);
}

void userControl::flyWheelToggle(){
  if ((flyLastPress == false) && simp->Controller1.ButtonX.pressing()) flyWheelOn = !flyWheelOn;
  flyLastPress = simp->Controller1.ButtonX.pressing();

  if ((flySpeedToggle == false) && simp->Controller1.ButtonY.pressing()) flyFast = !flyFast;
  flySpeedToggle = simp->Controller1.ButtonY.pressing();

  flyWheelPow = flyFast ? 100 : 80;

  if(flyWheelOn) simp->flyOuttake.spin(fwd, flyWheelPow, pct);
  else simp->flyOuttake.spin(fwd, 0, pct);
}

void userControl::storageRoller(){
  
  if (simp->Controller1.ButtonL1.pressing()){
    simp->leftIntake.spin(fwd, -10, percentUnits::pct);
    simp->rightIntake.spin(fwd, 10, percentUnits::pct);
    simp->rollerIntake.spin(directionType::fwd, 40, pct);
  } 
  else if (simp->Controller1.ButtonL2.pressing()) {
    simp->rollerIntake.spin(directionType::fwd, 40, pct);
    simp->leftIntake.spin(fwd, -50, percentUnits::pct);
    simp->rightIntake.spin(fwd, 50, percentUnits::pct);
  }else simp->rollerIntake.spin(directionType::fwd, 0, pct);
}

void userControl::setBrakeMode(){
  if(simp->Controller1.ButtonB.pressing()) simp->set_drive_break_type(hold);
  else if(simp->Controller1.ButtonA.pressing()) simp->set_drive_break_type(coast);
}

void userControl::intakeM(){
  if(simp->Controller1.ButtonR2.pressing()){
    simp->leftIntake.spin(fwd, -100, percentUnits::pct);
    simp->rightIntake.spin(fwd, 100, percentUnits::pct);
    if(!simp->shootD.pressing()) simp->rollerIntake.spin(directionType::fwd, 60, pct);
    else simp->rollerIntake.stop(brakeType::hold);

  } else if(simp->Controller1.ButtonR1.pressing()){ 
    simp->leftIntake.spin(fwd, 100, percentUnits::pct);
    simp->rightIntake.spin(fwd, -100, percentUnits::pct);
    simp->rollerIntake.spin(directionType::fwd,-90, pct);
  } else { 
    simp->leftIntake.spin(fwd, 0, percentUnits::pct);
    simp->rightIntake.spin(fwd, 0, percentUnits::pct);
    storageRoller();
  }
}

void userControl::driveM(){
  a3 = simp->Controller1.Axis3.position(pct) * 120;
  a4 = simp->Controller1.Axis4.position(pct) * 120;
  a1 = simp->Controller1.Axis1.position(pct) * 120;

  if((std::abs(a3) > 960) || (std::abs(a4) > 960) || (std::abs(a1) > 960)){
    simp->frontRight.spin(fwd, a3 - a4 - a1, voltageUnits::mV);
    simp->frontLeft.spin(fwd, -a3 - a4 - a1, voltageUnits::mV);
    simp->backRight.spin(fwd, a3 + a4 - a1, voltageUnits::mV);
    simp->backLeft.spin(fwd, -a3 + a4 - a1, voltageUnits::mV);
  } else {
    simp->frontRight.spin(fwd, 0, voltageUnits::mV);
    simp->frontLeft.spin(fwd, 0, voltageUnits::mV);
    simp->backRight.spin(fwd, 0, voltageUnits::mV);
    simp->backLeft.spin(fwd, 0, voltageUnits::mV);
  }
}

void userControl::driveMA(){
  a3 = simp->Controller1.Axis3.position(pct) * 120;
  a4 = simp->Controller1.Axis4.position(pct) * 120;
  a1 = simp->Controller1.Axis2.position(pct) * 120;

  double currAngle = ((simp->getPI()/180)*simp->gyroM.heading());
  
  simp->frontLeft.spin(fwd, (a4*cos(simp->get_flbr()-currAngle) + a3*sin(simp->get_flbr()-currAngle)) - a1, voltageUnits::mV);
  simp->frontRight.spin(fwd, -(a4*cos(simp->get_frbl()-currAngle) + a3*sin(simp->get_frbl()-currAngle)) - a1, voltageUnits::mV);
  simp->backLeft.spin(fwd, (a4*cos(simp->get_frbl()-currAngle) + a3*sin(simp->get_frbl()-currAngle)) - a1, voltageUnits::mV);
  simp->backRight.spin(fwd, -(a4*cos(simp->get_flbr()-currAngle) + a3*sin(simp->get_flbr()-currAngle)) - a1, voltageUnits::mV);
}

void userControl::setDriveMode(){
  if(simp->Controller1.ButtonLeft.pressing()) driverMode = false;
  else if(simp->Controller1.ButtonRight.pressing()) driverMode = true;
}

void userControl::driveLoop(){
  while(true){
    intakeM();
    flyWheelToggle();
    setBrakeMode();
    setDriveMode();

    if(driverMode) driveM();
    else driveMA();

    wait(20, msec);
  }
}