#ifndef _TIME_H_
#define _TIME_H_
#include <cstdint>

namespace ipengine
{
	using time_t = int64_t;

	class Time
	{
	public:
		Time() :
			m_nanoseconds(0),
			m_timescale(1.0f)
		{}

		Time(time_t time) :
			m_nanoseconds(time),
			m_timescale(1.0f)
		{}

		Time(time_t time, float timeScale) :
			m_nanoseconds(time),
			m_timescale(timeScale)
		{}

		Time(const Time& other) :
			m_nanoseconds(other.m_nanoseconds),
			m_timescale(other.m_timescale)
		{}

		Time& operator=(const Time& other)
		{
			if (this == &other)
				return *this;
			m_nanoseconds = other.m_nanoseconds;
			m_timescale = other.m_timescale;
			return *this;
		}

		Time(double seconds, float timeScale) :
			m_nanoseconds(static_cast<time_t>(seconds * 1e9 + 0.5)),
			m_timescale(timeScale)
		{}

		Time& operator=(double seconds)
		{
			m_nanoseconds = static_cast<time_t>(seconds * 1e9 + 0.5);
			return *this;
		}

		Time& operator=(time_t nanoseconds)
		{
			m_nanoseconds = nanoseconds;
			return *this;
		}

		time_t nano() const
		{
			return m_nanoseconds;
		}

		double hr() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) / 3600.0;
		}

		double min() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) / 60.0;
		}

		double sec() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-9;
		}

		double mil() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-6;
		}

		double mic() const
		{
			return static_cast<double>(m_nanoseconds) * 1e-3;
		}

		time_t nano_scaled() const
		{
			return static_cast<time_t>(static_cast<double>(m_nanoseconds) * static_cast<double>(m_timescale) + 0.5);
		}

		double hr_scaled() const
		{
			return ((static_cast<double>(m_nanoseconds) * 1e-9) / 3600.0) * m_timescale;
		}

		double min_scaled() const
		{
			return ((static_cast<double>(m_nanoseconds) * 1e-9) / 60.0) * m_timescale;
		}

		double sec_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-9) * m_timescale;
		}

		double mil_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-6) * m_timescale;
		}

		double mic_scaled() const
		{
			return (static_cast<double>(m_nanoseconds) * 1e-3) * m_timescale;
		}

		float timescale() const
		{
			return m_timescale;
		}

		void set_timescale(float timescale)
		{
			m_timescale = timescale;
		}

	private:
		time_t m_nanoseconds;
		float m_timescale;
	};





}

#endif // !_TIME_H_
