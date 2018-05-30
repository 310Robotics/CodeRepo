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
============================================================================================================================
Driving Task
When buttons 5U and 7U are pressed at the same time, the drive can shift from tank drive to arcade drive
============================================================================================================================
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
- Proportional gain constant
- Requested values for encoders
- Maximum motor power
- Boolean for disabling and enabling control loop
*/
static float  pidEncoder_Kp = 0.59525;
static float	pidEncoder_Ki = 0.03;
static float pidEncoder_Kd = 0.5;
static int pidEncoderIntegrationLimit = 100;
static float  pidRequestedValueLB;
static float  pidRequestedValueRB;
static int maxEncoderPIDControllerPower = 127;
static bool enableEncoderPIDController = true;
static bool enableEncoderPIDIntegration = true;
static bool enableEncoderPIDDerivative = false;
/*
============================================================================================================================
Proportional gain controller for drive motors.
You can set desired encoder values using
============================================================================================================================
*/
task encoderPIDController()
{
	//intialize error and drive power vars
	int currentLeftEncoder;
	int currentRightEncoder;
	float lastErrorLeft;
	float lastErrorRight;
	float pidErrorLeft;
	float pidErrorRight;
	float pidIntegralLeft;
	float pidIntegralRight;
	float pidDerivativeLeft;
	float pidDerivativeRight;
	float pidDriveLeft;
	float pidDriveRight;
	
	/*
	while(encoder > 100)
		move motor back
	while(encoder < 100)
		move back
	*/
	
	while(true){
		if (enableEncoderPIDController){
			currentLeftEncoder = -SensorValue[leftEncoder];
			currentRightEncoder = SensorValue[rightEncoder];
			//error = desired value - current value
			pidErrorLeft = pidRequestedValueLB - currentLeftEncoder;
			pidErrorRight = pidRequestedValueRB - currentRightEncoder;

			//proportional gain = proportional constant * error
			pidDriveLeft = (pidEncoder_Kp * pidErrorLeft);
			pidDriveRight = (pidEncoder_Kp * pidErrorRight);

			//integrate error
			if (enableEncoderPIDIntegration){
				if (fabs(pidErrorLeft) < pidEncoderIntegrationLimit){
					pidIntegralLeft += pidErrorLeft;
				}
				if (fabs(pidErrorRight) < pidEncoderIntegrationLimit){
					pidIntegralRight += pidErrorRight;
				}
				//add integrated error multiplied by integral gain constant to drive
				pidDriveLeft += (pidEncoder_Ki * pidIntegralLeft);
				pidDriveRight += (pidEncoder_Ki * pidIntegralRight);
			} else {
				pidIntegralLeft = 0;
				pidIntegralRight = 0;
			}
			
			//calculate and apply derivative
			if (enableEncoderPIDDerivative){
				
				//set last error
				lastErrorLeft = pidErrorLeft;
				lastErrorRight = pidErrorRight;
			} else {
				
			}

			//limit drive power
			if( pidDriveLeft > maxEncoderPIDControllerPower )
				pidDriveLeft = maxEncoderPIDControllerPower;
			if( pidDriveLeft < (-maxEncoderPIDControllerPower) )
				pidDriveLeft = (-maxEncoderPIDControllerPower);
			if( pidDriveRight > maxEncoderPIDControllerPower)
				pidDriveRight = maxEncoderPIDControllerPower;
			if( pidDriveRight < (-maxEncoderPIDControllerPower) )
				pidDriveRight = (-maxEncoderPIDControllerPower);

			//run motors at calculated power
			motor[LB] = pidDriveLeft;
			motor[RB] = pidDriveRight;

			//don't hog CPU
			wait1Msec( 25 );
		} else {
			pidErrorLeft = 0;
			pidErrorRight = 0;
			pidIntegralLeft = 0;
			pidIntegralRight = 0;
			pidDriveLeft = 0;
			pidDriveRight = 0;
		}
	}
}


//- Set ticks per inch on wheel
static int ticksPerInch = 29;

