#pragma once

#include <chrono>

#include "FZN/Defines.h"


namespace fzn
{
	class FZN_EXPORT Chrono
	{
	public:
		Chrono();

		void start();
		void toggle_pause();
		void stop();

		bool has_started() const;
		bool is_paused() const;

		void update();
		const std::chrono::duration< int, std::milli >& get_time() { return m_chrono; }

	private:
		std::chrono::duration< int, std::milli > m_chrono;

		std::chrono::steady_clock::time_point m_previous_clock;
		bool m_paused{ true };
	};
};
