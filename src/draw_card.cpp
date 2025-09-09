#include "app/draw_card.hpp"
#include <SFML/Graphics.hpp>
#include <string>

void drawCard(const Card& card, sf::RenderTarget& rt, const sf::Vector2f& position, const sf::Vector2f& size) {
  sf::RectangleShape cardRect(size);
  cardRect.setPosition(position);
  cardRect.setFillColor(sf::Color::White);
  rt.draw(cardRect);

  sf::Font font("resources/DejaVuSans.ttf");

  sf::Text rankText(font);

  switch (card.rank) {
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
      rankText.setString(std::to_string(card.rank));
      break;
  }

  const float characterSize = size.y / 3.f;

  rankText.setCharacterSize(characterSize);
  // rankText.setPosition({position.x + (rank == 11 ? 24.f : 12.f), position.y + 4.f});

  const float rankTextWidth = rankText.getLocalBounds().size.x;
  const float rankCenterX = position.x + (size.x / 2.f) - (rankTextWidth / 2.f);
  rankText.setPosition({rankCenterX, position.y + 4.f});

  sf::Text suitText(font);
  switch (card.suit) {
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
