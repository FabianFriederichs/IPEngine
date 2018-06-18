/** \addtogroup runtime
*  @{
*/

/*!
\file Time.h
*/

#ifndef _TIME_H_
#define _TIME_H_
#include <cstdint>
#include <chrono>

namespace ipengine
{
	//! We store time in nanoseconds, therefore this typedef uses a 64 bit signed integer.
	using time_t = int64_t;
	/*!
	\brief This class provides some convenience function for handling time.

	Time is stored as an integer of nanoseconds. We use this kind of storage to prevent floating
	point precision errors.

	Some functions are defined to convert the nanoseconds value into more convenient units.

	\todo	Add arithmetic operators
	*/
	class Time
	{
	public:
		//! Initializes with 0 ns and timescale of 1.0
		Time() :
			m_nanoseconds(0),
			m_timescale(1.0f)
		{}

		//! Initializes with time ns and timescale of 1.0
		Time(time_t time) :
			m_nanoseconds(time),
			m_timescale(1.0f)
		{}

		//! Initializes with time ns and timescale of timeScale
		Time(time_t time, float timeScale) :
			m_nanoseconds(time),
			m_timescale(timeScale)
		{}

		//! Copy constructor
		Time(const Time& other) :
			m_nanoseconds(other.m_nanoseconds),
			m_timescale(other.m_timescale)
		{}

		//! Copy assignment
		Time& operator=(const Time& other)
		{
			if (this == &other)
				return *this;
			m_nanoseconds = other.m_nanoseconds;
			m_timescale = other.m_timescale;
			return *this;
		}

		//! Initializes with a value in seconds and the given time scale
		Time(double seconds, float timeScale) :
			m_nanoseconds(static_cast<time_t>(seconds * 1e9 + 0.5)),
			m_timescale(timeScale)
		{}

		//! Sets the time value to a value given in seconds
		Time& operator=(double seconds)
		{
			m_nanoseconds = static_cast<time_t>(seconds * 1e9 + 0.5);
			return *this;
		}

		//! Sets the time value to a value given in nanoseconds
		Time& operator=(time_t nanoseconds)
		{
			m_nanoseconds = nanoseconds;
			return *this;
		}

		//! Returns time in nanoseconds
		time_t nano() const
		{
			return m_nanoseconds;
		}

		//! Returns time in hours
		double hr() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) / 3600.0;
		}
		
		//! Returns time in minutes
		double min() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) / 60.0;
		}

		//! Returns time in seconds
		double sec() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-9;
		}

		//! Returns time in milliseconds
		double mil() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-6;
		}

		//! Returns time in microseconds
		double mic() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-3;
		}

		//! Returns time scaled by timescale in nanoseconds
		time_t nano_scaled() const
		{
			return static_cast<time_t>(static_cast<double>(m_nanoseconds) * static_cast<double>(m_timescale) + 0.5);
		}

		//! Returns time scaled by timescale in hours
		double hr_scaled() const
		{
			return ((static_cast<double>(m_nanoseconds) * 1e-9) / 3600.0) * m_timescale;
		}

		//! Returns time scaled by timescale in minutes
		double min_scaled() const
		{
			return ((static_cast<double>(m_nanoseconds) * 1e-9) / 60.0) * m_timescale;
		}

		//! Returns time scaled by timescale in seconds
		double sec_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) * m_timescale;
		}

		//! Returns time scaled by timescale in milliseconds
		double mil_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-6) * m_timescale;
		}

		//! Returns time scaled by timescale in microseconds
		double mic_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-3) * m_timescale;
		}

		//! Returns the current timscale
		float timescale() const
		{
			return m_timescale;
		}

		//! Sets the timescale
		void set_timescale(float timescale)
		{
			m_timescale = timescale;
		}

		//! Returns the current point in time via the systems high resolution clock if available.
		inline static Time now()
		{
			return Time(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
		}

	private:
		time_t m_nanoseconds;
		float m_timescale;
	};
}

#endif
/** @}*/