#pragma once

#ifdef DEBUG
#	define DEBUG_LOG(msg, ... )\
		printf(msg, ##__VA_ARGS__ );
#else
#	define DEBUG_LOG(msg, ... )
#endif