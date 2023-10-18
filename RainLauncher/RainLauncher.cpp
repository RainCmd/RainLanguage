// RainLauncher.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Builder.h>
#include <VirtualMachine.h>

int main(int cnt, char** args)
{
	for (size_t i = 0; i < cnt; i++)
	{
		std::cout << args[i] << "\n";
	}
	std::cout << "Hello World!\n";
}