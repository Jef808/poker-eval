#ifndef UTILS_H_
#define UTILS_H_

#include "types.h"

#include <array>
#include <string>
#include <vector>

std::string to_string(uint32_t card);
uint32_t card_from_rank_suit(int rank, int suit);
uint32_t card_from_string(char* card_s);
std::array<uint32_t, 5> hand_from_string(const std::string& hand_s);
std::array<uint32_t, 52> initialize_deck();

template<size_t N>
inline std::string to_string(const std::array<uint32_t, N>& hand) {
  auto hand_cpy = hand;
  std::sort(hand_cpy.begin(), hand_cpy.end(), [](uint32_t a, uint32_t b){
    return ((a >> 8) & 0xf) > ((b >> 8) & 0xf);
  });

  std::string result;
  for (size_t i = 0; i < hand_cpy.size(); ++i) {
    if (i > 0) result += " ";
    result += to_string(hand_cpy[i]);
  }
  return result;
}

#endif // UTILS_H_
