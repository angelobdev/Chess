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

        // Pieces Values
        static const byte PAWN_VALUE = 0b00010000;          // 1 point
        static const byte KNIGHT_BISHOP_VALUE = 0b00110000; // 3 points
        static const byte ROOK_VALUE = 0b01010000;          // 5 points
        static const byte QUEEN_VALUE = 0b10010000;         // 9 points
        static const byte KING_VALUE = 0b11110000;          // Invaluable

    private:
        // Piece Info
        byte m_Descriptor; // Bits: V V V V / C T T T
        bool m_HasMoved;

        // Rendering
        sf::Sprite *p_Sprite;

    public:
        Piece(char symbol, sf::RectangleShape &tile);

        ~Piece()
        {
            delete p_Sprite;
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
            return this->m_HasMoved;
        }

        unsigned int getValue() const
        {
            return static_cast<unsigned int>(this->m_Descriptor >> 4);
        }

        // Setters

        void setMoved(bool moved)
        {
            this->m_HasMoved = moved;
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
