/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreUtils/Ripemd160.hpp>

#include <sstream>

namespace A1::Utils::Ripemd160
{

namespace
{

unsigned int inv(unsigned int a)
{
    unsigned int b = 0;

    b |= ((a >> 0) & 0xFF) << 24;
    b |= ((a >> 8) & 0xFF) << 16;
    b |= ((a >> 16) & 0xFF) << 8;
    b |= ((a >> 24) & 0xFF) << 0;

    return b;
}

unsigned int shl(unsigned int a, unsigned int s)
{
    return ((a << s) | (a >> (32 - s)));
}

unsigned int f(int j, unsigned int x, unsigned int y, unsigned int z)
{
    if (j > 63) return x ^ (y | (~z));
    if (j > 47) return (x & z) | (y & (~z));
    if (j > 31) return (x | (~y)) ^ z;
    if (j > 15) return (x & y) | ((~x) & z);
    if (j > -1) return x ^ y ^ z;
    return 0;
}

unsigned int K(int j)
{
    if (j > 63) return 0xa953fd4e;
    if (j > 47) return 0x8f1bbcdc;
    if (j > 31) return 0x6ed9eba1;
    if (j > 15) return 0x5a827999;
    if (j > -1) return 0x00000000;
    return 0;
}

unsigned int K_(int j)
{
    if (j > 63) return 0x00000000;
    if (j > 47) return 0x7a6d76e9;
    if (j > 31) return 0x6d703ef3;
    if (j > 15) return 0x5c4dd124;
    if (j > -1) return 0x50a28be6;
    return 0;
}

unsigned int r(int j)
{
    if (j > 63) 
    {
        int r[16] = { 4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };

        return r[j % 16];
    }
    if (j > 47) 
    {
        int r[16] = { 1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2 };

        return r[j % 16];
    }
    if (j > 31) 
    {
        int r[16] = { 3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12 };

        return r[j % 16];
    }

    if (j > 15) 
    {
        int r[16] = { 7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8 };

        return r[j % 16];
    }

    if (j > -1) 
    {
        int r[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

        return r[j % 16];
    }
    return 0;
}

unsigned int r_(int j)
{
    if (j > 63) 
    {
        int r_[16] = { 12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };

        return r_[j % 16];
    }

    if (j > 47) 
    {
        int r_[16] = { 8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14 };

        return r_[j % 16];
    }

    if (j > 31) 
    {
        int r_[16] = { 15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13 };

        return r_[j % 16];
    }

    if (j > 15) 
    {
        int r_[16] = { 6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2 };

        return r_[j % 16];
    }

    if (j > -1) 
    {
        int r_[16] = { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12 };

        return r_[j % 16];
    }
    return 0;
}

unsigned int s(int j)
{
    if (j > 63) 
    {
        int s[16] = { 9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };

        return s[j % 16];
    }

    if (j > 47) 
    {
        int s[16] = { 11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12 };

        return s[j % 16];
    }

    if (j > 31) 
    {
        int s[16] = { 11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5 };

        return s[j % 16];
    }

    if (j > 15) 
    {
        int s[16] = { 7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12 };

        return s[j % 16];
    }

    if (j > -1) 
    {
        int s[16] = { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8 };

        return s[j % 16];
    }
    return 0;
}

unsigned int s_(int j)
{
    if (j > 63) 
    {
        int s_[16] = { 8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };

        return s_[j % 16];
    }

    if (j > 47) 
    {
        int s_[16] = { 15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8 };

        return s_[j % 16];
    }

    if (j > 31) 
    {
        int s_[16] = { 9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5 };

        return s_[j % 16];
    }

    if (j > 15) 
    {
        int s_[16] = { 9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11 };

        return s_[j % 16];
    }

    if (j > -1) 
    {
        int s_[16] = { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6 };

        return s_[j % 16];
    }
    return 0;
}

void block_hash(unsigned int *X, unsigned int &h0, unsigned int &h1, unsigned int &h2, unsigned int &h3, unsigned int &h4)
{
    unsigned int A, B, C, D, E, A_, B_, C_, D_, E_, T;

    A = A_ = h0;
    B = B_ = h1;
    C = C_ = h2;
    D = D_ = h3;
    E = E_ = h4;

    for (int j = 0; j < 80; j++)
    {
        T = shl(A + f(j, B, C, D) + X[r(j)] + K(j), s(j)) + E;
        A = E;
        E = D;
        D = shl(C, 10);
        C = B;
        B = T;

        T = shl(A_ + f(79 - j, B_, C_, D_) + X[r_(j)] + K_(j), s_(j)) + E_;
        A_ = E_;
        E_ = D_;
        D_ = shl(C_, 10);
        C_ = B_;
        B_ = T;
    }

    T = h1 + C + D_;
    h1 = h2 + D + E_;
    h2 = h3 + E + A_;
    h3 = h4 + A + B_;
    h4 = h0 + B + C_;
    h0 = T;
}

} // namespace

std::string hash( std::string_view const data )
{
    unsigned long long file_size = data.length();
    std::uint8_t bytes[file_size];

    std::memcpy(bytes, data.data(), file_size);
    int size_of_last_block = file_size % 64;

    int number_of_zero_bytes;

    if (size_of_last_block < 56)
        number_of_zero_bytes = 56 - size_of_last_block;
    else
        number_of_zero_bytes = 64 - size_of_last_block + 56;

    unsigned long long num_of_blocks = (file_size + number_of_zero_bytes + 8) / 64;

    int how_many_blocks_modified = 1;

    if (size_of_last_block >= 56)
        how_many_blocks_modified++;

    unsigned int h0 = 0x67452301;
    unsigned int h1 = 0xefcdab89;
    unsigned int h2 = 0x98badcfe;
    unsigned int h3 = 0x10325476;
    unsigned int h4 = 0xc3d2e1f0;

    std::uint8_t X[64];
    unsigned int foo[16];

    unsigned long long start = 0;
    for (unsigned long long i = 0; i < num_of_blocks - how_many_blocks_modified; i++)
    {
        std::copy(bytes + start, bytes + start + 64, X);//getting 64 bytes of data into x
        start += 64;

        for (int j=0; j<16; j++)
        {
            foo[j] = 256 * 256 * 256 * (int)(X[4 * j + 3]) + 256 * 256 * int(X[4 * j + 2]) + 256 * int(X[4 * j + 1]) + int(X[4 * j]);
        }

        block_hash(foo, h0, h1, h2, h3, h4);
    }

    if (size_of_last_block < 56)
    {
        unsigned char buf[64];

        for (int i=0; i<size_of_last_block; i++){
            buf[i] = (unsigned char)(bytes[start + i]);
        }


        int it = size_of_last_block;

        buf[it++] = 0x80;

        for (int i = 0; i < number_of_zero_bytes - 1; buf[it++] = 0, i++);

        unsigned long long bit_file_size = file_size * 8;

        for (int i = 0; i < 8; i++)
            buf[it++] = (unsigned char)(bit_file_size >> i * 8);

        for (int i = 0; i < 16; i++)
        {
            foo[i] = 256 * 256 * 256 * buf[4 * i + 3] + 256 * 256 * buf[4 * i + 2] + 256 * buf[4 * i + 1] + buf[4 * i];
        }

        block_hash(foo, h0, h1, h2, h3, h4);
    }
    else
    {
        unsigned char buf[128];

        for (int i=0; i<size_of_last_block; i++){
            buf[i] = (unsigned char)(bytes[start + i]);
        }

        int it = size_of_last_block;

        buf[it++] = 0x80;

        for (int i = 0; i < number_of_zero_bytes - 1; buf[it++] = 0, i++);

        unsigned long long bit_file_size = file_size * 8;

        for (int i = 0; i < 8; i++)
            buf[it++] = (unsigned char)(bit_file_size >> i * 8);


        for (int i = 0; i < 16; i++)
        {
            foo[i] = 256 * 256 * 256 * buf[4 * i + 3] + 256 * 256 * buf[4 * i + 2] + 256 * buf[4 * i + 1] + buf[4 * i];
        }

        block_hash(foo, h0, h1, h2, h3, h4);

        for (int i = 16; i < 32; i++)
        {
            foo[i - 16] = 256 * 256 * 256 * buf[4 * i + 3] + 256 * 256 * buf[4 * i + 2] + 256 * buf[4 * i + 1] + buf[4 * i];
        }

        block_hash(foo, h0, h1, h2, h3, h4);
    }

    std::ostringstream s;

    s << std::hex << inv(h0) << inv(h1) << inv(h2) << inv(h3) << inv(h4);

    return s.str();
}

} // namespace A1::Utils::Ripemd160
