#include <gtest/gtest.h>
#include "CodePoint.hpp"

using brilliant::utf8::CodePoint;

constexpr std::uint32_t singleByte = 0x61; //ascii 'a'
constexpr std::uint32_t twoByte = 0xd098u;
constexpr std::uint32_t threeByte = 0xe0a4b9;
constexpr std::uint32_t fourByte = 0xf0908d88;


TEST(CodePointTest, testAsUint)
{
    EXPECT_EQ(CodePoint(singleByte, 1).decode(), singleByte);
    EXPECT_EQ(CodePoint(twoByte, 2).decode(), twoByte);
    EXPECT_EQ(CodePoint(threeByte, 3).decode(), threeByte);
    EXPECT_EQ(CodePoint(fourByte, 4).decode(), fourByte);
}

TEST(CodePointTest, testAsCodePointUint)
{
    static_assert(CodePoint(singleByte, 1).encode() == 0x61u);

    EXPECT_EQ(CodePoint(singleByte, 1).encode(), 0x61u);
    EXPECT_EQ(CodePoint(twoByte, 2).encode(), 0x418u);
    EXPECT_EQ(CodePoint(threeByte, 3).encode(), 0x939u);
    EXPECT_EQ(CodePoint(fourByte, 4).encode(), 0x10348);
}