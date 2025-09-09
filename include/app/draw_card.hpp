#ifndef DRAW_CARD_H_
#define DRAW_CARD_H_

#include <SFML/Graphics.hpp>
#include "app/card.hpp"

// Draws a single card (rank/suit) into a render target
void drawCard(const Card& card,
              sf::RenderTarget& rt,
              const sf::Vector2f& position,
              const sf::Vector2f& size);

#endif // DRAW_CARD_H_
