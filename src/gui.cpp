#include <SFML/Graphics.hpp>

#include "app/draw_range_hand.hpp"

class RangeSelector {
public:
  RangeSelector(const sf::Font& font)
    : font_{font} {}

  void draw(sf::RenderTarget& rt,
            const sf::Vector2f& position,
            const sf::Vector2f& cardSize,
            const float gap = 1.f) {
    sf::RectangleShape container;
    container.setOrigin({0.f, 0.f});
    container.setSize({14.f * cardSize.x + 13 * gap, cardSize.y + 13 * gap});
    container.setPosition(position);
    container.setFillColor(sf::Color::Black);
    rt.draw(container);

    for (int rank1 = 14; rank1 >= 2; --rank1) {
      // First draw the pair (upper-left corner of the remaining hands to draw)
      const sf::Vector2f pos = {
        position.x + (14 - rank1) * (cardSize.x + gap),
        position.y + (14 - rank1) * (cardSize.y + gap)
      };
      RangeHand hand{rank1, rank1};
      drawRangeHand(hand, rt, font_, pos, cardSize);

      for (int rank2 = rank1 - 1; rank2 >= 2; --rank2) {
        // Draw the suited hand along the row
        sf::Vector2f pos = {
          position.x + (14 - rank2) * (cardSize.x + gap),
          position.y + (14 - rank1) * (cardSize.y + gap)
        };
        RangeHand hand{rank1, rank2, true};
        drawRangeHand(hand, rt, font_, pos, cardSize);

        // Draw the offsuit hand along the column
        pos.x = position.x + (14 - rank1) * (cardSize.x + gap);
        pos.y = position.y + (14 - rank2) * (cardSize.y + gap);
        hand.suited = false;
        drawRangeHand(hand, rt, font_, pos, cardSize);
      }
    }
  }

private:
  const sf::Font& font_;
};


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
  sf::Vector2f rangeSelectorPosition = {20.f, 20.f};
  sf::Vector2f rangeSelectorCardSize = {50.f, 50.f};
  float rangeSelectorGap = 1.f;

  // Define event handlers
  const auto onClose = [&window](const sf::Event::Closed&) {
    window.close();
  };
  const auto onKeyPress = [&window](const sf::Event::KeyPressed& key) {
    if (key.scancode == sf::Keyboard::Scancode::Q) {
      window.close();
    }
  };

  // Main loop
  while (window.isOpen()) {
    // Attach event handlers
    window.handleEvents(onClose, onKeyPress);

    // Clear window
    window.clear(sf::Color::Black);

    // Draw gui elements
    rangeSelector.draw(window,
                       rangeSelectorPosition,
                       rangeSelectorCardSize,
                       rangeSelectorGap);

    // Display window contents
    window.display();
  }
}
