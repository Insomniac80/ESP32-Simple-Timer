/**
 * 	Timer class.
 * 
 * 	Why?: Arduino code should not be delay()ed except there is a very 
 * 	(very!) good reason to do so.
 * 	The background is that for the delay time the entire system waits 
 * 	for the delay() to run out, which suspends the entire program flow. 
 * 
 *	Fortunately, the Arduino IDE offers the possibility to query the 
 * 	milliseconds that have passed since the last reboot. This can be 
 * 	used to determine exact intervals without affecting the rest of 
 * 	the system. 
 * 
 * 	This is exactly what happens here: a Reset() stores the current 
 * 	timestamp. Once the interval time is reached (or exceeded), 
 * 	IntervalReached() returns true, and again stores the current 
 * 	timestamp for the next interval.
 * 	IntervalReached() must of course be polled continuously to be 
 * 	informed promptly when the interval is reached or exceeded.
 * 	
 * There are a few things to keep in mind:
 * 		- There are two types of designs in timers: 
 *			one is to have measured the interval time exactly x times over 
 *			several intervals at the end. For example, if you measure 1000ms, 
 *			and retrieve the information whether the timer has expired (and 
 *			thus a new interval begins) after 1003ms, the next interval would 
 *			be only 997ms. Thus the intervals themselves would be subject to 
 *			fluctuations (depending on when the code comes to request the 
 *			status), but the total time would be almost exact.
 *			This timer is designed the other way around: each interval has 
 *			exactly the set time, no matter by how much the last one was 
 *			already overdrawn. So it is mainly designed to measure the 
 *			individual intervals very precisely. If, however, many small 
 *			measurements are needed which must have exactly the exact total 
 *			time at the end, a separate implementation may be the better way.
 *			Depending on the code, however, experience shows that the timer 
 *			only overruns by a very small single-digit number of milliseconds 
 *			at most, provided that the loop can run unblocked and the 
 *			controller's code is not excessively complex block by block.
 * 		- The "no own task-design" here means that the timer can safely  
 * 			be defined already without needing it yet. Apart from the memory 
 * 			requirement, no code runs automatically - the timer is only checked 
 * 			on IntervalReached()-call. If not called for it, a timer does not 
 * 			slow down the normal system flow in the background in any way.
 * 		- Deactivate() offers the possibility to get a false back from 
 * 			IntervalReached() all the time. Thus the call in the code does not 
 * 			have to be always enclosed by an if-case with a not continuously 
 * 			used timer.
 * 		- The static methods SecToMillis and MinToMillis allow conversion to 
 * 			milliseconds in a good readable way. But: the interval time is 
 * 			hold by a uint32_t, which means it is limited to ~49 days. This 
 * 			is a design decision, as usually many timers are in use, and almost 
 * 			never such a high interval time is needed.
 * 			This surely is only the case for the _interval_. The millis-timer
 * 			itself is a uint64_t, which takes ~584942417 years until an overflow.
 */

#pragma once

#include <Arduino.h>

using std::numeric_limits;

template <typename T>
uint32_t NarrowConvertToUint32(T value_)
{
   if (value_ < 0)
		return 0;
	else if (numeric_limits<T>::max() < numeric_limits<uint32_t>::max())
		return static_cast<uint32_t>(value_);
	else if (value_ > static_cast<T>(numeric_limits<uint32_t>::max()))
		return numeric_limits<uint32_t>::max();
	else
		return static_cast<uint32_t>(value_);
}

class Timer {

public:

	//	SetInterval()-parameter definition
	static constexpr auto TIMER_CONTINUE = false;
	//	SetInterval()-parameter definition
	static constexpr auto TIMER_RESET = true;

	/**
	 * @brief Creates an non-active timer with an interval of roughly 49 days.
	 * 
	 * Note: If timer interval is not clear at time of creation,
	 * 		this one is easy to create and later to be changed.
	 * 		SetInterval() will activate the timer on set (do not forget 
	 * 		setting the reset paramerter aswell).
	 */
	Timer();

	/**
	 * @brief Creates an active timer with a specific interval.
	 * 
	 * Note: To pass minutes or seconds, static methods are available.
	 * 		CHECK TEXT FOR MAX VALUES BEFORE USING THEM!
	 * 
	 * @param intervalInMillis_: The interval time of the timer in milliseconds.
	 */
	explicit Timer(uint32_t intervalInMillis_);


	/**
	 * @brief Converts seconds to milliseconds.
	 * 
	 * Note: DO NOT USE WITH VALUES ABOVE 4.294.967 SECONDS (roughly 49 days)!
	 * 		This is also the limit of this timer (because of memory reasons).
	 * 
	 * @param secondsToConvert_: Seconds to be converted to milliseconds.
	 */
	static uint32_t SecToMillis(float secondsToConvert_);

	/**
	 * @brief Converts minutes to milliseconds.
	 * 
	 * Note: DO NOT USE WITH VALUES ABOVE 71.582 MINUTES (roughly 49 days)!
	 * 		This is also the limit of this timer (because of memory reasons).
	 * 
	 * @param minutesToConvert_: Minutes to be converted to milliseconds.
	 */
	static uint32_t MinToMillis(float minutesToConvert_);


	/**
	 * @brief Activates the timer. Starts with full interval. 
	 * 		Resets OverrideIntervalReached().
	 */
	auto Activate() -> void;

	/**
	 * @brief Deactivates the timer (IntervalReached() returns always false).
	 * 		Resets OverrideIntervalReached().
	 */
	auto Deactivate() -> void;

	/**
	 * @brief Returns true if the timer is active.
	 */
	auto IsActive() const -> bool {return mActivated;}


	/**
	 * @brief Sets the interval to the passed time. If the timer is 
	 * 			inactive, will be activated again.
	 * 
	 * @param millisInterval_: The interval the timer has to be set to.
	 * @param resetTimer_: If true, interval timer will also reset. Use 
	 * 						TIMER_CONTINUE or TIMER_RESET as parameters.
	 */
	void SetInterval(uint32_t millisInterval_, bool resetTimer_);

	/**
	 * @brief Chechs if the set interval has been reached since last 
	 * 		positive call / Reset(). If so, returns true once and 
	 * 		begins with a new interval.
	 * IMPORTANT: The new interval begins from zero, no matter how much 
	 * 			time has passed since the last interval was reached.
	 */
	bool IntervalReached();

	/**
	 * @brief Interval reached will return true on next call (once).
	 * 		Does only have an effect if timer is active.
	 * 		Will be reset on Reset()s if not called for before.
	 */
	void OverrideIntervalReached();

	/**
	 * @brief Returns the actual set interval time (full interval time, 
	 * 			do not confuse with TimeLeftInMillis()).
	 */
	uint32_t GetInterval() const {return mMillisInterval;}

	/**
	 * @brief Resets the timer (interval begins from zero).
	 */
	void ResetInterval();


	/**
	 * @brief Returns the time left until the next interval is reached.
	 */
	uint32_t TimeLeftInMillis() const;

	/**
	 * @brief Returns the time passed since the last interval was reached (and called for).
	 */
	uint32_t TimePassedInMillis() const;
	

private:

	static constexpr uint16_t SEC_TO_MILLIS_MULTI = 1000;
	static constexpr uint16_t MIN_TO_MILLIS_MULTI = 60000;

	uint64_t mMillisStartPeriod{millis()};
	uint32_t mMillisInterval;

	bool mOverrideIntervalReached{false};

	bool mActivated{true};

};