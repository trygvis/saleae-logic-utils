#include "common.h"

#define BOOST_TEST_MODULE EventTimerTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace Logic;

//int test_main(int, char* []) {
BOOST_AUTO_TEST_CASE( my_test )
{
    char buf[1000];
    const SampleRate* sampleRate = SampleRate::sampleRate100kHz;

    {
        EventTime eventTime = EventTime(0);
        BOOST_REQUIRE(eventTime.value == 0);
        string s = string(eventTime.to_str(buf, sizeof(buf)));
        BOOST_CHECK_EQUAL(string("0.00s"), s);
    }

    {
        EventTime eventTime = EventTime(sampleRate->interval);
        BOOST_CHECK_EQUAL(string("10.00us"), string(eventTime.to_str(buf, sizeof(buf))));
    }
}
