// Cadmus Robotics 310
// Testing Program

// Autogenerated code by ROBOTC config wizard
#pragma config(Sensor, dgtl1,  LBEncoder,      sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  BREncoder,      sensorQuadEncoder)
#pragma config(Motor,  port4,           LB,            tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           RB,            tmotorVex393_MC29, openLoop)

task main()
{
	while (true) {
		if( vexRT[Btn7U] == 1) {
			motor[RB] = 64;
		} else {
			motor[RB] = 0;
		}
	}
}
