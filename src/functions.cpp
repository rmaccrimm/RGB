#include "functions.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include "gpu.h"
using namespace std;

void utils::load_rom(u8 memory[], size_t offset, const char *path)
{
    size_t file_size;
    ifstream ifs(path, ios_base::in | ios_base::binary);
    ifs.seekg(0, ios_base::end);
    file_size = (size_t)ifs.tellg();
    ifs.seekg(0, ios_base::beg);
    if (!ifs.good()) {
        cout << "Error reading file " << path << ": " << std::strerror(errno) << endl;
		return;
    }
    char *buff = new char[file_size];
    ifs.read(buff, file_size);
    ifs.close();
    assert(file_size + offset <= 0x10000);
    std::memcpy(memory + offset, buff, file_size);
    delete buff;
}

void utils::load_rom(u8 memory[], size_t start, size_t offset, const char *path)
{
    size_t file_size;
    ifstream ifs(path, ios_base::in | ios_base::binary);
    ifs.seekg(0, ios_base::end);
    file_size = (size_t)ifs.tellg() - start;
    ifs.seekg(start, ios_base::beg);
    if (!ifs.good()) {
        cout << "Error reading file " << path << ": " << std::strerror(errno) << endl;
		return;
    }
    char *buff = new char[file_size];
    ifs.read(buff, file_size);
    ifs.close();
    assert(file_size + offset <= 0x10000);
    std::memcpy(memory + offset, buff, file_size);
    delete buff;
}

void utils::load_rom(u8 memory[], u8 data[], size_t size)
{
    std::memcpy(memory, data, size);
}

bool utils::half_carry_add(u16 a, u16 b) 
{
    return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;
}

bool utils::half_carry_sub(u16 a, u16 b)
{
    return ((a & 0xf) - (b & 0xf)) < 0;
}

bool utils::full_carry_add(u16 a, u16 b)
{
    return (((a & 0xff) + (b & 0xff)) & 0x100) == 0x100;
}

bool utils::full_carry_sub(u16 a, u16 b)
{
    return ((a & 0xff) - (b & 0xff)) < 0;
}

u8 utils::swap(u8 x) 
{
    return 0xff & (x >> 4 | x << 4);
}

u8 utils::set(u8 x, int bit) 
{
    assert(bit < 8);
    return x | (1 << bit);
}

u8 utils::reset(u8 x, int bit)
{
    assert(bit < 8);
    return x & (~(1 << bit));
}

void utils::to_lower(std::string &s) 
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
} 
