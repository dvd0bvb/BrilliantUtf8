#include <fstream>
#include <ranges>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Decode.hpp"
#include "quickbrown_expectedout.hpp"

using namespace brilliant::utf8;

// lists of first bytes of a code point
constexpr auto VALID_ONE_BYTE = std::views::iota(ONE_BYTE_PREFIX, ONE_BYTE_MASK);
constexpr auto VALID_TWO_BYTE = std::views::iota(TWO_BYTE_PREFIX, TWO_BYTE_MASK);
constexpr auto VALID_THREE_BYTE = std::views::iota(THREE_BYTE_PREFIX, THREE_BYTE_MASK);
constexpr auto VALID_FOUR_BYTE = std::views::iota(FOUR_BYTE_PREFIX, FOUR_BYTE_MASK);
constexpr auto VALID_CONTINUE = std::views::iota(CONTINUATION_PREFIX, CONTINUATION_MASK);
constexpr auto INVALID_ALWAYS = std::views::iota(FOUR_BYTE_MASK, u8'\xff');

void checkRange(auto range, char8_t mask, char8_t prefix, bool expectedValue)
{
    for (auto c : range)
    {
        EXPECT_EQ(checkCodeUnitMask(c, mask, prefix), expectedValue);
    }
}

TEST(DecodeTest, testCheckCodeUnitMask)
{
    checkRange(VALID_ONE_BYTE, ONE_BYTE_MASK, ONE_BYTE_PREFIX, true);
    checkRange(VALID_TWO_BYTE, ONE_BYTE_MASK, ONE_BYTE_PREFIX, false);
    checkRange(VALID_THREE_BYTE, ONE_BYTE_MASK, ONE_BYTE_PREFIX, false);
    checkRange(VALID_FOUR_BYTE, ONE_BYTE_MASK, ONE_BYTE_PREFIX, false);
    checkRange(VALID_CONTINUE, ONE_BYTE_MASK, ONE_BYTE_PREFIX, false);
    checkRange(INVALID_ALWAYS, ONE_BYTE_MASK, ONE_BYTE_PREFIX, false);

    checkRange(VALID_ONE_BYTE, TWO_BYTE_MASK, TWO_BYTE_PREFIX, false);
    checkRange(VALID_TWO_BYTE, TWO_BYTE_MASK, TWO_BYTE_PREFIX, true);
    checkRange(VALID_THREE_BYTE, TWO_BYTE_MASK, TWO_BYTE_PREFIX, false);
    checkRange(VALID_FOUR_BYTE, TWO_BYTE_MASK, TWO_BYTE_PREFIX, false);
    checkRange(VALID_CONTINUE, TWO_BYTE_MASK, TWO_BYTE_PREFIX, false);
    checkRange(INVALID_ALWAYS, TWO_BYTE_MASK, TWO_BYTE_PREFIX, false);

    checkRange(VALID_ONE_BYTE, THREE_BYTE_MASK, THREE_BYTE_PREFIX, false);
    checkRange(VALID_TWO_BYTE, THREE_BYTE_MASK, THREE_BYTE_PREFIX, false);
    checkRange(VALID_THREE_BYTE, THREE_BYTE_MASK, THREE_BYTE_PREFIX, true);
    checkRange(VALID_FOUR_BYTE, THREE_BYTE_MASK, THREE_BYTE_PREFIX, false);
    checkRange(VALID_CONTINUE, THREE_BYTE_MASK, THREE_BYTE_PREFIX, false);
    checkRange(INVALID_ALWAYS, THREE_BYTE_MASK, THREE_BYTE_PREFIX, false);

    checkRange(VALID_ONE_BYTE, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, false);
    checkRange(VALID_TWO_BYTE, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, false);
    checkRange(VALID_THREE_BYTE, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, false);
    checkRange(VALID_FOUR_BYTE, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, true);
    checkRange(VALID_CONTINUE, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, false);
    checkRange(INVALID_ALWAYS, FOUR_BYTE_MASK, FOUR_BYTE_PREFIX, false);

    checkRange(VALID_ONE_BYTE, CONTINUATION_MASK, CONTINUATION_PREFIX, false);
    checkRange(VALID_TWO_BYTE, CONTINUATION_MASK, CONTINUATION_PREFIX, false);
    checkRange(VALID_THREE_BYTE, CONTINUATION_MASK, CONTINUATION_PREFIX, false);
    checkRange(VALID_FOUR_BYTE, CONTINUATION_MASK, CONTINUATION_PREFIX, false);
    checkRange(VALID_CONTINUE, CONTINUATION_MASK, CONTINUATION_PREFIX, true);
    checkRange(INVALID_ALWAYS, CONTINUATION_MASK, CONTINUATION_PREFIX, false);
}

