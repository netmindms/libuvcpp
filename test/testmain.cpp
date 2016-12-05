//
// Created by netmind on 16. 11. 28.
//

#include <gtest/gtest.h>

#include <uv.h>
#include "../uvcpp/uvcpplog.h"

int main(int argc, char* argv[]) {

	NMDU_SET_LOG_LEVEL(LOG_VERBOSE);
//	NMDU_SET_LOG_FILE("log.txt");
//	NMDU_SET_LOG_LEVEL_FILE(LOG_VERBOSE);
	::testing::InitGoogleTest(&argc, argv);
//	::testing::GTEST_FLAG(filter) = "basic.check";
	return RUN_ALL_TESTS();
}