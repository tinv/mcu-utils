
#include "CustomGtestMacros.h"
#include "codec/cobs.h"
#include <string.h>

class CobsFixture : public ::testing::Test
{
protected:

	void SetUp()
	{
		resetBuffer();
	}

	void resetBuffer()
	{
		memset(mBuffer, 0x00, sizeof(mBuffer));
	}

	void fillBuffer(uint8_t* buffer, size_t len, uint8_t zero_val_interval)
	{
		int val = 0;

		for (size_t i = 0; i < len; i++)
		{
			if (++val == 0xff) {
				val = 0x01;
			}

			if ((i % zero_val_interval) == 0)
			{
				val = 0x00;
			}

			buffer[i] = val;
		}
	}


	uint8_t mBuffer[1024];

};

TEST_F( CobsFixture, encode_00 )
{
	uint8_t data[] = { 0x00 };
	const uint8_t expected[] = { 0x01, 0x01, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(2, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, encode_00_00 )
{
	uint8_t data[] = { 0x00, 0x00 };
	const uint8_t expected[] = { 0x01, 0x01, 0x01, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(3, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, encode_00_11_00 )
{
	uint8_t data[] = { 0x00, 0x11, 0x00 };
	const uint8_t expected[] = { 0x01, 0x02, 0x11, 0x01, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(4, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, encode_11_22_00_33 )
{
	uint8_t data[] = { 0x11, 0x22, 0x00, 0x33 };
	const uint8_t expected[] = { 0x03, 0x11, 0x22, 0x02, 0x33, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, encode_11_22_33_44 )
{
	uint8_t data[] = { 0x11, 0x22, 0x33, 0x44 };
	const uint8_t expected[] = { 0x05, 0x11, 0x22, 0x33, 0x44, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, encode_11_00_00_00 )
{
	uint8_t data[] = { 0x11, 0x00, 0x00, 0x00 };
	const uint8_t expected[] = { 0x02, 0x11, 0x01, 0x01, 0x01, 0x00 };

	size_t size = muCobs.encode(data, sizeof(data), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

//******************************************************************************

TEST_F( CobsFixture, decode_01_01_00 )
{
	uint8_t encoded[] = { 0x01, 0x01, 0x00 };
	const uint8_t expected[] = { 0x00 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(2, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, decode_01_01_01_00 )
{
	uint8_t encoded[] = { 0x01, 0x01, 0x01, 0x00 };
	const uint8_t expected[] = { 0x00, 0x00 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(3, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, decode_01_02_11_01_00 )
{
	uint8_t encoded[] = { 0x01, 0x02, 0x11, 0x01, 0x00 };
	const uint8_t expected[] = { 0x00, 0x11, 0x00 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(4, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, decode_03_11_22_02_33_00 )
{
	uint8_t encoded[] = { 0x03, 0x11, 0x22, 0x02, 0x33, 0x00 };
	const uint8_t expected[] = { 0x11, 0x22, 0x00, 0x33 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, decode_05_11_22_33_44_00 )
{
	uint8_t encoded[] = { 0x05, 0x11, 0x22, 0x33, 0x44, 0x00 };
	const uint8_t expected[] = { 0x11, 0x22, 0x33, 0x44 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

TEST_F( CobsFixture, decode_02_11_01_01_01_00 )
{
	uint8_t encoded[] = { 0x02, 0x11, 0x01, 0x01, 0x01, 0x00 };
	const uint8_t expected[] = { 0x11, 0x00, 0x00, 0x00 };

	size_t size = muCobs.decode(encoded, sizeof(encoded), mBuffer);

	EXPECT_EQ(5, size);
	EXPECT_ARRAY_EQ(uint8_t, mBuffer, expected, sizeof(expected));
}

//******************************************************************************

TEST_F( CobsFixture, encode_decode_512bytes_with_0_val_every_10bytes )
{
	uint8_t srcData[512] = { 0x00 };
	uint8_t dstData[512] = { 0x00 };
	fillBuffer(srcData, sizeof(srcData), 10);

	size_t sizeEnc = muCobs.encode(srcData, sizeof(srcData), mBuffer);
        muCobs.decode(mBuffer, sizeEnc, dstData);

	EXPECT_ARRAY_EQ(uint8_t, srcData, dstData, sizeof(sizeEnc));
}

TEST_F( CobsFixture, encode_multiple_decode_calls_512bytes_with_0_val_every_15bytes )
{
	uint8_t srcData[512] = { 0x00 };
	uint8_t dstData[512] = { 0x00 };
	fillBuffer(srcData, sizeof(srcData), 15);

	size_t sizeEnc = muCobs.encode(srcData, sizeof(srcData), mBuffer);
	size_t sizeDec = muCobs.decode(mBuffer, sizeEnc - 10, dstData);
        muCobs.decode(mBuffer + sizeEnc - 10, 10, dstData + sizeDec);

	EXPECT_ARRAY_EQ(uint8_t, srcData, dstData, sizeof(sizeEnc));
}
