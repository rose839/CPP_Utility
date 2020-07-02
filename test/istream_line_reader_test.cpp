#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <gmock/gmock.h>
#include "../src/istream_line_reader.h"

TEST(IstreamLineReader, CheckWhenNoIstream) {

	try {
		for (const std::string& line: IstreamLineReader()) {
			ASSERT_TRUE(false);
		}
	} catch (const std::exception& e) {
		ASSERT_TRUE(true);
	}
}

TEST(IstreamLineReader, CheckWhenBeStringstream) {
	std::stringstream ss;

	ss << "hello" << std::endl;
	ss << "world" << std::endl;

	try {
		for (const std::string& line: IstreamLineReader(ss)) {
			if (line != "hello" && line != "world") {
				ASSERT_TRUE(false);
			}
		}
	} catch (const std::exception& e) {
		ASSERT_TRUE(false);
	}
}

TEST(IstreamLineReader, CheckWhenBeIfstream) {
	std::ifstream fs(__FILE__);
	std::ifstream tmp(__FILE__);
	std::string line_tmp;

	try {
		for (const std::string& line : IstreamLineReader(fs)) {
			getline(tmp, line_tmp);
			if (line != line_tmp) {
				ASSERT_TRUE(false);
			}		
		}
	} catch (const std::exception& e) {
		fs.close();
		tmp.close();
		ASSERT_TRUE(false);
	}
	
	fs.close();
	tmp.close();
}
