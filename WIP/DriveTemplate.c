#pragma config(Sensor, dgtl1,  rightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEncoder,    sensorQuadEncoder)
#pragma config(Motor,  port4,           LB,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           RB,            tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// define absolute value
#define abs(X) ((X < 0) ? -1 * X : X)

/*
TODO:
= add P (or PI) controller for stabilizing both sides of drive for encoder PID loop
	+ http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_drive_some_distance
= add integral gain to
= full pre-auton LCD selection screen
= methods for moving robot forward and turning via encoderPIDController, static constant float for configuring ticks per inch
= motor slew rate task:
	+ https://www.vexforum.com/index.php/6146-robotc-programming-tips/p2#p53799
	+ https://renegaderobotics.org/what-is-slew-rate/
*/

/*
- Set default drive type
- Allow drive toggling with 5U + 7U
*/
static bool isTankDrive = false;
static bool allowDriveToggle = true;

/*
Driving Task
When buttons 5U and 7U are pressed at the same time, the drive can shift from tank drive to arcade drive
*/
task toggleDrive(){
	int left, right;
	while(true){
		if (allowDriveToggle && vexRT[Btn5U] == 1 && vexRT[Btn7U] == 1){
			isTankDrive = !isTankDrive;
		}
		if(isTankDrive){
			motor[LB] = vexRT[Ch3];
			motor[RB] = vexRT[Ch2];
			} else {
			left = vexRT[Ch3] + vexRT[Ch1];
			right = vexRT[Ch3] - vexRT[Ch1];
			motor[LB] = left;
			motor[RB] = right;
		}
	}
}

/*
Proportional gain controller for drive motors.
You can set desired encoder values using
*/

/*
- Proportional gain constant
- Requested values for encoders
- Maximum motor power
- Boolean for disabling and enabling control loop
*/
static float  pid_Kp = .7;
static float  pidRequestedValueLB;
static float  pidRequestedValueRB;
static int maxEncoderPIDControllerPower = 70;
static bool enableEncoderPIDController = true;

task encoderPIDController()
{
	//intialize error and drive power vars
	float pidErrorLeft;
	float pidErrorRight;
	float pidDriveLeft;
	float pidDriveRight;

	while(true){
		if (enableEncoderPIDController){
			//error = desired value - current value
			pidErrorLeft = pidRequestedValueLB - SensorValue[leftEncoder];
			pidErrorRight = pidRequestedValueRB - SensorValue[rightEncoder];

			//drive power = proportional constant * error
			pidDriveLeft = (pid_Kp * pidErrorLeft);
			pidDriveRight = (pid_Kp * pidErrorRight);

			//limit drive power
			if( pidDriveLeft > maxEncoderPIDControllerPower )
				pidDriveLeft = maxEncoderPIDControllerPower;
			if( pidDriveLeft < (-maxEncoderPIDControllerPower) )
				pidDriveLeft = (-maxEncoderPIDControllerPower);
			if( pidDriveRight > maxEncoderPIDControllerPower )
				pidDriveRight = maxEncoderPIDControllerPower;
			if( pidDriveRight < (-maxEncoderPIDControllerPower) )
				pidDriveRight = (-maxEncoderPIDControllerPower);

			//run motors at calculated power
			motor[LB] = pidDriveLeft;
			motor[RB] = pidDriveRight;

			//don't hog CPU
			wait1Msec( 25 );
		}
	}
}


/*
- Set ticks per inch on wheel
*/
static int ticksPerInch = 29;

/*
Move robot forward
*/
void moveForward(int tenthsOfInch, int power){
	//YO YOU SHOULD REALLY TEST THIS
	int tickGoal = (ticksPerInch * tenthsOfInch) / 10;
	SensorValue[leftEncoder] = 0;
	SensorValue[rightEncoder] = 0;
	pidRequestedValueLB = tickGoal;
	pidRequestedValueRB = -tickGoal;
}

/*
Turn robot
	+ http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_make_turns
*/
void rotate(int degrees, int power){

}

/*
- Select an auton method
*/
static int selectedAuton = 1;

