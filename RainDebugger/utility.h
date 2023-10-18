#pragma once
#include <vector>
#include <Windows.h>
#include <string>

struct Process
{
	unsigned int id;
	std::string name;
};

void GetProcesses(std::vector<Process>& processes);