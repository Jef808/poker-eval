#ifndef CARD_SELECTOR_H_
#define CARD_SELECTOR_H_

#include <SFML/Graphics.hpp>
#include "app/card.hpp"
#include <optional>
#include <vector>

class CardSelector {
  public:
    CardSelector(const sf::Vector2f& position, const sf::Vector2f& cardSize);

    void draw(sf::RenderTarget& rt) const;
    std::optional<Card> getClickedCard(const sf::Vector2i& mousePos) const;

    void selectCard(const Card& card);
    void reset();

  private:
    sf::Vector2f position_;
    sf::Vector2f cardSize_;
    std::vector<int> selectedCards_;
};

#endif // CARD_SELECTOR_H_
