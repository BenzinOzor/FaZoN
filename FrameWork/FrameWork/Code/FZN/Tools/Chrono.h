#pragma once

#include <chrono>

#include "FZN/Defines.h"


namespace fzn
{
	class FZN_EXPORT Chrono
	{
	public:
		using Milli = std::chrono::duration< int, std::milli >;

		Chrono();

		void start();
		void toggle_pause();
		void stop();

		bool has_started() const;
		bool is_paused() const;

		void update();
		const Milli& get_time() { return m_chrono; }

	private:
		Milli m_chrono;

		std::chrono::steady_clock::time_point m_previous_clock;
		bool m_paused{ true };
	};
};
