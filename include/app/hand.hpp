#ifndef HAND_H_
#define HAND_H_

#include <SFML/Graphics.hpp>
#include <optional>
#include "app/card.hpp"

// Display a 2-card hand
class Hand {
  public:
    Hand(const sf::Vector2f& position, const sf::Vector2f& cardSize);

    void draw(sf::RenderTarget& rt) const;
    void setCard(int index, const Card& card);
    const std::optional<Card>& getCard(size_t i) const;
    void reset();

  private:
    sf::Vector2f position_;
    sf::Vector2f cardSize_;
    std::optional<Card> cards_[2];
};

#endif // HAND_H_
