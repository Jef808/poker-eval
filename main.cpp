#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>

#include <algorithm>
#include <random>
#include <chrono>

#include "types.h"
#include "utils.h"
#include "tables.h"
#include "bitset_rankindex.h"

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

// Highest q value of 5 distinct cards is 7936 (A-K-Q-J-10)
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

void print_bits(uint32_t value) {
  std::bitset<32> bits(value);
  std::string bit_str = bits.to_string();

  for (auto i = 0; i < 4; ++i) {
    std::cout << bit_str.substr(i * 8, 8);
    if (i < 3) std::cout << " ";
  }

  std::cout << " (0x" << std::hex << value << std::dec << ")" << std::endl;
}

std::array<uint32_t, 52> initialize_deck() {
  std::array<uint32_t, 52> cards{};
  size_t index = 0;
  for (auto suit : {SPADES, HEARTS, DIAMONDS, CLUBS}) {
    for (int rank = 2; rank <= 14; ++rank) {
      cards[index++] = card_from_rank_suit(rank, suit);
    }
  }
  return cards;
}

auto main() -> int {
  using Card = uint32_t;
  using Hand = std::array<Card, 5>;


  std::cout << "2h: " << to_string(card_from_rank_suit(2, HEARTS)) << std::endl;
  std::cout << "2s: " << to_string(card_from_rank_suit(2, SPADES)) << std::endl;
  std::cout << "2d: " << to_string(card_from_rank_suit(2, DIAMONDS)) << std::endl;
  std::cout << "2c: " << to_string(card_from_rank_suit(2, CLUBS)) << std::endl;

  std::cout << "Ah: " << to_string(card_from_rank_suit(14, HEARTS)) << std::endl;
  std::cout << "Kh: " << to_string(card_from_rank_suit(13, HEARTS)) << std::endl;
  std::cout << "Qh: " << to_string(card_from_rank_suit(12, HEARTS)) << std::endl;
  std::cout << "Jh: " << to_string(card_from_rank_suit(11, HEARTS)) << std::endl;
  std::cout << "Th: " << to_string(card_from_rank_suit(10, HEARTS)) << std::endl;
  std::cout << "9h: " << to_string(card_from_rank_suit(9, HEARTS)) << std::endl;
  std::cout << "8h: " << to_string(card_from_rank_suit(8, HEARTS)) << std::endl;
  std::cout << "7h: " << to_string(card_from_rank_suit(7, HEARTS)) << std::endl;
  std::cout << "6h: " << to_string(card_from_rank_suit(6, HEARTS)) << std::endl;
  std::cout << "5h: " << to_string(card_from_rank_suit(5, HEARTS)) << std::endl;
  std::cout << "4h: " << to_string(card_from_rank_suit(4, HEARTS)) << std::endl;
  std::cout << "3h: " << to_string(card_from_rank_suit(3, HEARTS)) << std::endl;
  std::cout << "2h: " << to_string(card_from_rank_suit(2, HEARTS)) << std::endl;

  auto deck = initialize_deck();
  std::random_device rd;
  std::mt19937 g(rd());

  std::vector<uint16_t> cards{};
  cards.reserve(5000000);

  for (auto i = 0; i < 1000000; ++i) {
    std::shuffle(deck.begin(), deck.end(), g);
    for (auto j = 0; j < 5; ++j) {
      cards.push_back(deck[j]);
    }
  }

  std::cout << "Initializing succinct bitset rank vector..." << std::endl;

  auto hash = BitsetRankIndex(115856201, KEYS);

  std::cout << "Done" << std::endl;
  std::cout << "# of keys: " << hash.size() << std::endl;

  std::cout << "hash.contains(18240449): " << (hash.contains(18240449) ? "true" : "false") << std::endl;
  std::cout << "hash.contains(18240450): " << (hash.contains(18240450) ? "true" : "false") << std::endl;
  std::cout << "hash.index(24783229): " << hash(24783229) << std::endl;

  std::vector<uint16_t> evals{};
  evals.reserve(1000000);

  auto start = std::chrono::high_resolution_clock::now();

  for (auto i = 0; i < 1000000; ++i) {
    auto eval = eval5(hash, {cards[5 * i + 0], cards[5 * i + 1], cards[5 * i + 2], cards[5 * i + 3], cards[5 * i + 4]});
    evals.push_back(eval);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "Evaluated " << evals.size() << " hands in " << duration << " ms" << std::endl;
  std::cout << "Average time per hand: " << static_cast<double>(duration) / 1000000.0 << " ms" << std::endl;

  for (auto i = 0; i < 1000000; ++i) {
    if (i % 100000 == 0) {
      std::array<uint32_t, 5> hand = {cards[5 * i + 0], cards[5 * i + 1], cards[5 * i + 2], cards[5 * i + 3], cards[5 * i + 4]};
      std::cout << to_string(hand) << ": " << evals[i] << std::endl;
    }
  }
  Card two_hearts = card_from_rank_suit(2, HEARTS);
  Card king_diamonds = card_from_rank_suit(13, DIAMONDS);
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

  std::cout << "Two of Hearts:    ";
  print_bits(two_hearts);

  std::cout << "King of Diamonds: ";
  print_bits(king_diamonds);

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

  std::cout << "*** HASH ***" << std::endl;
  uint32_t samples[] = {48, 72, 80, 108, 112, 120, 368, 378, 392, 396, 405, 408, 420};
  std::cout << "Expect: 0, 1, 2, 3, 4, 5, 20, 21, 22, 23, 24, 25, 26," << std::endl;
  std::cout << "Got:    ";
  for (auto s : samples) {
    std::cout << hash(s) << ", ";
  }
  std::cout << std::endl;

  auto high_flush_hand_eval = eval5(hash, high_flush_hand);
  std::cout << to_string(high_flush_hand) << ": " << high_flush_hand_eval << std::endl;

  Hand king_high_straight = {
    card_from_rank_suit(13, HEARTS),
    card_from_rank_suit(12, HEARTS),
    card_from_rank_suit(11, HEARTS),
    card_from_rank_suit(10, HEARTS),
    card_from_rank_suit(9, SPADES),
  };

  auto king_high_straight_eval = eval5(hash, king_high_straight);
  std::cout << to_string(king_high_straight) << ": " << king_high_straight_eval << std::endl;

  Hand worst_hand = {
    card_from_rank_suit(7, HEARTS),
    card_from_rank_suit(5, HEARTS),
    card_from_rank_suit(4, HEARTS),
    card_from_rank_suit(3, HEARTS),
    card_from_rank_suit(2, SPADES),
  };

  auto worst_hand_eval = eval5(hash, worst_hand);
  std::cout << to_string(worst_hand) << ": " << worst_hand_eval << std::endl;

  Hand hand = {
    card_from_rank_suit(14, HEARTS),
    card_from_rank_suit(13, HEARTS),
    card_from_rank_suit(11, CLUBS),
    card_from_rank_suit(9, HEARTS),
    card_from_rank_suit(7, SPADES),
  };

  std::cout << to_string(hand) << ": " << eval5(hash, hand) << std::endl;

  hand = {
    card_from_rank_suit(3, HEARTS),
    card_from_rank_suit(3, SPADES),
    card_from_rank_suit(3, CLUBS),
    card_from_rank_suit(11, HEARTS),
    card_from_rank_suit(7, SPADES),
  };

  std::cout << to_string(hand) << ": " << eval5(hash, hand) << std::endl;

  int idx = q(hand);
  uint16_t s = unique5[idx];
  std::cout << "s: " << s << std::endl;

  idx = (hand[0] & 0xff) * (hand[1] & 0xff) * (hand[2] & 0xff) * (hand[3] & 0xff) * (hand[4] & 0xff);
  std::cout << "prime idx: " << idx << std::endl;
  std::cout << "contains: " << (hash.contains(idx) ? "true" : "false") << std::endl;
  std::cout << "hash:     " << hash(idx) << std::endl;
  std::cout << "Expected: " << "469" << std::endl;
  std::cout << "hash value: " << VALUES[hash(idx)] << std::endl;
  std::cout << "Expected:   " << "2369" << std::endl;

  hand = {
    card_from_rank_suit(10, HEARTS),
    card_from_rank_suit(9, SPADES),
    card_from_rank_suit(8, CLUBS),
    card_from_rank_suit(7, HEARTS),
    card_from_rank_suit(6, SPADES),
  };

  std::cout << to_string(hand) << ": " << eval5(hash, hand) << std::endl;

  hand = {
    card_from_rank_suit(2, HEARTS),
    card_from_rank_suit(2, SPADES),
    card_from_rank_suit(2, CLUBS),
    card_from_rank_suit(2, DIAMONDS),
    card_from_rank_suit(3, SPADES),
  };

  std::cout << to_string(hand) << ": " << eval5(hash, hand) << std::endl;
  std::cout << "Expected      : 166" << std::endl;

  hand = {
    card_from_rank_suit(8, HEARTS),
    card_from_rank_suit(8, SPADES),
    card_from_rank_suit(8, CLUBS),
    card_from_rank_suit(4, DIAMONDS),
    card_from_rank_suit(4, SPADES),
  };

  std::cout << to_string(hand) << ": " << eval5(hash, hand) << std::endl;
  std::cout << "Expected      : 248" << std::endl;

  std::cout << "hash(984): " << hash(984) << std::endl;
  std::cout << "Expected:  " << 67 << std::endl;

  std::cout << "hash(2079): " << hash(2079) << std::endl;
  std::cout << "Expected:   " << 136 << std::endl;

  std::cout << "hash(2450): " << hash(2450) << std::endl;
  std::cout << "Expected:   " << 154 << std::endl;

  std::cout << "hash(2604): " << hash(2604) << std::endl;
  std::cout << "Expected:   " << 166 << std::endl;

  std::cout << "hash(3256): " << hash(3256) << std::endl;
  std::cout << "Expected:   " << 195 << std::endl;

  std::cout << "hash(9207): " << hash(9207) << std::endl;
  std::cout << "Expected:   " << 418 << std::endl;

  return 0;
}
