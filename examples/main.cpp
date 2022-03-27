#include <Arduino.h>

#include <Timer.hpp>

// 	Create a timer. 

//	If the required time interval is not yet clear, use the first example. 
//	The timer is inactive by default.
Timer sTimerExample;
//	The second one creates one with an interval time of five seconds.
//	The timer is active by default and starts immediately.
Timer sTimerExampleWithTime(Timer::SecToMillis(5));

void setup() 
{
	//	Set the time for a timer at any time within the code (f.e. here).
	//	The second parameter decides if the timer begins at zero, which ist mostly the case.
	sTimerExample.SetInterval(Timer::MinToMillis(2), Timer::TIMER_RESET);
}

void loop() 
{
	if (sTimerExample.IntervalReached())
	{	//	The timer itself does not run in an own task. It is
		//	only checked by IntervalReached(). This timer was set to two minutes,
		//	so after at least this time since setting the interval, it will return 
		//	true once, and then again after two, four.. and so on minutes.

	}

	//	There are several methods provided which could be helpful. All were annotated 
	//	accordingly and should show the appropriate meaning.
	//	Some in short:

	Timer::SecToMillis(123);
	//	Static method. As seen above, converts seconds to milliseconds for convenient usage.
	//	IMPORTANT: Timer uses uint32_t for storing intervals, which´s maximum is ~49 days!
	//	See method comment for max in seconds.

	Timer::MinToMillis(123);
	//	Static method. As seen above, converts minutes to milliseconds for convenient usage.
	//	IMPORTANT: Timer uses uint32_t for storing intervals, which´s maximum is ~49 days!
	//	See method comment for max in minutes.

	sTimerExample.ResetInterval();
	//	Timer starts from 0: next true of IntervalReached() will occur after full interval time.

	sTimerExample.Deactivate();	
	// 	IntervalReached() now returns always false.

	sTimerExample.Activate();	
	// 	IntervalReached() starts over (internally: ResetInterval()) and now returns again true in intervals.

	sTimerExample.OverrideIntervalReached();	
	// 	Next IntervalReached() retruns true on next call no matter the interval is reached. Then 
	//	starts over (internally: ResetInterval()).

	// 	For methods getting the time passed, set interval and other things see hpp-file.
}