#include <fstream>
#include <vector>
#include <iostream>
#include <iterator>
#include <cassert>
#include <cstring>
#include <cerrno>
#include <algorithm>

#include "gpu.h"
#include "utils.h"

using namespace std;

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

u8 utils::set_cond(u8 x, u8 mask, bool cond) 
{
    if (cond) 
        return x | mask;
    else 
        return x & (~mask);
}

void utils::to_lower(std::string &s) 
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
} 

void utils::to_upper(std::string &s) 
{
    std::transform(s.begin(), s.end(), s.begin(), ::toupper);
} 

bool utils::bit(int x, int bit)
{
    return (bool)(x & (1 << bit));
}

void utils::load_file(std::vector<u8> &dest, const std::string &file_path)
{
    std::ifstream ifs(file_path, std::ios_base::in | std::ios_base::binary);
    ifs.unsetf(std::ios_base::skipws);
    ifs.seekg(0, std::ios_base::end);
    std::streampos file_size = (size_t)ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    if (!ifs.good()) {
        std::cout << "Error reading file " << file_path << ": " << std::strerror(errno) 
                  << std::endl;
		return;
    }
    
    dest.reserve(file_size);
    dest.insert(dest.begin(), 
        std::istream_iterator<u8>(ifs), std::istream_iterator<u8>());
    ifs.close();
}