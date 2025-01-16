#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <vector>
#include <array>
#include <stack>
#include <variant>

#include "Piece.h"
#include "Move.h"

constexpr auto STANDARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

namespace Chess
{
    class Game : public sf::Drawable
    {
    private:
        // Info
        std::array<Piece *, 64> m_Pieces;
        mutable std::string m_FenBuffer;

        Piece::Color m_CurrentTurn;
        int m_CurrentSelectedIndex;

        std::vector<int> m_PossibleMoves;
        std::stack<std::variant<Move, CastlingMove, EnPassantMove>> m_MovesHistory;

        unsigned int m_WhiteScore;
        unsigned int m_BlackScore;

        // Rendering
        sf::Color m_WhiteColor, m_BlackColor;

        float m_BoardSize;
        float m_GUIOffset;

        mutable sf::RectangleShape m_Tile;
        mutable sf::RectangleShape m_SelectedBox;
        mutable sf::RectangleShape m_PossibleMoveBox;

    public:
        Game(sf::RenderWindow &window, sf::Color whiteColor = sf::Color(0xf1d7c0ff), sf::Color blackColor = sf::Color(0xa97a65ff));

        ~Game()
        {
            for (auto piece : m_Pieces)
            {
                delete piece;
            }
        }

        // Methods

        void handleClick(sf::Vector2i mousePos);

        void prepareGUI();

        void resize(const sf::Vector2u newSize);

    private:
        void restart();

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

        Piece *currentSelectedPiece()
        {
            if (m_CurrentSelectedIndex != -1)
                return m_Pieces[m_CurrentSelectedIndex];
            return nullptr;
        }

        void registerMove(int from, int to);
        void registerCastlingMove(int kingFrom, int kingTo, int rookFrom, int rookTo);
        void registerEnPassantMove(int pawnFrom, int pawnTo, int capturedIndex);
        void undoLastMove();

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