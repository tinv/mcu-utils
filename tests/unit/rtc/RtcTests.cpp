/*
 * Copyright 2025 TecInvent Electronics Ltd
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <zephyr/device.h>
#include "rtc/rtc.h"
#include "rtc_mock.h"

extern RTCMock *RTCMockObj;

using namespace ::testing;

class MuRtcFixture: public ::testing::Test
{

      protected:
	MuRtcFixture()
	{
	}

	void SetUp() override
	{
		RTCMockObj = new NiceMock<RTCMock>();
		muRtc.init();
	}

	struct mu_rtc_dataTime mu_rtc_dt;

	void TearDown() override
	{
		delete RTCMockObj;
	}
	void setRTCdevice(void)
	{
		EXPECT_CALL(
			*RTCMockObj,
			rtc_set_time(
				_, Pointee(AllOf(
					   Field(&rtc_time::tm_year, Eq(mu_rtc_dt.year - 1900)),
					   Field(&rtc_time::tm_mon, Eq(mu_rtc_dt.month - 1)),
					   Field(&rtc_time::tm_mday, Eq(mu_rtc_dt.day)),
					   Field(&rtc_time::tm_hour, Eq(mu_rtc_dt.hour)),
					   Field(&rtc_time::tm_min, Eq(mu_rtc_dt.minute)),
					   Field(&rtc_time::tm_sec, Eq(mu_rtc_dt.second)),
					   Field(&rtc_time::tm_nsec, Eq(mu_rtc_dt.nanosecond))))));
	}
};

// compare struct mu_rtc_dataTime
bool operator==(const mu_rtc_dataTime &lhs, const mu_rtc_dataTime &rhs)
{
	return std::tie(lhs.year, lhs.month, lhs.day, lhs.hour, lhs.minute, lhs.second,
			lhs.nanosecond) == std::tie(rhs.year, rhs.month, rhs.day, rhs.hour,
						    rhs.minute, rhs.second, rhs.nanosecond);
}

TEST_F(MuRtcFixture, should_get_date_time_unix)
{
	struct rtc_time tm;
	/* Values got from: https://www.epochconverter.com */
	time_t expected = 1766071951;
	tm.tm_year = 2025 - 1900;
	tm.tm_mon = 12 - 1;
	tm.tm_mday = 18;
	tm.tm_hour = 15;
	tm.tm_min = 32;
	tm.tm_sec = 31;

	EXPECT_CALL(*RTCMockObj, rtc_get_time(_, _))
		.Times(1)
		.WillOnce(DoAll(SetArgPointee<1>(tm), Return(0)));

	time_t unixTime;
	int ret = muRtc.getDateTimeUnix(&unixTime);

	EXPECT_EQ(ret, 0);
	EXPECT_EQ(expected, unixTime);
}

TEST_F(MuRtcFixture, should_get_date_time)
{
	struct rtc_time tm;
	mu_rtc_dataTime expected_dt;

	expected_dt.year = 2026;
	expected_dt.month = 12;
	expected_dt.day = 30;
	expected_dt.hour = 23;
	expected_dt.minute = 55;
	expected_dt.second = 4;
	expected_dt.nanosecond = 777;

	tm.tm_year = expected_dt.year - 1900;
	tm.tm_mon = expected_dt.month - 1;
	tm.tm_mday = expected_dt.day;
	tm.tm_hour = expected_dt.hour;
	tm.tm_min = expected_dt.minute;
	tm.tm_sec = expected_dt.second;
	tm.tm_nsec = expected_dt.nanosecond;

	EXPECT_CALL(*RTCMockObj, rtc_get_time(_, _))
		.Times(1)
		.WillOnce(DoAll(SetArgPointee<1>(tm), Return(0)));

	int ret = muRtc.getDateTime(&mu_rtc_dt);

	EXPECT_EQ(ret, 0);
	EXPECT_EQ(expected_dt, mu_rtc_dt);
}

TEST_F(MuRtcFixture, should_set_date_time)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 12;
	mu_rtc_dt.day = 30;
	mu_rtc_dt.hour = 23;
	mu_rtc_dt.minute = 55;
	mu_rtc_dt.second = 4;
	mu_rtc_dt.nanosecond = 777;

	setRTCdevice();

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, 0);
}

TEST_F(MuRtcFixture, should_set_date_time_19JAN2038)
{
	/* January 19, 2038 3:14:07 */
	mu_rtc_dt.year = 2038;
	mu_rtc_dt.month = 1;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	setRTCdevice();

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, 0);
}

TEST_F(MuRtcFixture, should_fail_when_year_less_than_2026)
{
	mu_rtc_dt.year = 2025;
	mu_rtc_dt.month = 1;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_year_greater_than_2026)
{
	mu_rtc_dt.year = 10000;
	mu_rtc_dt.month = 1;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_month_less_than_1)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 0;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_month_greater_than_12)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 13;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_day_less_than_1)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 1;
	mu_rtc_dt.day = 0;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_day_greater_than_31)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 11;
	mu_rtc_dt.day = 32;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_hour_less_than_0)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 3;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = -1;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_hour_greater_than_23)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 12;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 24;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_minute_less_than_0)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 2;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = -1;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_minute_greater_than_59)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 11;
	mu_rtc_dt.day = 13;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 60;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_second_less_than_0)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 2;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = -1;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_second_greater_than_59)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 11;
	mu_rtc_dt.day = 13;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 60;
	mu_rtc_dt.nanosecond = 10;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_nanosecond_less_than_0)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 2;
	mu_rtc_dt.day = 19;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = -1;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}

TEST_F(MuRtcFixture, should_fail_when_nanosecond_greater_than_999999999)
{
	mu_rtc_dt.year = 2026;
	mu_rtc_dt.month = 11;
	mu_rtc_dt.day = 13;
	mu_rtc_dt.hour = 3;
	mu_rtc_dt.minute = 12;
	mu_rtc_dt.second = 7;
	mu_rtc_dt.nanosecond = 1000000000;

	int ret = muRtc.setDateTime(mu_rtc_dt);
	EXPECT_EQ(ret, -EINVAL);
}