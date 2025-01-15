#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

typedef unsigned char byte;

namespace Chess
{
    class Piece : public sf::Drawable, public sf::Transformable
    {
    public:
        enum Color
        {
            White = 0b00000000,
            Black = 0b00001000,
        };

        enum Type
        {
            Pawn = 0b00000001,
            Rook = 0b00000010,
            Knight = 0b00000011,
            Bishop = 0b00000100,
            Queen = 0b000000101,
            King = 0b00000110,
        };

    private:
        static sf::Texture preloadTexture(char symbol);

        // Preloaded Textures
        static const sf::Texture WHITE_PAWN_TEXTURE;
        static const sf::Texture WHITE_ROOK_TEXTURE;
        static const sf::Texture WHITE_KNIGHT_TEXTURE;
        static const sf::Texture WHITE_BISHOP_TEXTURE;
        static const sf::Texture WHITE_QUEEN_TEXTURE;
        static const sf::Texture WHITE_KING_TEXTURE;

        static const sf::Texture BLACK_PAWN_TEXTURE;
        static const sf::Texture BLACK_ROOK_TEXTURE;
        static const sf::Texture BLACK_KNIGHT_TEXTURE;
        static const sf::Texture BLACK_BISHOP_TEXTURE;
        static const sf::Texture BLACK_QUEEN_TEXTURE;
        static const sf::Texture BLACK_KING_TEXTURE;

    private:
        // Piece Info
        byte m_Descriptor; // Bits: n n E M / C T T T

        // Rendering
        sf::Sprite *p_Sprite;

    public:
        Piece(char symbol, sf::RectangleShape &tile);

        ~Piece()
        {
            delete p_Sprite;
        }

        // Methods

        void resizeSprite(sf::Vector2f tileSize)
        {
            p_Sprite->setScale(sf::Vector2f(tileSize.x / p_Sprite->getTexture().getSize().x, tileSize.y / p_Sprite->getTexture().getSize().y));
        }

        // Getters

        Color getColor() const
        {
            return static_cast<Color>(m_Descriptor & 0b00001000);
        }

        Type getType() const
        {
            return static_cast<Type>(m_Descriptor & 0b00000111);
        }

        bool hasMoved() const
        {
            return (this->m_Descriptor & 0b00010000) >> 4;
        }

        bool isEnPassantVulnerable() const
        {
            return (this->m_Descriptor & 0b00100000) >> 5;
        }

        unsigned int getValue() const
        {
            switch (getType())
            {
            case Piece::Type::Pawn:
                return 1;
            case Piece::Type::Knight:
                return 3;
            case Piece::Type::Bishop:
                return 3;
            case Piece::Type::Rook:
                return 5;
            case Piece::Type::Queen:
                return 9;
            default:
                return -1;
            }
        }

        char getSymbol() const
        {
            char symbol;
            switch (getType())
            {
            case Piece::Type::Pawn:
                symbol = 'p';
                break;
            case Piece::Type::Knight:
                symbol = 'n';
                break;
            case Piece::Type::Bishop:
                symbol = 'b';
                break;
            case Piece::Type::Rook:
                symbol = 'r';
                break;
            case Piece::Type::Queen:
                symbol = 'q';
                break;
            case Piece::Type::King:
                symbol = 'k';
                break;
            }

            if (getColor() == Piece::Color::White)
                symbol = std::toupper(symbol);

            return symbol;
        }

        // Setters

        void setMoved(bool moved)
        {
            if (moved)
            {
                m_Descriptor |= 0b00010000;
            }
            else
            {
                m_Descriptor &= 0b11101111;
            }
        }

        void setEnPassantVulnerable(bool vul)
        {
            if (vul)
            {
                m_Descriptor |= 0b00100000;
            }
            else
            {
                m_Descriptor &= 0b11011111;
            }
        }

        // Utilities

        std::string toString()
        {
            std::string result;

            result += (getColor() == White) ? "White " : "Black ";

            switch (getType())
            {
            case Pawn:
                result += "Pawn";
                break;
            case Rook:
                result += "Rook";
                break;
            case Knight:
                result += "Knight";
                break;
            case Bishop:
                result += "Bishop";
                break;
            case Queen:
                result += "Queen";
                break;
            case King:
                result += "King";
                break;
            }

            result += " (";
            result += ('0' + getValue());
            result += ")";

            return result;
        }

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    };
}
