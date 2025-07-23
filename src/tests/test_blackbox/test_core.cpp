
#include <cstdio> 
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <wchar.h>

#include <windows.h> 

#include <BBApi.h>
#include <Blackbox.h>


TEST(AppObjectTest, Instantiation) 
{
    char cmdlineWithoutApp[] = "\x00";
    HINSTANCE hInstance{};

    bbcore::Blackbox blackbox(hInstance, cmdlineWithoutApp);
} 