/*
Pre-auton includes has an LCD menu that allows
*/
void lcdProgram(){
	bLCDBacklight = true;

	const short leftButton = 1;
	const short centerButton = 2;
	const short rightButton = 4;
	const short leftRightButtons = 5;
	bool select = false; //changed to true every time center button is pressed

	int maxSensorDisplays = 4;
	int maxVarsDisplays = 4;
	int maxAutonDisplays = 5;
	int maxDisplays = maxSensorDisplays;
	int currentDisplay = 0;

	int currentMode = 0;
	const short SensorMode = 0;
	const short VarsMode = 1;
	const short AutonMode = 2;
	
	displayLCDCenteredString (0, "Starting...");
	wait1Msec(1000);
	clearLCDLine(0);
	clearLCDLine(1);

	while (true){
	//while (bIfiRobotDisabled == 1) {
		select = false;

		if (nLCDButtons == leftButton){
			currentDisplay--;
			//currentDisplay = currentDisplay % maxDisplays;
			wait1Msec(250);
		}
		if (nLCDButtons == rightButton){
			currentDisplay++;
			//currentDisplay = currentDisplay % maxDisplays;
			wait1Msec(250);
		}
		if (nLCDButtons == centerButton){
			select = true;
		}

		if (nLCDButtons == leftRightButtons){
			clearLCDLine(0);
			clearLCDLine(1);
			currentDisplay = 0;
			currentMode++;
			if (currentMode > 2){
				currentMode = 0;
			}
			switch(currentMode){
				case SensorMode:
					maxDisplays = maxSensorDisplays;
					displayLCDCenteredString(0, "|Sensors|");
					wait1Msec(500);
					break;
				case VarsMode:
					maxDisplays = maxVarsDisplays;
					displayLCDCenteredString(0, "|Global Vars|");
					wait1Msec(500);
					break;
				case AutonMode:
					maxDisplays = maxAutonDisplays;
					displayLCDCenteredString(0, "|Auton Selector|");
					wait1Msec(500);
					break;
				default:
					//do nothing
			}
		}
		if (currentDisplay < 0){
			currentDisplay = 3;
		}
		if (currentDisplay > 3){
			currentDisplay = 0;
		}
		switch(currentMode){
			case SensorMode:
				switch(currentDisplay){
					case 0:
						string batteryLevel = nAvgBatteryLevel;
						displayLCDCenteredString(0, "Battery Voltage");
						displayLCDCenteredString(1, batteryLevel);
						break;
					case 1:
						string le = SensorValue[leftEncoder];
						displayLCDCenteredString(0, "Encoder LB");
						displayLCDCenteredString(1, le);
						break;
					case 2:
						string re = SensorValue(rightEncoder);
						displayLCDCenteredString(0, "Encoder RB");
						displayLCDCenteredString(1, re);
						break;
					case 3:
						displayLCDCenteredString(0, "Sensor 4");
						displayLCDCenteredString(1, "empty");
						break;
					default:
						displayLCDCenteredString(0, "default case");
						break;
				}
				break;
			case VarsMode:
				switch(currentDisplay){
					//If select is true, change values
					case 0:
						displayLCDCenteredString(0, "var 1");
						break;
					case 1:
						displayLCDCenteredString(0, "var 2");
						break;
					case 2:
						displayLCDCenteredString(0, "var 3");
						break;
					case 3:
						displayLCDCenteredString(0, "var 4");
						break;
					default:
						displayLCDCenteredString(0, "default case");
						break;
				}
				break;
			case AutonMode:
				switch(currentDisplay){
					case 0:
						if (select){
							selectedAuton = 1;
						}
						if (selectedAuton == 1){
							displayLCDCenteredString(0, "[Auton 1]");
						} else {
							displayLCDCenteredString(0, "Auton 1");
						}
						break;
					case 1:
						if (select){
							selectedAuton = 2;
						}
						if (selectedAuton == 2){
							displayLCDCenteredString(0, "[Auton 2]");
						} else {
							displayLCDCenteredString(0, "Auton 2");
						};
						break;
					case 2:
						if (select){
							selectedAuton = 3;
						}
						if (selectedAuton == 3){
							displayLCDCenteredString(0, "[Auton 3]");
						} else {
							displayLCDCenteredString(0, "Auton 3");
						}
						break;
					case 3:
						if (select){
							selectedAuton = 4;
						}
						if (selectedAuton == 4){
							displayLCDCenteredString(0, "[Auton 4]");
						} else {
							displayLCDCenteredString(0, "Auton 4");
						}
						break;
					case 4:
						if (select){
							selectedAuton = 5;
						}
						if (selectedAuton == 4){
							displayLCDCenteredString(0, "[No Auton]");
						} else {
							displayLCDCenteredString(0, "No Auton");
						}
						break;
					default:
						displayLCDCenteredString(0, "default case");
						break;
				}
				break;
			default:
				//do nothing
		}
	}
	clearLCDLine(0);
	clearLCDLine(1);
	bLCDBacklight = false;
}

task pre_auton(){
	lcdProgram();
}

void auton1(){
}
void auton2(){
}
void auton3(){
}
void auton4(){
}
task autonomous(){
	if (selectedAuton == 1){
		auton1();
	}
	if (selectedAuton == 2){
		auton2();
	}
	if (selectedAuton == 3){
		auton3();
	}
	if (selectedAuton == 4){
		auton4();
	}
}

task main(){
	//startTask(encoderPIDController);
	//startTask(toggleDrive);

	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;

	while(true){
		lcdProgram();
	}
}
