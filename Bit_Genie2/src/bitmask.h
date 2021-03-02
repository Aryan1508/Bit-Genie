#pragma once
#include <stdint.h>

namespace BitMask
{
  constexpr uint64_t file_a = 0x0101010101010101;
  constexpr uint64_t file_b = file_a << 1;
  constexpr uint64_t file_c = file_b << 1;
  constexpr uint64_t file_d = file_c << 1;
  constexpr uint64_t file_e = file_d << 1;
  constexpr uint64_t file_f = file_e << 1;
  constexpr uint64_t file_g = file_f << 1;
  constexpr uint64_t file_h = file_g << 1;
  constexpr uint64_t not_file_h = ~file_h;
  constexpr uint64_t not_file_a = ~file_a;
  constexpr uint64_t not_file_gh = ~(file_g | file_h);
  constexpr uint64_t not_file_ab = ~(file_a | file_b);
  constexpr uint64_t not_file_af = ~(file_a | file_f);
  constexpr uint64_t not_file_ah = ~(file_a | file_h);

  constexpr uint64_t rank1 = 0xff;
  constexpr uint64_t rank2 = rank1 << 8;
  constexpr uint64_t rank3 = rank2 << 8;
  constexpr uint64_t rank4 = rank3 << 8;
  constexpr uint64_t rank5 = rank4 << 8;
  constexpr uint64_t rank6 = rank5 << 8;
  constexpr uint64_t rank7 = rank6 << 8;
  constexpr uint64_t rank8 = rank7 << 8;
}