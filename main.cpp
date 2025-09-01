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

  std::vector<uint32_t> hands = {
    card_from_rank_suit(14, SPADES),
    card_from_rank_suit(13, SPADES),
    card_from_rank_suit(10, HEARTS),
    card_from_rank_suit(11, SPADES),
  };

  std::array<uint32_t, 3> board = {
    card_from_rank_suit(2, SPADES),
    card_from_rank_suit(7, SPADES),
    card_from_rank_suit(9, HEARTS),
  };

  // Run Monte Carlo simulation
  std::cout << "Running Monte Carlo simulation...\n" << std::endl;
  std::cout << "Player 1: " << to_string(std::array<uint32_t,2>{hands[0], hands[1]}) << std::endl;
  std::cout << "Player 2: " << to_string(std::array<uint32_t,2>{hands[2], hands[3]}) << std::endl;
  std::cout << "Board: " << to_string(board) << "\n" << std::endl;

  const size_t num_simulations = 1000000;
  std::vector<uint16_t> results(num_simulations * 2);

  auto montecarlo = MonteCarlo();
  montecarlo.set_hands(hands.begin(), hands.end());
  montecarlo.set_board(board.begin(), board.end());

  auto mc_hands = montecarlo.hands();
  std::cout << "Hands set: " <<
    to_string(mc_hands[0][0]) << " " <<
    to_string(mc_hands[0][1]) << " " <<
    to_string(mc_hands[1][0]) << " " <<
    to_string(mc_hands[1][1]) << std::endl;

  auto mc_board = montecarlo.board();
  std::cout << "Board set: " <<
    to_string(mc_board[0]) << " " <<
    to_string(mc_board[1]) << " " <<
    to_string(mc_board[2]) << std::endl;

  montecarlo.simulate(results.data(), num_simulations);

  std::cout << "\nResults sample:" << std::endl;
  for (size_t i = 0; i < 10; ++i) {
    auto res1 = results[i * 2 + 0];
    auto res2 = results[i * 2 + 1];
    std::cout << "Simulation " << i + 1 << ": Player 1: " << res1 << ", Player 2: " << res2;
    if (res1 < res2) {
      std::cout << " => Player 1 wins";
    } else if (res2 < res1) {
      std::cout << " => Player 2 wins";
    } else {
      std::cout << " => Tie";
    }
    std::cout << std::endl;
  }

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

  std::cout << "After " << num_simulations << " simulations:" << std::endl;
  std::cout << "Player 1 win %: " << prob1 * 100.0f << "%" << std::endl;
  std::cout << "Player 2 win %: " << prob2 * 100.0f << "%" << std::endl;
  std::cout << "Ties: " << prob_tie * 100.0f << "%" << std::endl;

  return 0;
}
