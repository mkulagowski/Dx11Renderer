#pragma once

#ifdef _DEBUG

template<class ...Ts> void LOGI(const char* format, Ts... args) {
	std::string formatStr(format);
	formatStr = "[INFO] " + formatStr + "\n";
	printf(formatStr.c_str(), args...);
}

template<class ...Ts> void LOGW(const char* format, Ts... args) {
	std::string formatStr(format);
	formatStr = "[WARN] " + formatStr + "\n";
	printf(formatStr.c_str(), args...);
}

template<class ...Ts> void LOGE(const char* format, Ts... args) {
	std::string formatStr(format);
	formatStr = "[ERRR] " + formatStr + "\n";
	printf(formatStr.c_str(), args...);
}

#else

#define LOGI(...)
#define LOGE(...)
#define LOGW(...)

#endif // _DEBUG