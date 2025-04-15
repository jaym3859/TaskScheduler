#include "Logger.h"

std::shared_ptr<Log> Logger::logger_ = nullptr;

std::shared_ptr<Log> Logger::Get() {
	if (!logger_)
		logger_ = std::make_shared<Log>();
	return logger_;
}
