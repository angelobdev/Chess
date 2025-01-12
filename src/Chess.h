#pragma once

#include <array>
#include <SFML/Graphics.hpp>

#include "Piece.h"

#define STANDARD_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"

namespace Chess
{
    class Game : public sf::Drawable
    {
    private:
        // Info
        std::array<Piece *, 64> m_Pieces;

        Piece::Color m_CurrentTurn;
        int m_CurrentSelectedIndex;

        std::vector<int> m_Movements;

        unsigned int m_WhiteScore;
        unsigned int m_BlackScore;

        // Rendering
        sf::Color m_WhiteColor, m_BlackColor;
        mutable sf::RectangleShape m_Tile;
        mutable sf::RectangleShape m_SelectedBox;
        mutable sf::RectangleShape m_PossibleMoveBox;

    public:
        Game(sf::RenderWindow &window, sf::String &fen, sf::Color whiteColor = sf::Color(0xf1d7c0ff), sf::Color blackColor = sf::Color(0xa97a65ff));

        ~Game()
        {
            for (auto piece : m_Pieces)
            {
                delete piece;
            }
        }

        // Methods

        void restart(sf::String &fen);

        void handleClick(sf::Vector2i mousePos);

        // Getters

        Piece::Color getCurrentTurn() const
        {
            return m_CurrentTurn;
        }

        unsigned int getWhiteScore() const
        {
            return m_WhiteScore;
        }

        unsigned int getBlackScore() const
        {
            return m_BlackScore;
        }

    private:
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        void switchTurn()
        {
            m_CurrentTurn = m_CurrentTurn == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;
        }

        Piece *currentSelectedPiece()
        {
            if (m_CurrentSelectedIndex != -1)
                return m_Pieces[m_CurrentSelectedIndex];
            return nullptr;
        }

        void swapPieces(int from, int to)
        {
            if (from < 0 || from >= 64 || to < 0 || to >= 64)
                return;

            std::swap(m_Pieces[from], m_Pieces[to]);
        }

        void calculatePossibleMoves(int index);

        void addMovesInDirection(int startFile, int startRank, int fileIncrement, int rankIncrement, Piece::Color pieceColor);

        void calculatePawnMoves(int index);
        void calculateRookMoves(int index);
        void calculateKnightMoves(int index);
        void calculateBishopMoves(int index);
        void calculateQueenMoves(int index);
        void calculateKingMoves(int index);

    public:
        // Utilities

        static bool isValidIndex(int index)
        {
            return index >= 0 && index < 64;
        }

        static std::string indexToFRString(int index)
        {
            std::string res = "";

            auto file = indexToFile(index);
            auto rank = indexToRank(index);

            res += 'a' + file;
            res += '1' + rank;

            return res;
        }

        static int indexToFile(int index)
        {
            return index % 8;
        }

        static int indexToRank(int index)
        {
            return index / 8;
        }

        static int fileRankToIndex(int file, int rank)
        {
            return rank * 8 + file;
        }
    };
}