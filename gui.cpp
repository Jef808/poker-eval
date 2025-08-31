#include <SFML/Graphics.hpp>

const sf::Vector2f cardSize = {120.f, 180.f};

void drawCard(sf::RenderTarget& rt, const sf::Vector2f& position, int rank, char suit) {
  sf::RectangleShape cardRect(cardSize);
  cardRect.setPosition(position);
  cardRect.setOrigin(cardSize / 2.f);
  cardRect.setFillColor(sf::Color::White);
  rt.draw(cardRect);

  // Draw card border
  sf::RectangleShape border(cardSize);
  border.setPosition(position);
  border.setOrigin(cardSize / 2.f);
  border.setFillColor(sf::Color::Transparent);
  border.setOutlineThickness(5.f);
  border.setOutlineColor(sf::Color::Black);
  rt.draw(border);

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
  rankText.setCharacterSize(40);
  rankText.setPosition({position.x - cardSize.x / 2.f + 10.f, position.y - cardSize.y / 2.f + 10.f});

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
  suitText.setCharacterSize(40);

  const float rankTextWidth = rankText.getLocalBounds().size.x;
  const float offset = rankTextWidth / 4.f;
  suitText.setPosition({position.x - cardSize.x / 2.f + (rank == 10 ? 17.f : 6.f), position.y - cardSize.y / 2.f + 50.f});
  rt.draw(rankText);
  rt.draw(suitText);
}

int main() {
  const sf::Vector2u windowSize = {1920u, 1080u};

  auto window = sf::RenderWindow(sf::VideoMode(windowSize), "Holdem Evaluator");
  window.setFramerateLimit(60);

  const sf::Vector2f CENTER = {windowSize.x / 2.f, windowSize.y / 2.f};

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

    window.clear(sf::Color(40, 40, 40)); // Dark green background

    sf::RectangleShape cardRect(cardSize);
    drawCard(window, CENTER, 14, 'd');

    window.display();
  }
}
