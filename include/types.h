#ifndef TYPES_H_
#define TYPES_H_

#include <cstdint>

enum Suit {
  SPADES = 1,
  HEARTS = 2,
  DIAMONDS = 4,
  CLUBS = 8,
};

constexpr uint32_t MAX_HASH_KEY = 115856201;

struct EvalResult {
  float win_prob;
  float tie_prob;
};

#endif // TYPES_H_
