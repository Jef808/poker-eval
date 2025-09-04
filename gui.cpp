#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

struct Card {
  int rank;
  char suit;
};

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

class CardSelector {
  public:
    CardSelector(const sf::Vector2f& position, const sf::Vector2f& cardSize)
      : position_(position), cardSize_(cardSize) {}

    void draw(sf::RenderTarget& rt) const {
      sf::RectangleShape selectorRect({cardSize_.x * 13.f, cardSize_.y * 4.f});
      selectorRect.setOrigin({0.f, 0.f});
      selectorRect.setPosition(position_);
      selectorRect.setFillColor(sf::Color::Black);
      rt.draw(selectorRect);

      for (auto suit : {'h', 'd', 'c', 's'}) {
        for (int rank = 2; rank <= 14; ++rank) {
          const sf::Vector2f cardPosition = {
              position_.x + (rank - 2) * cardSize_.x,
              position_.y + (suit == 'h' ? 0.f : suit == 'd' ? cardSize_.y : suit == 'c' ? 2 * cardSize_.y : 3 * cardSize_.y)
          };
          drawCard(rank, suit, rt, cardPosition, cardSize_ - sf::Vector2f{1.f, 1.f});
          if (std::find(selectedCards_.begin(), selectedCards_.end(), (rank - 2) + (suit == 'h' ? 0 : suit == 'd' ? 13 : suit == 'c' ? 26 : 39)) != selectedCards_.end()) {
            sf::RectangleShape highlight(cardSize_);
            highlight.setPosition(cardPosition);
            highlight.setFillColor(sf::Color(0, 40, 0, 200)); // Semi-transparent green
            rt.draw(highlight);
          }
        }
      }
    }

    std::optional<Card> getClickedCard(const sf::Vector2i& mousePos) const {
      sf::Vector2f relativePos = static_cast<sf::Vector2f>(mousePos) - position_;

      if (relativePos.x < 0 || relativePos.y < 0 || relativePos.x >= cardSize_.x * 13.f || relativePos.y >= cardSize_.y * 4.f) {
        return std::nullopt;
      }

      int col = static_cast<int>(relativePos.x / cardSize_.x);
      int row = static_cast<int>(relativePos.y / cardSize_.y);

      if (col < 0 || col >= 13 || row < 0 || row >= 4) {
        return std::nullopt;
      }

      int rank = col + 2;
      char suit;
      switch (row) {
        case 0: suit = 'h'; break;
        case 1: suit = 'd'; break;
        case 2: suit = 'c'; break;
        case 3: suit = 's'; break;
        default: return std::nullopt;
      }

      return Card{rank, suit};
    }

    void selectCard(const Card& card) {
      int index = (card.rank - 2) + (card.suit == 'h' ? 0 : card.suit == 'd' ? 13 : card.suit == 'c' ? 26 : 39);
      selectedCards_.push_back(index);
    }

  private:
    sf::Vector2f position_;
    sf::Vector2f cardSize_;
    std::vector<int> selectedCards_;
};

class Hand {
  public:
    Hand(const sf::Vector2f& position, const sf::Vector2f& cardSize)
      : position_(position), cardSize_(cardSize) {}

    void draw(sf::RenderTarget& rt) const {
      const sf::Vector2f size_ = {cardSize_.x * 2.f + 5.f, cardSize_.y};
      sf::RectangleShape handRect(size_);
      handRect.setPosition(position_);
      handRect.setFillColor(sf::Color(0, 100, 0)); // Dark green
      rt.draw(handRect);

      for (int i = 0; i < 2; ++i) {
        if (cards_[i].has_value()) {
          drawCard(cards_[i]->rank, cards_[i]->suit, rt, position_ + sf::Vector2f{i * (cardSize_.x + 5.f), 0.f}, cardSize_);
        } else {
          sf::RectangleShape cardBack(cardSize_);
          cardBack.setPosition(position_ + sf::Vector2f{i * (cardSize_.x + 5.f), 0.f});
          cardBack.setFillColor(sf::Color::Blue);
          rt.draw(cardBack);
        }
      }
    }

    void setCard(int index, const Card& card) {
      if (index < 0 || index >= 2) return;
      cards_[index] = card;
    }

  private:
    sf::Vector2f position_;
    sf::Vector2f cardSize_;
    std::optional<Card> cards_[2];
};

int main() {
  const sf::Vector2u initialWindowSize = {960u, 1080u};

  auto window = sf::RenderWindow(sf::VideoMode(initialWindowSize), "Holdem Evaluator");
  window.setFramerateLimit(60);

  const sf::Vector2f handSelectorPosition = {20.f, 500.f};

  auto cardSelector = CardSelector(handSelectorPosition, {60.f, 90.f});

  auto hand1 = Hand({20.f, 20.f}, {120.f, 180.f});
  auto hand2 = Hand({320.f, 20.f}, {120.f, 180.f});

  int nextInput = 0;

  std::optional<Card> debugCard;

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
      }
    }

    const sf::Vector2f windowSize = sf::Vector2f(window.getSize());
    const sf::Vector2f CENTER = windowSize / 2.f;

    window.clear(sf::Color(100, 40, 40)); // Dark red background

    const sf::Vector2f cardSize = {120.f, 180.f};

    cardSelector.draw(window);

    hand1.setCard(0, Card{14, 'h'});
    hand2.setCard(0, Card{10, 's'});

    cardSelector.selectCard(Card{14, 'h'});
    cardSelector.selectCard(Card{10, 's'});

    hand1.draw(window);
    hand2.draw(window);

    if (debugCard.has_value()) {
      drawCard(debugCard->rank, debugCard->suit, window, CENTER - cardSize / 2.f, cardSize);
    }

    window.display();
  }
}
