#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>

#include "tables.h"

// bit scheme for a card
/**
 * xxxbbbbb | bbbbbbbb | cdhsrrrr | xxpppppp
 *
 * where:
 *   p = prime number of rank (deuce=2, trey=3, four=5, ... , king=13, ace=41)
 *   r = rank of card (deuce=2, trey=3, four=4, ... , king=13, ace=14)
 *   cdhs = suit of card (bit turned on based on suit)
 *   b = bit turned on depending on rank of card
 */

enum Suit {
  SPADES = 1,
  HEARTS = 2,
  DIAMONDS = 4,
  CLUBS = 8,
};

static const int prime_for_rank[15] = {
  0, 0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41
};

inline uint32_t card_from_rank_suit(int rank, int suit) {
  return (prime_for_rank[rank] | ((rank - 2) << 8) | (1 << (16 + rank - 2)) |
          (suit << 12));
}

void print_bits(uint32_t value) {
  std::bitset<32> bits(value);
  std::string bit_str = bits.to_string();

  for (auto i = 0; i < 4; ++i) {
    std::cout << bit_str.substr(i * 8, 8);
    if (i < 3) std::cout << " ";
  }

  std::cout << " (0x" << std::hex << value << std::dec << ")" << std::endl;
}

struct Deck {
  std::array<uint32_t, 15> spades;
  std::array<uint32_t, 15> hearts;
  std::array<uint32_t, 15> diamonds;
  std::array<uint32_t, 15> clubs;
};

Deck initialize_deck() {
  Deck Cards{};
  for (int rank = 2; rank <= 14; ++rank) {
    Cards.spades[rank] = card_from_rank_suit(rank, SPADES);
    Cards.hearts[rank] = card_from_rank_suit(rank, HEARTS);
    Cards.diamonds[rank] = card_from_rank_suit(rank, DIAMONDS);
    Cards.clubs[rank] = card_from_rank_suit(rank, CLUBS);
  }
  return Cards;
}

inline bool is_flush(const std::array<uint32_t, 5>& hand) {
  return hand[0] & hand[1] & hand[2] & hand[3] & hand[4] & 0xf000;
}

inline int q(const std::array<uint32_t, 5>& hand) {
  return (hand[0] | hand[1] | hand[2] | hand[3] | hand[4]) >> 16;
}

std::array<uint16_t, 7937> gen_flush_tables() {
  std::array<uint32_t, 15> cards{0, 0};
  for (auto rank = 2; rank <= 14; ++rank) {
    cards[rank] = card_from_rank_suit(rank, SPADES);
  }

  std::array<uint16_t, 7937> flush_table{};
  flush_table.fill(0);

  // Straight flushes
  uint16_t hand_value = 1;
  for (int rank = 14; rank >= 6; --rank) {
    auto q_value = q({cards[rank], cards[rank - 1], cards[rank - 2], cards[rank - 3], cards[rank - 4]});
    flush_table[q_value] = hand_value++;
  }
  // Last straight flush (A-2-3-4-5)
  auto q_value = q({cards[14], cards[2], cards[3], cards[4], cards[5]});
  flush_table[q_value] = hand_value++;

  // Skip over straight flushes and four of a kinds
  hand_value += 156 + 156;

  // Flushes (non-straight)
  for (auto a = 14; a >= 7; --a) {
    for (auto b = a - 1; b >= 5; --b) {
      for (auto c = b - 1; c >= 4; --c) {
        for (auto d = c - 1; d >= 3; --d) {
          for (auto e = d - 1; e >= 2; --e) {
            auto q_value = q({cards[a], cards[b], cards[c], cards[d], cards[e]});
            if (flush_table[q_value] == 0) {
              flush_table[q_value] = hand_value++;
            }
          }
        }
      }
    }
  }

  return flush_table;
}