/*
============================================================================================================================
Move robot forward
============================================================================================================================
*/
void moveForward(int tenthsOfInch){
	//YO YOU SHOULD REALLY TEST THIS
	int tickGoal = (ticksPerInch * tenthsOfInch) / 10;
	SensorValue[leftEncoder] = 0;
	SensorValue[rightEncoder] = 0;
	pidRequestedValueLB = tickGoal;
	pidRequestedValueRB = tickGoal;
}
//- Set ticks per 90 degrees on wheel
static int ticksPerDeg = 0;
/*
============================================================================================================================
Rotate robot
============================================================================================================================
+ http://www.robotc.net/wikiarchive/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Using_encoders_to_make_turns
*/
void rotateRight(int degrees){
	int tickGoal = ticksPerDeg * degrees;
	pidRequestedValueLB = -tickGoal;
	pidRequestedValueRB = tickGoal;
}
void rotateLeft(int degrees){
	int tickGoal = ticksPerDeg * degrees;
	pidRequestedValueLB = tickGoal;
	pidRequestedValueRB = -tickGoal;
}


//- Select an auton method
static int selectedAuton = 1;

/*
============================================================================================================================
LCD program method
============================================================================================================================
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
			wait1Msec(250);
		}
		if (nLCDButtons == rightButton){
			currentDisplay++;
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
				wait1Msec(1000);
				break;
			case VarsMode:
				maxDisplays = maxVarsDisplays;
				displayLCDCenteredString(0, "|Global Vars|");
				wait1Msec(1000);
				break;
			case AutonMode:
				maxDisplays = maxAutonDisplays;
				displayLCDCenteredString(0, "|Auton Selector|");
				wait1Msec(1000);
				break;
			default:
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
				if (select){
					SensorValue[leftEncoder] = 0;
				}
				break;
			case 2:
				string re = SensorValue(rightEncoder);
				displayLCDCenteredString(0, "Encoder RB");
				displayLCDCenteredString(1, re);
				if (select){
					SensorValue[rightEncoder] = 0;
				}
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
				displayLCDCenteredString(0, "isTankDrive");
				if (isTankDrive){
					displayLCDCenteredString(1, "true");
					} else {
					displayLCDCenteredString(1, "false");
				}
				if (select){
					isTankDrive = !isTankDrive;
					wait1Msec(250);
				}
				break;
			case 1:
				displayLCDCenteredString(0, "allowDriveToggle");
				if (allowDriveToggle){
					displayLCDCenteredString(1, "true");
					} else {
					displayLCDCenteredString(1, "false");
				}
				if (select){
					allowDriveToggle = !allowDriveToggle;
					wait1Msec(250);
				}
				break;
			case 2:
				string ekp = pidEncoder_Kp;
				displayLCDCenteredString(0, "pidEncoder_Kp");
				if (select){
					pidEncoder_Kp += 0.1;
					displayLCDCenteredString(1, ekp);
					if (pidEncoder_Kp > 2.5){
						pidEncoder_Kp = 0.1;
					}
				} else {
					displayLCDCenteredString(1, ekp);
				}
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
				displayLCDCenteredString(1, "Description");
				break;
			case 1:
				if (select){
					selectedAuton = 2;
				}
				if (selectedAuton == 2){
					displayLCDCenteredString(0, "[Auton 2]");
					} else {
					displayLCDCenteredString(0, "Auton 2");
				}
				displayLCDCenteredString(1, "Description");
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
				displayLCDCenteredString(1, "Description");
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
				displayLCDCenteredString(1, "Description");
				break;
			case 4:
				if (select){
					selectedAuton = 5;
				}
				if (selectedAuton == 5){
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


/*
============================================================================================================================
Pre-auton Task
============================================================================================================================
*/
task pre_auton(){
	lcdProgram();
}

/*
============================================================================================================================
Auton methods
============================================================================================================================
*/
void auton1(){
}
void auton2(){
}
void auton3(){
}
void auton4(){
}

/*
============================================================================================================================
Auton Task
============================================================================================================================
*/
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

/*
============================================================================================================================
Main Task
============================================================================================================================
*/
task main(){
	startTask(encoderPIDController);
	//startTask(toggleDrive);

	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;
	//moveForward(20);
	pidRequestedValueLB = 100;
	pidRequestedValueRB = 100;

	while(true){
		datalogDataGroupStart();
		datalogAddValue( 0, SensorValue[rightEncoder] );
	 	datalogAddValue( 1, SensorValue[leftEncoder] );
		datalogDataGroupEnd();

		//lcdProgram();

	}
}
