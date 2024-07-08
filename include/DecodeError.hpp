#pragma once

namespace brilliant {
    namespace utf8 {

        enum class DecodeError 
        {
            OK,
            EXPECTED_CONTINUATION_PREFIX,
            INVALID_CODE_POINT_LEN,
            OVERLONG_SEQUENCE,
            INVALID_CODE_POINT,
            UNEXPECTED_SEQUENCE_END
        };

    }
}