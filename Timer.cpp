#include "Timer.hpp"

Timer::Timer() :
	Timer(numeric_limits<uint32_t>::max())
{
	Deactivate();
}

Timer::Timer(uint32_t intervalInMillis_) :
	mMillisInterval(intervalInMillis_)
{
}

bool Timer::IntervalReached()
{
	if (!mActivated)
		return false;

	if (mOverrideIntervalReached || (mMillisStartPeriod + mMillisInterval) < millis())
	{
		ResetInterval();
		return true;
	}
	
	return false;
}

uint32_t Timer::TimeLeftInMillis() const
{
	return NarrowConvertToUint32(mMillisStartPeriod + static_cast<int64_t>(mMillisInterval) - millis());
}
 
uint32_t Timer::TimePassedInMillis() const
{
	return NarrowConvertToUint32(static_cast<int64_t>(millis()) - mMillisStartPeriod);
}

auto Timer::Activate() -> void 
{
	ResetInterval();
	mActivated = true;
}

auto Timer::Deactivate() -> void 
{
	ResetInterval();	// ensure override is cleared
	mActivated = false;
}

uint32_t Timer::SecToMillis(float secondsToConvert_) 
{
	return NarrowConvertToUint32(static_cast<double>(secondsToConvert_) * SEC_TO_MILLIS_MULTI);
}

uint32_t Timer::MinToMillis(float minutesToConvert_) 
{
	return NarrowConvertToUint32(static_cast<double>(minutesToConvert_) * MIN_TO_MILLIS_MULTI);
}

void Timer::SetInterval(uint32_t millisInterval_, bool resetTimer_) 
{
	mMillisInterval = millisInterval_;
	if (TIMER_RESET == resetTimer_)
		ResetInterval();
	mActivated = true;
}

void Timer::ResetInterval() 
{
	mOverrideIntervalReached = false;
	mMillisStartPeriod = millis();
}

void Timer::OverrideIntervalReached() 
{
	mOverrideIntervalReached = true;
}
