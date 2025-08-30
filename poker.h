#ifndef POKER_H_
#define POKER_H_

#include <array>

#include "tables.h"

/**
 * bit scheme for a card:
 *
 * xxxbbbbb | bbbbbbbb | cdhsrrrr | xxpppppp
 *
 * where:
 *   p = prime number of rank (deuce=2, trey=3, four=5, ... , king=37, ace=41)
 *   r = rank of card (deuce=0, trey=1, four=2, ... , king=11, ace=12)
 *   cdhs = suit of card (bit turned on based on suit)
 *   b = bit turned on depending on rank of card
 */

inline int q(const std::array<uint32_t, 5>& hand) {
  return (hand[0] | hand[1] | hand[2] | hand[3] | hand[4]) >> 16;
}

inline bool is_flush(const std::array<uint32_t, 5>& hand) {
  return hand[0] & hand[1] & hand[2] & hand[3] & hand[4] & 0xf000;
}

template <typename HashFunc>
uint16_t eval5(const HashFunc& hash, const std::array<uint32_t, 5>& hand) {
  int idx = q(hand);

  // Check for flushes and straight flushes
  if (is_flush(hand)) {
    return flush_table[idx];
  }

  // Check for Straights and High card hands.
  uint16_t s;
  if ((s = unique5[idx])) {
    return s;
  }

  // Perfect hash lookup for remaining hands
  idx = (hand[0] & 0xff) * (hand[1] & 0xff) * (hand[2] & 0xff) * (hand[3] & 0xff) * (hand[4] & 0xff);
  return VALUES[hash(idx)];
}

template <typename HashFunc>
uint16_t eval5(const HashFunc& hash, uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, uint32_t c5) {
  int q = (c1 | c2 | c3 | c4 | c5) >> 16;

  // Check for flushes and straight flushes
  if (c1 & c2 & c3 & c4 & c5 & 0xf000) {
    return flush_table[q];
  }

  // Check for Straights and High card hands.
  if (uint16_t s = unique5[q]; s) {
    return s;
  }

  // Perfect hash lookup for remaining hands
  q = (c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) * (c4 & 0xff) * (c5 & 0xff);
  return VALUES[hash(q)];
}

#endif // POKER_H_
