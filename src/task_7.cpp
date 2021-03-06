#include <iostream>
#include <fstream>
#include "ArgParseStandalone.h"
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <limits>
#include <algorithm>

class message {
	public:
		message(int year, int month, int day, int hour, int minute, const std::string& message_string) {
			this->year = year;
			this->month = month;
			this->day = day;
			this->hour = hour;
			this->minute = minute;
			this->message_string = message_string;
		}
		int year;
		int month;
		int day;
		int hour;
		int minute;
		std::string message_string;
		bool operator<(const message& rhs) const {
			if (this->year < rhs.year) {
				return true;
			} else if (this->year > rhs.year) {
				return false;
			}
			if (this->month < rhs.month) {
				return true;
			} else if (this->month > rhs.month) {
				return false;
			}
			if (this->day < rhs.day) {
				return true;
			} else if (this->day > rhs.day) {
				return false;
			}
			if (this->hour < rhs.hour) {
				return true;
			} else if (this->hour > rhs.hour) {
				return false;
			}
			if (this->minute < rhs.minute) {
				return true;
			} else if (this->minute > rhs.minute) {
				return false;
			}
			return false;
		}
		void print() const {
			std::cout << year << "-" << month << "-" << day << " " << hour << ":" << minute << " " << this->message_string << std::endl;
		}
};

class guard_sleep {
	public:
		guard_sleep(int guard_id, int sleep_start, int sleep_stop) {
			this->guard_id = guard_id;
			this->sleep_start = sleep_start;
			this->sleep_stop = sleep_stop;
		}
		int guard_id;
		int sleep_start;
		int sleep_stop;
		int length() const {
			return sleep_stop-sleep_start;
		}
		void print() const {
			std::cout << "#" << guard_id << " sleeps for " << sleep_stop-sleep_start << " minutes" << std::endl;
		}
};

bool message_comparison(const message& i, const message& j) {
	return i < j;
}

int main(int argc, char** argv) {
	// Parse Arguments
	std::string input_filepath;
	bool verbose = false;
	ArgParse::ArgParser Parser("Task 5");
	Parser.AddArgument("-i/--input", "File defining the input", &input_filepath);
	Parser.AddArgument("-v/--verbose", "Print Verbose output", &verbose);

	if (Parser.ParseArgs(argc, argv) < 0) {
		std::cerr << "Problem parsing arguments!" << std::endl;
		return -1;
	}

	if (Parser.HelpPrinted()) {
		return 0;
	}

	// Open input as stream
	std::ifstream infile(input_filepath);

	std::vector<message> messages;

	// Load messages
	std::string line;
	while(infile) {
		line = "";
		std::getline(infile, line);
		if(line.size() == 0) {
			break;
		}
		int year = std::atoi(line.substr(1,4).c_str());
		int month = std::atoi(line.substr(6,2).c_str());
		int day = std::atoi(line.substr(9,2).c_str());
		int hour = std::atoi(line.substr(12,2).c_str());
		int minute = std::atoi(line.substr(15,2).c_str());
		std::string message_string = line.substr(19);
		messages.push_back(message(year, month, day, hour, minute, message_string));
	}

	// Sort messages
	std::sort(messages.begin(), messages.end(), message_comparison);

	// Extract sleep intervals
	int state = 0; // 0 - waiting for first guard, 1 - need sleep stop, 2 - can have new guard or sleep start
	int guard_id = -1;
	int sleep_start = -1;
	int sleep_stop = -1;
	std::vector<guard_sleep> sleep_intervals;
	for(unsigned int idx=0; idx < messages.size(); ++idx) {
		message& the_message = messages[idx];
		if (verbose) {
			std::cout << the_message.message_string << std::endl;
		}
		if (the_message.message_string[0] == 'G') {
			// New guard
			if((state == 0)||(state == 2)) {
				// Change state
				state = 2;
				// Extract guard_id
				std::string id_portion = the_message.message_string.substr(7);
				std::stringstream ss;
				ss.str(id_portion);
				std::string id_string;
				ss >> id_string;
				guard_id = std::atoi(id_string.c_str());
			} else {
				std::cerr << "Unexpected message! (" << idx << ") (" << the_message.message_string << ")" << std::endl;
				return -1;
			}
		} else if (the_message.message_string[0] == 'f') {
			if (state == 2) {
				// Change state
				state = 1;
				sleep_start = the_message.minute;
			} else {
				std::cerr << "Unexpected message! (" << idx << ") (" << the_message.message_string << ")" << std::endl;
				return -1;
			}
		} else if (the_message.message_string[0] == 'w') {
			if (state == 1) {
				// Change state
				state = 2;
				sleep_stop = the_message.minute;
				sleep_intervals.push_back(guard_sleep(guard_id, sleep_start, sleep_stop));
				sleep_start = -1;
				sleep_stop = -1;
			} else {
				std::cerr << "Unexpected message! (" << idx << ") (" << the_message.message_string << ")" << std::endl;
				return -1;
			}
		} else {
			std::cerr << "Unknown message!" << std::endl;
			return -1;
		}
	}

	// Determine sleepiest guard
	std::map<int,int> guard_sleep;
	for (unsigned int idx=0; idx < sleep_intervals.size(); ++idx) {
		guard_sleep[sleep_intervals[idx].guard_id] += sleep_intervals[idx].length();
	}

	// Assume there won't be multiple guards with the same sleep..
	int max_sleep = -1;
	int max_guard_id = -1;
	for (auto guard_it = guard_sleep.begin(); guard_it != guard_sleep.end(); ++guard_it) {
		if (guard_it->second > max_sleep) {
			max_sleep = guard_it->second;
			max_guard_id = guard_it->first;
		}
	}

	std::cout << "Sleepiest guard is #" << max_guard_id << " they slept for " << max_sleep << " minutes" << std::endl;

	// Determine sleepiest minute of max guard.
	int days_slept_on_minute[60];
	for(int idx=0; idx < 60; ++idx) {
		days_slept_on_minute[idx] = 0;
	}

	for (auto interval_it = sleep_intervals.begin(); interval_it != sleep_intervals.end(); ++interval_it) {
		if(interval_it->guard_id == max_guard_id) {
			for(int min = interval_it->sleep_start; min < interval_it->sleep_stop; ++min) {
				days_slept_on_minute[min] += 1;
			}
		}
	}

	// Determine the minute slept during the most
	int max_min = -1;
	int max_days = -1;
	for (int min=0; min < 60; ++min) {
		if (days_slept_on_minute[min] > max_days) {
			max_days = days_slept_on_minute[min];
			max_min = min;
		}
	}

	std::cout << "Their sleepiest minute is: " << max_min << " and they slept " << max_days << std::endl;

	std::cout << "Answer is: " << max_guard_id*max_min << std::endl;

	return 0;
}