TEST(DecodeTest, testGetCodePointSize)
{
    for (auto c : VALID_ONE_BYTE)
    {
        EXPECT_EQ(getCodePointSize(c).value(), 0u);
    }

    for (auto c : VALID_TWO_BYTE)
    {
        EXPECT_EQ(getCodePointSize(c).value(), 1u);
    }

    for (auto c : VALID_THREE_BYTE)
    {
        EXPECT_EQ(getCodePointSize(c).value(), 2u);
    }

    for (auto c : VALID_FOUR_BYTE)
    {
        EXPECT_EQ(getCodePointSize(c).value(), 3u);
    }

    for (auto c : INVALID_ALWAYS)
    {
        EXPECT_EQ(getCodePointSize(c).error(), DecodeError::INVALID_CODE_POINT_LEN);
    }
}

TEST(DecodeTest, testDecodeCodePoint)
{
    constexpr auto s1 = u8"\x00\x7f";
    constexpr auto s2 = u8"\xc2\x80\xdf\xbf";
    constexpr auto s3 = u8"\xe0\xa0\x80\xef\xbf\xbf";
    constexpr auto s4 = u8"\xf0\x90\x80\x80\xf4\x8f\xbf\xbf";

    static_assert(decodeCodePoint(u8'\xff').error() == DecodeError::INVALID_CODE_POINT_LEN);
    static_assert(decodeCodePoint(s1, s1 + 1).value().first == 0x00);

    EXPECT_EQ(decodeCodePoint(u8'\x00').value(), 0u);
    EXPECT_EQ(decodeCodePoint(u8'\x7f').value(), 0x7f);
    EXPECT_EQ(decodeCodePoint(s1, s1 + 1).value().first, 0u);
    EXPECT_EQ(decodeCodePoint(s1 + 1, s1 + 2).value().first, 0x7fu);
    EXPECT_EQ(decodeCodePoint(s2, s1 + 2).value().first, 0x80u);
    EXPECT_EQ(decodeCodePoint(s2 + 2, s2 + 4).value().first, 0x7ffu);
    EXPECT_EQ(decodeCodePoint(s3, s3 + 3).value().first, 0x800u);
    EXPECT_EQ(decodeCodePoint(s3 + 3, s3 + 6).value().first, 0xffffu);
    EXPECT_EQ(decodeCodePoint(s4, s4 + 4).value().first, 0x10000u);
    EXPECT_EQ(decodeCodePoint(s4 + 4, s4 + 8).value().first, 0x10ffffu);

    // overlong sequences
    constexpr auto s5 = u8"\xc0\x80\xc1\xbf";
    constexpr auto s6 = u8"\xe0\x80\x80\xe0\x9f\xbf";
    constexpr auto s7 = u8"\xf0\x80\x80\x80\xf0\x8f\xbf\xbf";

    EXPECT_EQ(decodeCodePoint(s5, s5 + 2).error(), DecodeError::OVERLONG_SEQUENCE);
    EXPECT_EQ(decodeCodePoint(s5 + 2, s5 + 4).error(), DecodeError::OVERLONG_SEQUENCE);
    EXPECT_EQ(decodeCodePoint(s6, s6 + 3).error(), DecodeError::OVERLONG_SEQUENCE);
    EXPECT_EQ(decodeCodePoint(s6 + 3, s6 + 6).error(), DecodeError::OVERLONG_SEQUENCE);
    EXPECT_EQ(decodeCodePoint(s7, s7 + 4).error(), DecodeError::OVERLONG_SEQUENCE);
    EXPECT_EQ(decodeCodePoint(s7 + 4, s7 + 8).error(), DecodeError::OVERLONG_SEQUENCE);

    // invalid code points
    constexpr auto s8 = u8"\xf4\x9f\xbf\xbf\xf4\xaf\xbf\xbf\xf4\xbf\xbf\xbf\xf5\x8f\xbf\xbf\xf7\xbf\xbf\xbf";

    EXPECT_EQ(decodeCodePoint(s8, s8 + 4).error(), DecodeError::INVALID_CODE_POINT);
    EXPECT_EQ(decodeCodePoint(s8 + 4, s8 + 8).error(), DecodeError::INVALID_CODE_POINT);
    EXPECT_EQ(decodeCodePoint(s8 + 8, s8 + 12).error(), DecodeError::INVALID_CODE_POINT);
    EXPECT_EQ(decodeCodePoint(s8 + 12, s8 + 16).error(), DecodeError::INVALID_CODE_POINT);
    EXPECT_EQ(decodeCodePoint(s8 + 16, s8 + 20).error(), DecodeError::INVALID_CODE_POINT);

    // unexpected end
    EXPECT_EQ(decodeCodePoint(s7, s7 + 2).error(), DecodeError::UNEXPECTED_SEQUENCE_END);

    // expected continuation
    constexpr auto s9 = u8"\xf0\xc3\x80\x80";

    EXPECT_EQ(decodeCodePoint(s9, s9 + 5).error(), DecodeError::EXPECTED_CONTINUATION_PREFIX);
}

