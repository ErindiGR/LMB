#pragma once


#define DEBUG_LOG(msg, ... )\
printf(msg, ##__VA_ARGS__ );