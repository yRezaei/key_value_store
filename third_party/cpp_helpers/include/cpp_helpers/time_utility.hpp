#pragma once
#ifndef CORE_TIME_UTILITY_H
#define CORE_TIME_UTILITY_H

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <iomanip>

using Clock = std::chrono::system_clock;

union TimeStamp
{
	std::uint64_t value;
	struct
	{
		std::uint64_t year			: 11;	// max number 2048
		std::uint64_t day			: 5;	// max number 32
		std::uint64_t month			: 4;	// max number 16
		std::uint64_t hour			: 5;	// max number 32
		std::uint64_t minute		: 6;	// max number 64
		std::uint64_t second		: 6;	// max number 64
		std::uint64_t millisecond	: 10;	// max number 1024
										//17 bits still unused
	};

	TimeStamp() : value(0)
	{
		auto current_time = Clock::now();
		auto epoch = current_time.time_since_epoch();
		epoch -= std::chrono::duration_cast<std::chrono::seconds>(epoch);
		auto ctime = Clock::to_time_t(current_time);
		auto local_time = std::localtime(&ctime);

		year = local_time->tm_year + 1900;
		month = local_time->tm_mon + 1;
		day = local_time->tm_mday;
		hour = local_time->tm_hour;
		minute = local_time->tm_min;
		second = local_time->tm_sec;
		millisecond = static_cast<unsigned>(epoch / std::chrono::milliseconds(1));
	}

	TimeStamp(std::uint64_t time_value) : 
		value(time_value) { }

	friend std::ostream& operator << (std::ostream& stream, const TimeStamp& stamp)
	{
		stream << stamp.year << "-"
			<< std::setfill('0') << std::setw(2) << stamp.month			<< "-"
			<< std::setfill('0') << std::setw(2) << stamp.day			<< " "
			<< std::setfill('0') << std::setw(2) << stamp.hour			<< ":"
			<< std::setfill('0') << std::setw(2) << stamp.minute		<< ":"
			<< std::setfill('0') << std::setw(2) << stamp.second		<< "."
			<< std::setfill('0') << std::setw(3) << stamp.millisecond;
		return stream;
	}

	std::string to_str() const
	{
		std::stringstream stream;
		stream << year << "-"
			<< std::setfill('0') << std::setw(2) << month		<< "-"
			<< std::setfill('0') << std::setw(2) << day			<< " "
			<< std::setfill('0') << std::setw(2) << hour		<< ":" 
			<< std::setfill('0') << std::setw(2) << minute		<< ":" 
			<< std::setfill('0') << std::setw(2) << second		<< "." 
			<< std::setfill('0') << std::setw(3) << millisecond;
		return stream.str();
	}
};

#endif // !CORE_TIME_UTILITY_H

