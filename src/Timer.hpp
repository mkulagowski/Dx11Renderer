#pragma once

#include <chrono>

template<typename RetT = double, class ResT = std::milli, typename ClockT = std::chrono::steady_clock>
class Timer
{
	//static_assert(std::chrono::is_clock<ClockT>::value, "ClockT must be a clock type from std::chrono!"); //< waiting for cpp20
	//static_assert(is_base_of_val_template_v<intmax_t, std::ratio, ResT>, "ResT must be an std::ratio type!");
	static_assert(std::is_arithmetic<RetT>::value, "RetT must be a numbr type!");

	using DurT = std::chrono::duration<RetT, ResT>;
private:
    std::chrono::time_point<ClockT> mStart; // start point

public:
	Timer() {};
	Timer(bool callStart) { if (callStart) Start(); };

	template<typename F, typename ...Args>
	static auto measureExecution(F&& func, Args&&... args)
	{
		static_assert(std::is_invocable<F, Args...>::value, "func parameter must be invocable!");

		auto tmr = Timer(true);
		std::invoke(std::forward<decltype(func)>(func), std::forward<Args>(args)...);
		return tmr.Stop();
	}

    void Start() {
		mStart = ClockT::now();
	}

	DurT Stop() {
		DurT res = ClockT::now() - mStart;
		Start();
		return res;
	}
};

