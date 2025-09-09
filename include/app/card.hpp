#ifndef CARD_H_
#define CARD_H_

#include <cstdint>
#include <stdexcept>
#include "utils.h"

struct Card {
  int rank;  // 2..14
  char suit; // 'h', 'd', 'c', 's'
};

// Inline adapter for your engine card representation
inline uint32_t to_engine_card(const Card& c) {
  int suit;
  switch (c.suit) {
    case 'h': suit = HEARTS; break;
    case 'd': suit = DIAMONDS; break;
    case 'c': suit = CLUBS; break;
    case 's': suit = SPADES; break;
    default: throw std::runtime_error("Invalid suit");
  }
  return card_from_rank_suit(c.rank, suit);
}

#endif // CARD_H_
