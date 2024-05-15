#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char** args)
{
    ::testing::InitGoogleTest(&argc, args);
    ::testing::InitGoogleMock(&argc, args);
    int res = RUN_ALL_TESTS();

    return res;
}
