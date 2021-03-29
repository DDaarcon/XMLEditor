#include <iostream>
#include <vector>
#include <fstream>
#include "units.hpp"

/* 
	to add:
	quoted sentences in commands
 */


int main() {
	using Units::Unit;
	using Units::Interface;

	char const* instruction = "Unit address explain (accessing units): \nMAIN - main unit, \ti533 - unit with id 533, \nLiSt - child unit with name \"LiSt\", \t4 - 5th child unit\n\nUnit address example: \nMAIN>0>width\t(access main unit, 1st child unit, unit named \"width\")\n3>0\t(access 4th unit in memory, 1st child)\ni423>2\t(access unit with id 423, 3rd child unit)\n\nOpen file:\topenf MAIN <source>\nSave file:\tsavef <unit> <destination>\nDisplay:\tdisp <unit>\tDisplay all:\tdisp\nDisplay as XML:\tdispx <unit>\nChange value (int/float/string):\tvali/valf/vals <unit> <value>\nCopy:\tcopy <unit> <parent>\nCopy to memory:\tcopy <unit>\n\n";
	std::cout << instruction;

	Interface interface;
	
	std::string command;

	while (getline(std::cin, command)) {
		if (!interface.ReadCommand(command)) break;
	}


	

	return 0;
}
