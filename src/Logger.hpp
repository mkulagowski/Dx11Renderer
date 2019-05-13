#pragma once

#ifdef _DEBUG

template<class ...Ts> void LOG(const char* format, Ts... args) {
	printf(format, args...);
}

#define LOGG(type, fmt, ...) LOG(("[" + std::string(type) + "][" + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__) + "]  " + std::string(fmt) + "\n").c_str(), __VA_ARGS__)
#define LOGI(fmt, ...) LOGG("INFO", fmt, __VA_ARGS__)
#define LOGE(fmt, ...) LOGG("ERRR", fmt, __VA_ARGS__)
#define LOGW(fmt, ...) LOGG("WARN", fmt, __VA_ARGS__)

#else

#define LOGI(...)
#define LOGE(...)
#define LOGW(...)

#endif // _DEBUG