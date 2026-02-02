#include "Chrono.h"

namespace fzn
{
	Chrono::Chrono()
	{
		m_paused = true;
		m_previous_clock = std::chrono::steady_clock::now();
		m_chrono = std::chrono::milliseconds( 0 );
	}

	void Chrono::start()
	{
		m_paused = false;
		m_chrono = std::chrono::milliseconds( 0 );
		m_previous_clock = std::chrono::steady_clock::now();
	}

	void Chrono::toggle_pause()
	{
		m_paused = !m_paused;

		if( m_paused == false )
			m_previous_clock = std::chrono::steady_clock::now();
	}

	void Chrono::stop()
	{
		m_paused = true;
		m_chrono = std::chrono::milliseconds( 0 );
	}

	bool Chrono::has_started() const
	{
		return m_chrono > std::chrono::milliseconds( 0 );
	}

	bool Chrono::is_paused() const
	{
		return m_paused;
	}

	void Chrono::update()
	{
		if( m_paused )
			return;

		const std::chrono::steady_clock::time_point current_clock = std::chrono::steady_clock::now();

		Milli delta = std::chrono::duration_cast<std::chrono::milliseconds>( current_clock - m_previous_clock );

		m_chrono += delta;

		m_previous_clock = std::chrono::steady_clock::now();
	}

}
