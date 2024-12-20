#pragma once

enum LogLevel {
	LogDbg = 0,
	LogInf,
	LogWrn,
	LogErr,
};

#define LOGD(format, ...)                                    \
	{                                                         \
		if (opt.log_level <= LogDbg) {                         \
			fprintf(stdout, "[D] " format "\n", ##__VA_ARGS__); \
		}                                                      \
	}

#define LOGI(format, ...)                                    \
	{                                                         \
		if (opt.log_level <= LogInf) {                         \
			fprintf(stdout, "[I] " format "\n", ##__VA_ARGS__); \
		}                                                      \
	}

#define LOGW(format, ...)                                    \
	{                                                         \
		if (opt.log_level <= LogWrn) {                         \
			fprintf(stderr, "[W] " format "\n", ##__VA_ARGS__); \
		}                                                      \
	}

#define LOGE(format, ...)                                    \
	{                                                         \
		if (opt.log_level <= LogErr) {                         \
			fprintf(stderr, "[E] " format "\n", ##__VA_ARGS__); \
		}                                                      \
	}
