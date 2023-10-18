// RainDebugger.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <vector>

int main()
{
    HWND hwnd = GetConsoleWindow();
    for (size_t i = 0; i < 3; i++)
    {
        ShowWindow(hwnd, SW_HIDE);

    }
}