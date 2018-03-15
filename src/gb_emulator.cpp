#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "debug.h"
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
using namespace std;

vector<u8> read_rom(size_t &file_size, string path)
{
	ifstream ifs(path, ios_base::in | ios_base::binary);
	ifs.seekg(0, ios_base::end);
	file_size = ifs.tellg();
	ifs.seekg(0, ios_base::beg);

	if (!ifs.good())
		cout << "Error reading file: " + path << endl;
	
	char *buff = new char[file_size];
	ifs.read(buff, file_size);
	vector<u8> data(buff, buff + file_size);

	ifs.close();
	delete buff;
	return data;
}

int main()
{
	Processor gb_cpu;
	size_t rom_size;
	vector<u8> rom_data = read_rom(rom_size, "DMG_ROM.bin");
	
	/*for (u8 u: rom_data)
		cout << std::hex << (int)u << ' ';
		cout << std::endl;*/
	
	gb_cpu.map_to_memory(rom_data.data(), rom_size, 0);
	gb_cpu.run();
	
	for (unsigned int i = 0; i < rom_data.size(); i++) {
		if (i != 0 && (i % 16 == 0))
			cout << endl;
		cout << setw(2) << setfill('0') << hex << (int)rom_data[i] << ' ';
	}
	cout << endl;
	
	gb_cpu.print_register_values();
	
	return 0;
}