auto main() -> int {
  using Card = uint32_t;
  using Hand = std::array<Card, 5>;

  Deck deck = initialize_deck();

  Card king_diamonds = card_from_rank_suit(13, DIAMONDS);
  Card five_spades = deck.spades[5];
  Card jack_clubs = card_from_rank_suit(11, CLUBS);

  Hand low_flush_hand = {
    card_from_rank_suit(6, HEARTS),
    card_from_rank_suit(5, HEARTS),
    card_from_rank_suit(4, HEARTS),
    card_from_rank_suit(3, HEARTS),
    card_from_rank_suit(2, HEARTS)
  };

  Hand second_low_flush_hand = {
    card_from_rank_suit(6, DIAMONDS),
    card_from_rank_suit(5, DIAMONDS),
    card_from_rank_suit(4, DIAMONDS),
    card_from_rank_suit(3, DIAMONDS),
    card_from_rank_suit(2, DIAMONDS)
  };

  Hand high_flush_hand = {
    card_from_rank_suit(14, CLUBS),
    card_from_rank_suit(13, CLUBS),
    card_from_rank_suit(12, CLUBS),
    card_from_rank_suit(11, CLUBS),
    card_from_rank_suit(10, CLUBS)
  };

  Hand second_high_flush_hand = {
    card_from_rank_suit(14, HEARTS),
    card_from_rank_suit(13, HEARTS),
    card_from_rank_suit(12, HEARTS),
    card_from_rank_suit(11, HEARTS),
    card_from_rank_suit(9, HEARTS)
  };

  Hand non_flush_hand = {
    card_from_rank_suit(14, HEARTS),
    card_from_rank_suit(13, HEARTS),
    card_from_rank_suit(12, HEARTS),
    card_from_rank_suit(11, HEARTS),
    card_from_rank_suit(9, DIAMONDS)
  };

  auto low_q_value = q(low_flush_hand);
  auto second_low_q_value = q(second_low_flush_hand);
  auto high_q_value = q(high_flush_hand);
  auto second_high_q_value = q(second_high_flush_hand);

  std::cout << "King of Diamonds: ";
  print_bits(king_diamonds);

  std::cout << "Five of Spades:   ";
  print_bits(five_spades);

  std::cout << "Jack of Clubs:    ";
  print_bits(jack_clubs);

  std::cout << "6 of Hearts:     ";
  print_bits(low_flush_hand[0]);
  std::cout << "5 of Hearts:     ";
  print_bits(low_flush_hand[1]);
  std::cout << "4 of Hearts:     ";
  print_bits(low_flush_hand[2]);
  std::cout << "3 of Hearts:     ";
  print_bits(low_flush_hand[3]);
  std::cout << "2 of Hearts:     ";
  print_bits(low_flush_hand[4]);

  std::cout << "Is low hand flush: " << (is_flush(low_flush_hand) ? "true" : "false") << std::endl;
  std::cout << "Is second low hand flush: " << (is_flush(second_low_flush_hand) ? "true" : "false") << std::endl;
  std::cout << "Is high hand flush: " << (is_flush(high_flush_hand) ? "true" : "false") << std::endl;
  std::cout << "Is second high hand flush: " << (is_flush(second_high_flush_hand) ? "true" : "false") << std::endl;
  std::cout << "Is non-flush hand flush: " << (is_flush(non_flush_hand) ? "true" : "false") << std::endl;

  std::cout << "Hand value of low flush hand: " << flush_table[low_q_value] << std::endl;

  std::cout << "Q value of low flush hand: " << low_q_value << std::endl;
  std::cout << "Q value of second low flush hand: " << second_low_q_value << std::endl;
  std::cout << "Q value of high flush hand: " << high_q_value << std::endl;
  std::cout << "Q value of second high flush hand: " << second_high_q_value << std::endl;

  auto my_flush_table = gen_flush_tables();

  if (my_flush_table != flush_table) {
    std::cout << "Generated flush table does not match expected flush table." << std::endl;
    return 1;
  }

  return 0;
}
