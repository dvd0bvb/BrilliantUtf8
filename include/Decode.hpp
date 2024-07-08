#pragma once

#include <expected>
#include <ranges>
#include "Util.hpp"
#include "DecodeError.hpp"

namespace brilliant
{
    namespace utf8
    {

        template <class Iter>
        struct DecodeResult
        {
            DecodeError error;
            Iter resultIter;
        };

        inline constexpr bool checkCodeUnitMask(char8_t codeUnit, char8_t mask, char8_t expectedPrefix) noexcept
        {
            return (codeUnit & mask) == expectedPrefix;
        }

        constexpr inline std::expected<std::uint8_t, DecodeError> getCodePointSize(char8_t codeUnit) noexcept
        {
            for (auto &&[i, tuple] : std::views::enumerate(std::views::zip(MASKS, PREFIXES)))
            {
                if (checkCodeUnitMask(codeUnit, std::get<0>(tuple), std::get<1>(tuple)))
                {
                    return i;
                }
            }
            return std::unexpected(DecodeError::INVALID_CODE_POINT_LEN);
        }

        constexpr inline std::expected<std::uint32_t, DecodeError> decodeCodePoint(char8_t codePoint)
        {
            if (const auto result = getCodePointSize(codePoint))
            {
                if (result.value() != 0u)
                {
                    return std::unexpected(DecodeError::INVALID_CODE_POINT_LEN);
                }
                return static_cast<std::uint32_t>(codePoint);
            }
            else
            {
                return std::unexpected(result.error());
            }
        }

        template <class InputIter>
        requires std::constructible_from<char8_t, typename std::iterator_traits<InputIter>::value_type>
        constexpr std::expected<std::pair<std::uint32_t, InputIter>, DecodeError>
        decodeCodePoint(InputIter input, InputIter end) noexcept
        {
            std::uint32_t codePoint{};
            char8_t codeUnit = static_cast<char8_t>(*input);
            if (const auto result = getCodePointSize(codeUnit))
            {
                const auto maskIdx = result.value();
                codePoint = static_cast<std::uint32_t>(codeUnit & ~MASKS[maskIdx]);
                ++input;
                if (maskIdx > 0)
                {
                    for (auto i = 0; i < maskIdx; ++i)
                    {
                        if (input == end)
                        {
                            return std::unexpected(DecodeError::UNEXPECTED_SEQUENCE_END);
                        }

                        codeUnit = *input;
                        if (!checkCodeUnitMask(codeUnit, CONTINUATION_MASK, CONTINUATION_PREFIX))
                        {
                            return std::unexpected(DecodeError::EXPECTED_CONTINUATION_PREFIX);
                        }

                        codePoint <<= 6;
                        codePoint |= codeUnit & ~CONTINUATION_MASK;
                        ++input;
                    }
                }

                if ((maskIdx == 1 && codePoint < 0x80) ||
                    (maskIdx == 2 && codePoint < 0x800) ||
                    (maskIdx == 3 && codePoint < 0x10000))
                {
                    return std::unexpected(DecodeError::OVERLONG_SEQUENCE);
                }
                else if ((maskIdx == 2 && 0xd800 <= codePoint && codePoint <= 0xdfff) || //surrogates
                    (maskIdx == 3 && 0x10ffff < codePoint)) //out of range
                {
                    return std::unexpected(DecodeError::INVALID_CODE_POINT);
                }

                return std::make_pair(codePoint, input);
            }
            else
            {
                return std::unexpected(result.error());
            }
        }

        template <class InputIter, class OutputIter>
        constexpr DecodeResult<InputIter> decode(InputIter input, InputIter end, OutputIter out) noexcept
        {
            while (input != end)
            {
                if (const auto result = decodeCodePoint(input, end))
                {
                    const auto &pair = result.value();
                    *out = pair.first;
                    input = pair.second;
                }
                else
                {
                    return DecodeResult{result.error(), input};
                }
            }
            return DecodeResult{DecodeError::OK, end};
        }

    } // namespace utf8
} // namespace brilliant