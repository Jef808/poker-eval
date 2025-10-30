#include <SFML/Graphics.hpp>

#include "app/range_selector.hpp"

int main() {
  // Initialize config
  const auto windowSizeU = sf::Vector2u{960u, 1080u};
  const auto windowSizeF = static_cast<sf::Vector2f>(windowSizeU);
  const auto fontPath = "resources/DejaVuSans.ttf";

  // Initialize resources
  sf::Font font(fontPath);

  // Initialize window
  auto window = sf::RenderWindow(sf::VideoMode(windowSizeU), "Holdem Evaluator");
  window.setFramerateLimit(60);

  // Initialize gui objects
  RangeSelector rangeSelector(font);
  rangeSelector.update({20.f, 20.f}, {50.f, 50.f}, 1.f);

  // Define event handlers
  const auto onClose = [&window](const sf::Event::Closed&) {
    window.close();
  };
  const auto onKeyPress = [&window](const sf::Event::KeyPressed& key) {
    if (key.scancode == sf::Keyboard::Scancode::Q) {
      window.close();
    }
  };
  const auto onMouseClick = [&window, &rangeSelector](const sf::Event::MouseButtonPressed& mouse) {
    if (mouse.button == sf::Mouse::Button::Left) {
      sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
      auto handOpt = rangeSelector.getHandAtPosition(mousePos);
      if (handOpt) {
        rangeSelector.toggleSelectHand(*handOpt);
      }
    }
  };

  // Main loop
  while (window.isOpen()) {
    // Attach event handlers
    window.handleEvents(onClose, onKeyPress, onMouseClick);

    // Clear window
    window.clear(sf::Color::Black);

    // Draw gui elements
    rangeSelector.draw(window);

    // Display window contents
    window.display();
  }
}
