#ifndef UTILS_H_
#define UTILS_H_

#include <array>
#include <string>

inline std::string to_string(uint32_t card) {
  static const char* rank_strs[] = {
    "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"
  };
  static const char* suit_strs[] = {"?", "s", "h", "?", "d", "?", "?", "?", "c"};

  int rank = (card >> 8) & 0xf;
  int suit = (card >> 12) & 0xf;

  return std::string(rank_strs[rank]) + suit_strs[suit];
}

inline std::string to_string(const std::array<uint32_t, 5>& hand) {
  auto hand_cpy = hand;
  std::sort(hand_cpy.begin(), hand_cpy.end(), [](uint32_t a, uint32_t b){
    return ((a >> 8) & 0x0f) > ((b >> 8) & 0x0f);
  });

  std::string result;
  for (size_t i = 0; i < hand_cpy.size(); ++i) {
    if (i > 0) result += " ";
    result += to_string(hand_cpy[i]);
  }
  return result;
}

static const int prime_for_rank[15] = {
  0, 0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41
};

inline uint32_t card_from_rank_suit(int rank, int suit) {
  return (prime_for_rank[rank] | ((rank - 2) << 8) | (1 << (16 + rank - 2)) |
          (suit << 12));
}

inline uint32_t card_from_string(char* card_s) {
  int rank;
  int suit;

  switch (card_s[0]) {
    case '2': rank = 2; break;
    case '3': rank = 3; break;
    case '4': rank = 4; break;
    case '5': rank = 5; break;
    case '6': rank = 6; break;
    case '7': rank = 7; break;
    case '8': rank = 8; break;
    case '9': rank = 9; break;
    case 'T': rank = 10; break;
    case 'J': rank = 11; break;
    case 'Q': rank = 12; break;
    case 'K': rank = 13; break;
    case 'A': rank = 14; break;
    default: throw std::invalid_argument("Invalid card string");
  }

  switch (card_s[1]) {
    case 's': suit = 1; break;
    case 'h': suit = 2; break;
    case 'd': suit = 4; break;
    case 'c': suit = 8; break;
    default: throw std::invalid_argument("Invalid card string");
  }

  return card_from_rank_suit(rank, suit);
}

inline std::array<uint32_t, 5> hand_from_string(const std::string& hand_s) {
  std::array<uint32_t, 5> hand{};
  size_t pos = 0;
  size_t card_index = 0;

  while (pos < hand_s.size() && card_index < 5) {
    // Skip whitespace
    while (pos < hand_s.size() && std::isspace(hand_s[pos])) {
      ++pos;
    }
    if (pos + 1 >= hand_s.size()) break; // Not enough characters for a card

    // Read card
    char card_str[3] = {hand_s[pos], hand_s[pos + 1], '\0'};
    hand[card_index++] = card_from_string(card_str);
    pos += 2;
  }

  if (card_index != 5) {
    throw std::invalid_argument("Hand string must contain exactly 5 cards");
  }

  return hand;
}

#endif // UTILS_H_
