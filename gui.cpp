#include <SFML/Graphics.hpp>

void drawCard(int rank, char suit, sf::RenderTarget& rt, const sf::Vector2f& position, const sf::Vector2f& size) {
  sf::RectangleShape cardRect(size);
  cardRect.setPosition(position);
  cardRect.setFillColor(sf::Color::White);
  rt.draw(cardRect);

  sf::Font font("resources/DejaVuSans.ttf");

  sf::Text rankText(font);

  switch (rank) {
    case 14:
      rankText.setString("A");
      break;
    case 13:
      rankText.setString("K");
      break;
    case 12:
      rankText.setString("Q");
      break;
    case 11:
      rankText.setString("J");
      break;
    default:
      rankText.setString(std::to_string(rank));
      break;
  }

  const float characterSize = size.y / 3.f;

  rankText.setCharacterSize(characterSize);
  // rankText.setPosition({position.x + (rank == 11 ? 24.f : 12.f), position.y + 4.f});

  const float rankTextWidth = rankText.getLocalBounds().size.x;
  const float rankCenterX = position.x + (size.x / 2.f) - (rankTextWidth / 2.f);
  rankText.setPosition({rankCenterX, position.y + 4.f});

  sf::Text suitText(font);
  switch (suit) {
    case 'h':
      suitText.setString(L"♥");
      suitText.setFillColor(sf::Color::Red);
      rankText.setFillColor(sf::Color::Red);
      break;
    case 'd':
      suitText.setString(L"♦");
      suitText.setFillColor(sf::Color::Red);
      rankText.setFillColor(sf::Color::Red);
      break;
    case 'c':
      suitText.setString(L"♣");
      suitText.setFillColor(sf::Color::Black);
      rankText.setFillColor(sf::Color::Black);
      break;
    case 's':
      suitText.setString(L"♠");
      suitText.setFillColor(sf::Color::Black);
      rankText.setFillColor(sf::Color::Black);
      break;
  }
  suitText.setCharacterSize(characterSize * 1.2f);

  const float suitTextWidth = suitText.getLocalBounds().size.x;
  const float suitCenterX = position.x + (size.x / 2.f) - (suitTextWidth / 2.f);
  const float suitTextHeight = suitText.getLocalBounds().size.y;

  suitText.setPosition({suitCenterX, position.y + size.y / 2.f});

  rt.draw(rankText);
  rt.draw(suitText);
}

void drawCardSelector(const sf::Vector2f& position, sf::RenderTarget& rt) {
  const sf::Vector2f cardSize = {60.f, 90.f};
  sf::RectangleShape selectorRect({cardSize.x * 13.f, cardSize.y * 4.f});
  selectorRect.setOrigin({0.f, 0.f});
  selectorRect.setPosition(position);
  selectorRect.setFillColor(sf::Color::Black);
  rt.draw(selectorRect);

  for (auto suit : {'h', 'd', 'c', 's'}) {
    for (int rank = 2; rank <= 14; ++rank) {
      const sf::Vector2f cardPosition = {
          position.x + (rank - 2) * cardSize.x,
          position.y + (suit == 'h' ? 0.f : suit == 'd' ? cardSize.y : suit == 'c' ? 2 * cardSize.y : 3 * cardSize.y)
      };
      drawCard(rank, suit, rt, cardPosition, cardSize - sf::Vector2f{1.f, 1.f});
    }
  }
}

int main() {
  const sf::Vector2u initialWindowSize = {960u, 1080u};

  auto window = sf::RenderWindow(sf::VideoMode(initialWindowSize), "Holdem Evaluator");
  window.setFramerateLimit(60);

  while (window.isOpen()) {
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
        if (key->scancode == sf::Keyboard::Scancode::Q) {
          window.close();
        }
      }
    }

    const sf::Vector2f windowSize = sf::Vector2f(window.getSize());
    const sf::Vector2f CENTER = windowSize / 2.f;

    window.clear(sf::Color(100, 40, 40)); // Dark red background

    const sf::Vector2f cardSize = {120.f, 180.f};
    // drawCard(14, 'd', window, CENTER, cardSize);

    drawCardSelector({20.f, 500.f}, window);

    window.display();
  }
}
