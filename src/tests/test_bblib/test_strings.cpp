
#include <cstdio> 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <wchar.h>

#include <windows.h> 

#include <bblib.h>
#include <bbrc.h>
#include <bbroot.h>
#include <bbshell.h>

constexpr const char * oneSymbolTwoBytesCorrectUtf8 = "\xd0\xb0";
static const wchar_t * oneSymbolTwoBytesStringWchar = (const wchar_t*)"\x30\x04\x00\x00"; // 'а'

// from wchar to utf-8

TEST(StringConvWcharToUtf8, TellsNumberOfWrittenSymbols)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];

    auto numSymbols = wchar_to_utf8(oneSymbolTwoBytesStringWchar, buffer, BUF_SIZE);

    ASSERT_THAT(numSymbols, ::testing::Eq(3));
}

TEST(StringConvWcharToUtf8, WritesNoMoreThanMaxOutputBufSize)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];
    memset(buffer, 0x0, sizeof(buffer));

    (void)wchar_to_utf8(oneSymbolTwoBytesStringWchar, buffer, 1);

    ASSERT_THAT(buffer[1], ::testing::Eq(0x0));
}


TEST(StringConvWcharToUtf8, WritesEndingZero)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];
    memset(buffer, 0xff, sizeof(buffer));

    auto numSymbols = wchar_to_utf8(oneSymbolTwoBytesStringWchar, buffer, BUF_SIZE);

    ASSERT_THAT(buffer[2], ::testing::Eq(0x0));
}

TEST(StringConvWcharToUtf8, ConvertsEnglishString)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];
    memset(buffer, 0x00, sizeof(buffer));

    auto numSymbols = wchar_to_utf8(L"w", buffer, BUF_SIZE);

    ASSERT_THAT(buffer[0], ::testing::Eq('w'));
}

TEST(StringConvWcharToUtf8, ZeroOutputBufLength)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];

    auto numSymbols = wchar_to_utf8(oneSymbolTwoBytesStringWchar, buffer, 0);

    ASSERT_THAT(numSymbols, ::testing::Eq(0));
}

TEST(StringConvWcharToUtf8, InsufficientSpaceForOutputSymbolSequence)
{
    enum {BUF_SIZE = 4};
    char buffer[BUF_SIZE];
    memset(buffer, 0x00, sizeof(buffer));

    auto numSymbols = wchar_to_utf8(oneSymbolTwoBytesStringWchar, buffer, 1); 

    ASSERT_THAT(numSymbols, ::testing::Eq(0));
}

// utf-8 needed bytes length count 
TEST(StringCountWcharToUtf8, CalcNeededBytesWithEndingZero)
{ 
    auto numSymbols = wchar_to_utf8_length(oneSymbolTwoBytesStringWchar);

    ASSERT_THAT(numSymbols, ::testing::Eq(3));
}

// from utf-8 to wchar 

TEST(StringConvUtf8ToWchar, TellsNumberOfWrittenSymbols)
{
    enum {BUF_SIZE = 4};
    wchar_t buffer[BUF_SIZE];

    auto numSymbols = utf8_to_wchar(oneSymbolTwoBytesCorrectUtf8, buffer, BUF_SIZE);

    ASSERT_THAT(numSymbols, ::testing::Eq(2));
}

TEST(StringConvUtf8ToWchar, WritesNoMoreThanMaxOutputBufSize)
{
    enum {BUF_SIZE = 4};
    wchar_t buffer[BUF_SIZE];
    memset(buffer, 0xff, sizeof(buffer));

    (void)utf8_to_wchar(oneSymbolTwoBytesCorrectUtf8, buffer, 1);

    ASSERT_THAT(buffer[1], ::testing::Eq(0xffff));
}

TEST(StringConvUtf8ToWchar, WritesEndingZero)
{
    enum {BUF_SIZE = 4};
    wchar_t buffer[BUF_SIZE];
    memset(buffer, 0xff, sizeof(buffer));

    auto numSymbols = utf8_to_wchar(oneSymbolTwoBytesCorrectUtf8, buffer, BUF_SIZE);

    ASSERT_THAT(buffer[1], ::testing::Eq(wchar_t(0x0)));
}

TEST(StringConvUtf8ToWchar, ConvertsEnglishString)
{
    enum {BUF_SIZE = 4};
    wchar_t buffer[BUF_SIZE];
    memset(buffer, 0x0, sizeof(buffer));

    auto numSymbols = utf8_to_wchar("w", buffer, BUF_SIZE);

    ASSERT_THAT(buffer[0], ::testing::Eq(L'w'));
}

TEST(StringConvUtf8ToWchar, ZeroOutputBufLength)
{
    enum {BUF_SIZE = 4};
    wchar_t buffer[BUF_SIZE];

    auto numSymbols = utf8_to_wchar(oneSymbolTwoBytesCorrectUtf8, buffer, 0);

    ASSERT_THAT(numSymbols, ::testing::Eq(0));
}

TEST(StringCountUtf8ToWchar, CalcNeededBytesWithEndingZero)
{ 
    auto numSymbols = utf8_to_wchar_length(oneSymbolTwoBytesCorrectUtf8);

    ASSERT_THAT(numSymbols, ::testing::Eq(2));
}
