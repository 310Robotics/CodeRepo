#pragma config(Sensor, dgtl1,  rightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEncoder,    sensorQuadEncoder)
#pragma config(Motor,  port4,           LB,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           RB,            tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
TODO:
= add P (or PI) controller for stabilizing speeds for both drive tasks
= add integral gain to
= full pre-auton LCD selection screen
= methods for moving robot forward and turning via encoderPIDController, static constant float for configuring ticks per inch
= motor slew rate task:
	+ https://www.vexforum.com/index.php/6146-robotc-programming-tips/p2#p53799
	+ https://renegaderobotics.org/what-is-slew-rate/
*/

/*
- Proportional gain constant
- Requested values for encoders
*/
static float  pid_Kp = .7;
static float  pidRequestedValueLB;
static float  pidRequestedValueRB;


/*
- Set default drive type
- Allow drive toggling with 5U + 7U
*/
static bool isTankDrive = false;
static bool allowToggle = true;

/*
When buttons 5U and 7U are pressed at the same time, the drive can shift from tank drive to arcade drive
*/
task toggleDrive(){
	int left, right;
	while(true){
		if (allowToggle && vexRT[Btn5U] == 1 && vexRT[Btn7U] == 1){
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
task encoderPIDController()
{
	//intialize error and drive power vars
	float pidErrorLeft;
	float pidErrorRight;
	float pidDriveLeft;
	float pidDriveRight;

	while(true){
		//error = desired value - current value
		pidErrorLeft = pidRequestedValueLB - SensorValue[leftEncoder];
		pidErrorRight = pidRequestedValueRB - SensorValue[rightEncoder];

		//drive power = proportional constant * error
		pidDriveLeft = (pid_Kp * pidErrorLeft);
		pidDriveRight = (pid_Kp * pidErrorRight);

		//limit drive power
		if( pidDriveLeft > 127 )
			pidDriveLeft = 127;
		if( pidDriveLeft < (-127) )
			pidDriveLeft = (-127);
		if( pidDriveRight > 127 )
			pidDriveRight = 127;
		if( pidDriveRight < (-127) )
			pidDriveRight = (-127);

		//run motors at calculated power
		motor[LB] = pidDriveLeft;
		motor[RB] = pidDriveRight;

		//don't hog CPU
		wait1Msec( 25 );
	}
}

/*
Pre-auton includes has an LCD menu that allows
*/

void lcdProgram(){
	bLCDBacklight = true;

	const short leftButton = 1;
	const short centerButton = 2;
	const short rightButton = 4;
	const short leftRightButtons = 6;
	bool select = false; //changed to true every time center button is pressed

	int maxSensorDisplays = 4;
	int maxVarsDisplays = 4;
	int maxAutonDisplays = 4;
	int maxDisplays = maxSensorDisplays;
	int currentDisplay = 0;


	int maxModes = 2;
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
			currentDisplay = currentDisplay % maxDisplays;
			wait1Msec(250);
		}
		if (nLCDButtons == rightButton){
			currentDisplay++;
			currentDisplay = currentDisplay % maxDisplays;
			wait1Msec(250);
		}
		if (nLCDButtons == centerButton){
			select = true;
		}
		if (nLCDButtons == leftRightButtons){
			currentDisplay = 0;
			currentMode++;
			currentMode = currentMode % maxModes;
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
		switch(currentMode){
			case SensorMode:
				switch(currentDisplay){
					case 0:
						displayLCDCenteredString(0, "Sensor 1");
						break;
					case 1:
						displayLCDCenteredString(0, "Sensor 2");
						break;
					case 2:
						displayLCDCenteredString(0, "Sensor 3");
						break;
					case 3:
						break;
					default:
						break;
				}
				break;
			case VarsMode:
				switch(currentDisplay){
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
					case 3:
						break;
					default:
						break;
				}
				break;
			case AutonMode:
				switch(currentDisplay){
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
					case 3:
						break;
					default:
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

task autonomous(){

}

task main(){
	startTask(encoderPIDController);
	startTask(toggleDrive);

	SensorValue[rightEncoder] = 0;
	SensorValue[leftEncoder] = 0;

	while(true){
		lcdProgram();
	}
}
