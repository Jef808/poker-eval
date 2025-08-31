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
#include "poker.h"
#include "bitset_rankindex.h"

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

void print_bits(uint32_t value) {
  std::bitset<32> bits(value);
  std::string bit_str = bits.to_string();

  for (auto i = 0; i < 4; ++i) {
    std::cout << bit_str.substr(i * 8, 8);
    if (i < 3) std::cout << " ";
  }

  std::cout << " (0x" << std::hex << value << std::dec << ")" << std::endl;
}

auto main() -> int {
  auto deck = initialize_deck();
  std::random_device rd;
  std::mt19937 g(rd());

  // Benchmark eval5
  std::vector<uint32_t> cards{};
    cards.reserve(5000000);

  for (auto i = 0; i < 1000000; ++i) {
    std::shuffle(deck.begin(), deck.end(), g);
    for (auto j = 0; j < 5; ++j) {
      cards.push_back(deck[j]);
    }
  }

  auto hash = BitsetRankIndex(115856201, KEYS);

  std::vector<uint16_t> evals{};
  evals.reserve(1000000);

  auto start = std::chrono::high_resolution_clock::now();

  for (auto i = 0; i < 1000000; ++i) {
    auto eval = eval5(hash, std::array<uint32_t, 5>{cards[5 * i + 0], cards[5 * i + 1], cards[5 * i + 2], cards[5 * i + 3], cards[5 * i + 4]});
    evals.push_back(eval);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "5Cards: Evaluated " << evals.size() << " hands in " << duration << " ms" << std::endl;
  std::cout << "5Cards: Average time per hand: " << static_cast<double>(duration) / 1000000.0 << " ms" << std::endl;

  for (auto i = 0; i < 1000000; ++i) {
    if (i % 100000 == 0) {
      std::array<uint32_t, 5> hand = {cards[5 * i + 0], cards[5 * i + 1], cards[5 * i + 2], cards[5 * i + 3], cards[5 * i + 4]};
      std::cout << to_string(hand) << ": " << evals[i] << std::endl;
    }
  }


  // Benchmark eval7
  cards.clear();
  evals.clear();
  cards.reserve(7000000);

  for (auto i = 0; i < 1000000; ++i) {
    std::shuffle(deck.begin(), deck.end(), g);
    for (auto j = 0; j < 7; ++j) {
      cards.push_back(deck[j]);
    }
  }

  start = std::chrono::high_resolution_clock::now();

  for (auto i = 0; i < 1000000; ++i) {
    auto eval = eval7(hash, std::array<uint32_t, 7>{cards[7 * i + 0], cards[7 * i + 1], cards[7 * i + 2], cards[7 * i + 3], cards[7 * i + 4], cards[7 * i + 5], cards[7 * i + 6]});
    evals.push_back(eval);
  }

  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << "7Cards: Evaluated " << evals.size() << " hands in " << duration << " ms" << std::endl;
  std::cout << "7Cards: Average time per hand: " << static_cast<double>(duration) / 1000000.0 << " ms" << std::endl;

  return 0;
}
