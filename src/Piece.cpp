#include "Piece.h"

// STATIC

sf::Texture Chess::Piece::preloadTexture(char symbol)
{
    std::string texturePath = "assets/";
    texturePath += std::isupper(symbol) ? "white_" : "black_";

    switch (std::tolower(symbol))
    {
    case 'p':
        texturePath += "pawn";
        break;
    case 'r':
        texturePath += "rook";
        break;
    case 'n':
        texturePath += "knight";
        break;
    case 'b':
        texturePath += "bishop";
        break;
    case 'q':
        texturePath += "queen";
        break;
    case 'k':
        texturePath += "king";
        break;
    }

    texturePath += ".png";

    auto texture = sf::Texture(texturePath);

    texture.setSmooth(0);

    return texture;
}

const sf::Texture Chess::Piece::WHITE_PAWN_TEXTURE = Chess::Piece::preloadTexture('P');
const sf::Texture Chess::Piece::WHITE_ROOK_TEXTURE = Chess::Piece::preloadTexture('R');
const sf::Texture Chess::Piece::WHITE_KNIGHT_TEXTURE = Chess::Piece::preloadTexture('N');
const sf::Texture Chess::Piece::WHITE_BISHOP_TEXTURE = Chess::Piece::preloadTexture('B');
const sf::Texture Chess::Piece::WHITE_QUEEN_TEXTURE = Chess::Piece::preloadTexture('Q');
const sf::Texture Chess::Piece::WHITE_KING_TEXTURE = Chess::Piece::preloadTexture('K');

const sf::Texture Chess::Piece::BLACK_PAWN_TEXTURE = Chess::Piece::preloadTexture('p');
const sf::Texture Chess::Piece::BLACK_ROOK_TEXTURE = Chess::Piece::preloadTexture('r');
const sf::Texture Chess::Piece::BLACK_KNIGHT_TEXTURE = Chess::Piece::preloadTexture('n');
const sf::Texture Chess::Piece::BLACK_BISHOP_TEXTURE = Chess::Piece::preloadTexture('b');
const sf::Texture Chess::Piece::BLACK_QUEEN_TEXTURE = Chess::Piece::preloadTexture('q');
const sf::Texture Chess::Piece::BLACK_KING_TEXTURE = Chess::Piece::preloadTexture('k');

// OBJECT

Chess::Piece::Piece(char symbol, sf::RectangleShape &tile)
    : m_HasMoved(false)
{
    m_Descriptor = std::isupper(symbol) ? Color::White : Color::Black;

    switch (std::tolower(symbol))
    {
    case 'p':
        m_Descriptor |= Type::Pawn;
        m_Descriptor |= PAWN_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_PAWN_TEXTURE : BLACK_PAWN_TEXTURE);
        break;
    case 'r':
        m_Descriptor |= Type::Rook;
        m_Descriptor |= ROOK_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_ROOK_TEXTURE : BLACK_ROOK_TEXTURE);
        break;
    case 'n':
        m_Descriptor |= Type::Knight;
        m_Descriptor |= KNIGHT_BISHOP_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_KNIGHT_TEXTURE : BLACK_KNIGHT_TEXTURE);
        break;
    case 'b':
        m_Descriptor |= Type::Bishop;
        m_Descriptor |= KNIGHT_BISHOP_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_BISHOP_TEXTURE : BLACK_BISHOP_TEXTURE);
        break;
    case 'q':
        m_Descriptor |= Type::Queen;
        m_Descriptor |= QUEEN_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_QUEEN_TEXTURE : BLACK_QUEEN_TEXTURE);
        break;
    case 'k':
        m_Descriptor |= Type::King;
        m_Descriptor |= KING_VALUE;
        p_Sprite = new sf::Sprite(std::isupper(symbol) ? WHITE_KING_TEXTURE : BLACK_KING_TEXTURE);
        break;
    }

    p_Sprite->setScale(sf::Vector2f(tile.getSize().x / p_Sprite->getTexture().getSize().x, tile.getSize().y / p_Sprite->getTexture().getSize().y));
}

void Chess::Piece::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform.combine(getTransform());
    target.draw(*p_Sprite, states);
}
