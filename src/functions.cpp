#include "functions.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "gpu.h"
using namespace std;

void load_rom(u8 memory[], const char *path)
{
    size_t file_size;
    ifstream ifs(path, ios_base::in | ios_base::binary);
    ifs.seekg(0, ios_base::end);
    file_size = (size_t)ifs.tellg();
    ifs.seekg(0, ios_base::beg);
    if (!ifs.good()) {
        cout << "Error reading file: " << path << endl;
    }
    char *buff = new char[file_size];
    ifs.read(buff, file_size);
    ifs.close();
    std::memcpy(memory, buff, file_size);
    delete buff;
}

void load_rom(u8 memory[], u8 data[], size_t size)
{
    std::memcpy(memory, data, size);
}

bool half_carry_add(u16 a, u16 b) 
{
    return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;
}

bool half_carry_sub(u16 a, u16 b)
{
    return ((a & 0xf) - (b & 0xf)) < 0;
}

bool full_carry_add(u16 a, u16 b)
{
    return (((a & 0xff) + (b & 0xff)) & 0x100) == 0x100;
}

bool full_carry_sub(u16 a, u16 b)
{
    return ((a & 0xff) - (b & 0xff)) < 0;
}

u8 swap(u8 x) 
{
    return 0xff & (x >> 4 | x << 4);
}