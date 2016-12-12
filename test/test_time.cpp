//
// Created by netmind on 16. 12. 12.
//

#include <chrono>
#include <ctime>
#include <gtest/gtest.h>
#include "tlog.h"

using namespace std;
using namespace uvcpp;
using namespace chrono;

#if 0
TEST(basic, time) {
//	auto m = duration<int>::min();
//	auto n = system_clock::time_point::min();
//	time_point<system_clock, duration<int>> tpsec(duration<int>::min()));
	auto nt = system_clock::now();
	auto at = nt + duration<uint64_t, std::milli>(2);
	auto c = nt - system_clock::time_point::min();
	auto d = at - system_clock::time_point::min();
	ald("time diff = %lu", duration_cast<chrono::milliseconds>(d-c).count());

}
#endif