#include <format>

TEST(DecodeTest, testDecode)
{
    std::ifstream file("quickbrown.txt", std::ios::binary | std::ios::in);
    ASSERT_TRUE(file);

    std::vector<std::uint32_t> unicode;
    auto result = decode(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(unicode));

    EXPECT_EQ(result.error, DecodeError::OK);
    EXPECT_EQ(result.resultIter, std::istreambuf_iterator<char>());
    EXPECT_EQ(unicode.size(), QUICKBROWN_EXPECTED.size());

    EXPECT_THAT(unicode, testing::ElementsAreArray(QUICKBROWN_EXPECTED)) 
        << std::format("{}, {}, {}", unicode[269], unicode[270], unicode[271]);
}

TEST(DecodeTest, testDecodeInvalid)
{
    std::ifstream file("utf8_invalid.txt", std::ios::binary |std::ios::in);
    ASSERT_TRUE(file);
    
    std::string line;
    unsigned lineNum = 1;
    std::vector<unsigned> badLines;
    std::vector<std::uint32_t> unicode;

    while (std::getline(file, line))
    {
        if (auto result = decode(line.begin(), line.end(), std::back_inserter(unicode)); 
            result.error != DecodeError::OK)
        {
            badLines.push_back(lineNum);
            EXPECT_NE(result.resultIter, line.end());
        }
        ++lineNum;
    }

    constexpr std::array INVALID_LINES{ 75, 76, 83, 84, 85, 93, 102, 103, 105, 106, 107, 108, 109, 110, 114, 115, 116, 117, 124, 125, 130, 135, 140, 145, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 169, 175, 176, 177, 207, 208, 209, 210, 211, 220, 221, 222, 223, 224, 232, 233, 234, 235, 236, 247, 248, 249, 250, 251, 252, 253, 257, 258, 259, 260, 261, 262, 263, 264};
    EXPECT_THAT(badLines, testing::ElementsAreArray(INVALID_LINES));
}