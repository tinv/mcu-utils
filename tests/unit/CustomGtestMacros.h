
#ifndef CUSTOM_GTEST_MACROS_H
#define CUSTOM_GTEST_MACROS_H

#include <gtest/gtest.h>
#include <cstring>

#define EXPECT_ARRAY_EQ(TARTYPE, reference, actual, element_count) \
    {\
    TARTYPE* reference_ = static_cast<TARTYPE *> (reference); \
    const TARTYPE* actual_ = static_cast<const TARTYPE *> (actual); \
    for(size_t cmp_i = 0; cmp_i < element_count; cmp_i++ ){\
      EXPECT_EQ(reference_[cmp_i], actual_[cmp_i]);\
    }\
    }

#endif
