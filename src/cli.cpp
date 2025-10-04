#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include "types.h"
#include "utils.h"
#include "evaluation.hpp"

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <hand1> <hand2> [board]\n\n";
    std::cout << "Arguments:\n";
    std::cout << "  hand1   First player's 2 cards (e.g., \"As Kh\")\n";
    std::cout << "  hand2   Second player's 2 cards (e.g., \"Qd Jc\")\n";
    std::cout << "  board   Optional board cards (e.g., \"Ts 9h 8d\")\n\n";
    std::cout << "Card format: [2-9TJQKA][shdc] (rank + suit)\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " \"As Ah\" \"Kd Kc\"\n";
    std::cout << "  " << program_name << " \"As Kh\" \"Qd Jc\" \"Ts 9h 8d\"\n";
}

std::vector<uint32_t> parse_cards(const std::string& cards_str) {
    std::vector<uint32_t> cards;
    std::istringstream iss(cards_str);
    std::string card_str;

    while (iss >> card_str) {
        if (card_str.length() != 2) {
            throw std::invalid_argument("Invalid card format: " + card_str);
        }

        char card_cstr[3];
        card_cstr[0] = card_str[0];
        card_cstr[1] = card_str[1];
        card_cstr[2] = '\0';

        uint32_t card = card_from_string(card_cstr);
        cards.push_back(card);
    }

    return cards;
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) {
        print_usage(argv[0]);
        return 1;
    }

    try {
        // Parse hands
        std::vector<uint32_t> hand1_cards = parse_cards(argv[1]);
        std::vector<uint32_t> hand2_cards = parse_cards(argv[2]);

        if (hand1_cards.size() != 2) {
            std::cerr << "Error: Hand 1 must contain exactly 2 cards\n";
            return 1;
        }
        if (hand2_cards.size() != 2) {
            std::cerr << "Error: Hand 2 must contain exactly 2 cards\n";
            return 1;
        }

        // Combine hands into single vector for Evaluator
        std::vector<uint32_t> hands;
        hands.insert(hands.end(), hand1_cards.begin(), hand1_cards.end());
        hands.insert(hands.end(), hand2_cards.begin(), hand2_cards.end());

        // Parse board if provided
        std::vector<uint32_t> board_cards;
        if (argc == 4) {
            board_cards = parse_cards(argv[3]);
            if (board_cards.size() > 5) {
                std::cerr << "Error: Board cannot contain more than 5 cards\n";
                return 1;
            }
        }

        // Check for duplicate cards
        std::vector<uint32_t> all_cards = hands;
        all_cards.insert(all_cards.end(), board_cards.begin(), board_cards.end());
        std::sort(all_cards.begin(), all_cards.end());
        if (std::adjacent_find(all_cards.begin(), all_cards.end()) != all_cards.end()) {
            std::cerr << "Error: Duplicate cards detected\n";
            return 1;
        }

        // Display input
        std::array<uint32_t, 2> hand1_array = {hands[0], hands[1]};
        std::array<uint32_t, 2> hand2_array = {hands[2], hands[3]};

        std::cout << "Player 1: " << to_string(hand1_array) << std::endl;
        std::cout << "Player 2: " << to_string(hand2_array) << std::endl;

        if (!board_cards.empty()) {
            std::string board_str;
            for (size_t i = 0; i < board_cards.size(); ++i) {
                if (i > 0) board_str += " ";
                board_str += to_string(board_cards[i]);
            }
            std::cout << "Board: " << board_str << std::endl;
        }
        std::cout << std::endl;

        // Evaluate
        auto evaluator = Evaluator();
        evaluator.set_hands(hands.begin(), hands.end());

        if (!board_cards.empty()) {
            evaluator.set_board(board_cards.begin(), board_cards.end());
        }

        auto result = evaluator.evaluate();
        float prob1 = result.win_prob;
        float prob_tie = result.tie_prob;
        float prob2 = 1.0f - prob1 - prob_tie;

        // Display results
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Player 1 win: " << std::setw(5) << prob1 * 100.0f << "%" << std::endl;
        std::cout << "Player 2 win: " << std::setw(5) << prob2 * 100.0f << "%" << std::endl;
        std::cout << "Ties:         " << std::setw(5) << prob_tie * 100.0f << "%" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
