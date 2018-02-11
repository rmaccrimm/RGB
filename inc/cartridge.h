#include <string>
#include "definitions.h"

class Cartridge
{
public:
	Cartridge(std::string filename);

private:
	std::vector<u8> rom_data;
	std::vector<u8> read_rom(size_t &Filesize, std::string path);

};
