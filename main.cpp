#include <array>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include <algorithm>
#include <random>
#include <chrono>

#include "types.h"
#include "utils.h"
#include "eval.h"
#include "bitset_rankindex.h"
#include "montecarlo.h"

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
  // example.cpp
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::vector<uint32_t> hands = {
    card_from_rank_suit(14, HEARTS),
    card_from_rank_suit(14, DIAMONDS),
    card_from_rank_suit(8, DIAMONDS),
    card_from_rank_suit(9, DIAMONDS),
  };

  const size_t num_simulations = 100000;
  std::vector<uint16_t> results(num_simulations * 2, 0);

  auto montecarlo = MonteCarlo();
  montecarlo.set_hands(hands.begin(), hands.end());
  std::cout << "Player 1: " << to_string(std::array<uint32_t,2>{hands[0], hands[1]}) << std::endl;
  std::cout << "Player 2: " << to_string(std::array<uint32_t,2>{hands[2], hands[3]}) << std::endl;

  // montecarlo.set_board(board.begin(), board.end());
  // std::cout << "Board: " << to_string(board) << "\n" << std::endl;
  montecarlo.set_board();
  std::cout << "Board: (empty)\n" << std::endl;

  start = std::chrono::high_resolution_clock::now();

  montecarlo.simulate(results.data(), num_simulations);

  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  int num_wins1 = 0;
  int num_wins2 = 0;

  for (size_t i = 0; i < num_simulations; ++i) {
    auto res1 = results[i * 2 + 0];
    auto res2 = results[i * 2 + 1];
    if (res1 < res2) {
      ++num_wins1;
    } else if (res2 < res1) {
      ++num_wins2;
    }
  }

  float prob1 = static_cast<float>(num_wins1) / static_cast<float>(num_simulations);
  float prob2 = static_cast<float>(num_wins2) / static_cast<float>(num_simulations);
  float prob_tie = 1.0f - prob1 - prob2;

  std::cout << std::fixed << std::setprecision(2);

  std::cout << "Player 1 win: " << std::setw(5) << prob1 * 100.0f << "%" << std::endl;
  std::cout << "Player 2 win: " << std::setw(5) << prob2 * 100.0f << "%" << std::endl;
  std::cout << "Ties:         " << std::setw(5) << prob_tie * 100.0f << "%" << std::endl;

  std::cout << "\nSimulation took " << duration << " ms" << std::endl;

  return 0;
}
