#include <SFML/Graphics.hpp>
#include <algorithm>
#include <optional>
#include <vector>
#include <cstdint>

#include "utils.h"
#include "montecarlo.h"

#include "app/card.hpp"
#include "app/card_selector.hpp"
#include "app/cards_display.hpp"
#include "app/reset_button.hpp"

#include "app/debug_display.hpp"


int main() {
  auto montecarlo = MonteCarlo();
  const size_t num_simulations = 100000;
  std::vector<uint16_t> results(num_simulations * 2, 0);

  const sf::Vector2u initialWindowSize = {960u, 1080u};
  const sf::Vector2f initialWindowSizeF = static_cast<sf::Vector2f>(initialWindowSize);

  sf::Font font("resources/DejaVuSans.ttf");
  auto debug_display = DebugDisplay(font, 14);

  auto window = sf::RenderWindow(sf::VideoMode(initialWindowSize), "Holdem Evaluator");
  window.setFramerateLimit(60);

  const sf::Vector2f handSelectorPosition = {20.f, 500.f};

  auto cardSelector = CardSelector(handSelectorPosition, {60.f, 90.f});

  auto hand1 = Hand({20.f, 20.f}, {120.f, 180.f});
  auto hand2 = Hand({320.f, 20.f}, {120.f, 180.f});

  auto board = Board({20.f, 250.f}, {120.f, 180.f});

  auto resetButton = ResetButton({handSelectorPosition.x + 13.f * 60.f + 20.f, handSelectorPosition.y}, {100.f, 50.f});

  int nextInput = 0;
  std::optional<Card> selectedCard;

  // --- MonteCarlo wiring state ---
  std::vector<uint32_t> player_cards;
  std::vector<uint32_t> board_cards;
  int lastComputedInput = -1;

  float prob1 = 0.f, prob2 = 0.f, probTie = 0.f;

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Q) {
          window.close();
        }
      } else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        // If the card selector was clicked, store that card
        selectedCard = cardSelector.getClickedCard(sf::Mouse::getPosition(window));
        if (!selectedCard.has_value()) {
          // If the reset button was clicked, reset everything
          if (resetButton.isClicked(sf::Mouse::getPosition(window))) {
            // reset state
            hand1.reset();
            hand2.reset();
            board.reset();
            cardSelector.reset();
            nextInput = 0;
            selectedCard.reset();

            // reset computation state
            prob1 = prob2 = probTie = 0.f;
            lastComputedInput = -1;
          }
        }
      }
    }

    window.clear(sf::Color(100, 40, 40)); // Dark red background

    // Input handling
    if (selectedCard.has_value() && nextInput <= 8) {
      if (nextInput < 2) {
        hand1.setCard(nextInput, *selectedCard);
      } else if (nextInput < 4) {
        hand2.setCard(nextInput - 2, *selectedCard);
      } else {
        board.setCard(nextInput - 4, *selectedCard);
      }
      cardSelector.selectCard(*selectedCard);
      selectedCard.reset();
      ++nextInput;
    }

    // Trigger computation when hands are dealt and new card has been added
    if (hand1.size() + hand2.size() == 4) {
      int board_size = board.size();

      bool should_compute = false;
      if (board_size < 3) {
        // Flop not yet dealt, compute only once
        should_compute = (lastComputedInput == -1);
      } else {
        // Flop is set, compute when a new card is added
        should_compute = (lastComputedInput != 4 + board_size);
      }

      if (should_compute) {
        results.clear();
        player_cards.clear();
        player_cards.push_back(to_engine_card(*hand1.getCard(0)));
        player_cards.push_back(to_engine_card(*hand1.getCard(1)));
        player_cards.push_back(to_engine_card(*hand2.getCard(0)));
        player_cards.push_back(to_engine_card(*hand2.getCard(1)));

        montecarlo.set_hands(player_cards.begin(), player_cards.end());
        montecarlo.set_board(); // Clear board first

        if (board_size >= 3) {
          // Flop is set
          board_cards.clear();
          std::transform(board.getCards().begin(), board.getCards().begin() + board_size,
                         std::back_inserter(board_cards),
                         [](const std::optional<Card>& c) { return to_engine_card(*c); }
                         );
          montecarlo.set_board(board_cards.begin(), board_cards.end());
        }

        size_t r_simulations = montecarlo.simulate(results.data(), num_simulations);

        int num_wins1 = 0;
        int num_wins2 = 0;

        for (size_t i = 0; i < r_simulations; ++i) {
          auto res1 = results[i * 2 + 0];
          auto res2 = results[i * 2 + 1];
          if (res1 < res2) {
            ++num_wins1;
          } else if (res2 < res1) {
            ++num_wins2;
          }
        }

        prob1 = static_cast<float>(num_wins1) / static_cast<float>(r_simulations);
        prob2 = static_cast<float>(num_wins2) / static_cast<float>(r_simulations);
        probTie = 1.f - prob1 - prob2;

        lastComputedInput = 4 + board_size;
      }
    }

    // Draw UI
    cardSelector.draw(window);
    hand1.draw(window);
    hand2.draw(window);
    board.draw(window);
    resetButton.draw(window);

    if (lastComputedInput != -1) {
      sf::Text resultText(font);
      resultText.setCharacterSize(24);
      resultText.setFillColor(sf::Color::White);

      resultText.setString(
        "Player 1 Win: " + std::to_string(static_cast<int>(prob1 * 10000.f) / 100.f) + "%\n" +
        "Player 2 Win: " + std::to_string(static_cast<int>(prob2 * 10000.f) / 100.f) + "%\n" +
        "Tie: " + std::to_string(static_cast<int>(probTie * 10000.f) / 100.f) + "%"
      );
      resultText.setPosition({600.f, 50.f});
      window.draw(resultText);
    }

    // Draw debug info
    const auto& mousePos = sf::Mouse::getPosition(window);

    auto debug_s = "(" + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y) + ")";

    debug_display.update(debug_s);
    debug_display.draw(window);

    window.display();
  }
}
