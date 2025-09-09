#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cstdint>

#include "utils.h"
#include "montecarlo.h"

#include "app/card.hpp"
#include "app/card_selector.hpp"
#include "app/hand.hpp"
#include "app/reset_button.hpp"


int main() {
  auto montecarlo = MonteCarlo();
  const size_t num_simulations = 100000;
  std::vector<uint16_t> results(num_simulations * 2, 0);

  const sf::Vector2u initialWindowSize = {960u, 1080u};

  auto window = sf::RenderWindow(sf::VideoMode(initialWindowSize), "Holdem Evaluator");
  window.setFramerateLimit(60);

  const sf::Vector2f handSelectorPosition = {20.f, 500.f};

  auto cardSelector = CardSelector(handSelectorPosition, {60.f, 90.f});

  auto hand1 = Hand({20.f, 20.f}, {120.f, 180.f});
  auto hand2 = Hand({320.f, 20.f}, {120.f, 180.f});

  auto resetButton = ResetButton({handSelectorPosition.x + 13.f * 60.f + 20.f, handSelectorPosition.y}, {100.f, 50.f});

  int nextInput = 0;
  std::optional<Card> debugCard;

  // --- MonteCarlo wiring state ---
  std::vector<uint32_t> hands;
  bool computePending = false;
  bool computedOnce = false;

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
        debugCard = cardSelector.getClickedCard(sf::Mouse::getPosition(window));
        if (!debugCard.has_value()) {
          if (resetButton.isClicked(sf::Mouse::getPosition(window))) {
            // reset state
            hand1.reset();
            hand2.reset();
            cardSelector.reset();
            nextInput = 0;
            debugCard.reset();

            // reset computation state
            computePending = false;
            computedOnce = false;
            prob1 = prob2 = probTie = 0.f;
          }
        }
      }
    }

    window.clear(sf::Color(100, 40, 40)); // Dark red background

    // Input handling
    if (debugCard.has_value() && nextInput <= 3) {
      if (nextInput < 2) {
        hand1.setCard(nextInput, *debugCard);
        cardSelector.selectCard(*debugCard);
        debugCard.reset();
        ++nextInput;
      } else {
        hand2.setCard(nextInput - 2, *debugCard);
        cardSelector.selectCard(*debugCard);
        debugCard.reset();
        ++nextInput;
      }
    }

    // Trigger computation when hands are full
    if (nextInput == 4 && !computedOnce && !computePending) {
      results.clear();
      hands.clear();
      hands.push_back(to_engine_card(*hand1.getCard(0)));
      hands.push_back(to_engine_card(*hand1.getCard(1)));
      hands.push_back(to_engine_card(*hand2.getCard(0)));
      hands.push_back(to_engine_card(*hand2.getCard(1)));

      montecarlo.set_hands(hands.begin(), hands.end());
      montecarlo.simulate(results.data(), num_simulations);

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

      prob1 = static_cast<float>(num_wins1) / static_cast<float>(num_simulations);
      prob2 = static_cast<float>(num_wins2) / static_cast<float>(num_simulations);
      probTie = 1.f - prob1 - prob2;
      computedOnce = true;
    }

    // Draw UI
    cardSelector.draw(window);
    hand1.draw(window);
    hand2.draw(window);
    resetButton.draw(window);

    // TODO: Draw probabilities
    if (computedOnce) {
      sf::Font font("resources/DejaVuSans.ttf");
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

    window.display();
  }
}
