#include "Chess.h"

void Chess::Game::calculatePossibleMoves(int index)
{
    m_PossibleMoves.clear();

    switch (m_Pieces[index]->getType())
    {
    case Piece::Type::Pawn:
        calculatePawnMoves(index);
        break;
    case Piece::Type::Rook:
        calculateRookMoves(index);
        break;
    case Piece::Type::Knight:
        calculateKnightMoves(index);
        break;
    case Piece::Type::Bishop:
        calculateBishopMoves(index);
        break;
    case Piece::Type::Queen:
        calculateQueenMoves(index);
        break;
    case Piece::Type::King:
        calculateKingMoves(index);
        break;
    default:
        std::cerr << "Error: Invalid piece selected at index " << index << std::endl;
        break;
    }
}

void Chess::Game::addMovesInDirection(int startFile, int startRank, int fileIncrement, int rankIncrement, Piece::Color pieceColor)
{
    int file = startFile + fileIncrement;
    int rank = startRank + rankIncrement;
    while (file >= 0 && file < 8 && rank >= 0 && rank < 8)
    {
        int targetIndex = fileRankToIndex(file, rank);
        if (m_Pieces[targetIndex] == nullptr)
        {
            m_PossibleMoves.push_back(targetIndex);
        }
        else
        {
            if (m_Pieces[targetIndex]->getColor() != pieceColor)
            {
                m_PossibleMoves.push_back(targetIndex);
            }
            break;
        }
        file += fileIncrement;
        rank += rankIncrement;
    }
}

void Chess::Game::calculatePawnMoves(int index)
{
    auto pawn = m_Pieces[index];
    int dir = pawn->getColor() == Piece::Color::White ? 1 : -1;

    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    // Move forward
    int forwardIndex = fileRankToIndex(startFile, startRank + dir);
    if (isValidIndex(forwardIndex) && m_Pieces[forwardIndex] == nullptr)
    {
        m_PossibleMoves.push_back(forwardIndex);

        // Move two squares forward from starting position
        if (!pawn->hasMoved())
        {
            int doubleForwardIndex = fileRankToIndex(startFile, startRank + dir * 2);
            if (isValidIndex(doubleForwardIndex) && m_Pieces[doubleForwardIndex] == nullptr)
            {
                m_PossibleMoves.push_back(doubleForwardIndex);
            }
        }
    }

    // En passant
    if (startRank == (pawn->getColor() == Piece::Color::White ? 4 : 3))
    {
        int leftFile = startFile - 1;
        int rightFile = startFile + 1;
        int enPassantRank = startRank + dir;

        if (leftFile >= 0 && m_Pieces[fileRankToIndex(leftFile, startRank)] != nullptr &&
            m_Pieces[fileRankToIndex(leftFile, startRank)]->getType() == Piece::Type::Pawn &&
            m_Pieces[fileRankToIndex(leftFile, startRank)]->getColor() != pawn->getColor() &&
            m_Pieces[fileRankToIndex(leftFile, startRank)]->isEnPassantVulnerable())
        {
            m_PossibleMoves.push_back(fileRankToIndex(leftFile, enPassantRank));
        }

        if (rightFile < 8 && m_Pieces[fileRankToIndex(rightFile, startRank)] != nullptr &&
            m_Pieces[fileRankToIndex(rightFile, startRank)]->getType() == Piece::Type::Pawn &&
            m_Pieces[fileRankToIndex(rightFile, startRank)]->getColor() != pawn->getColor() &&
            m_Pieces[fileRankToIndex(rightFile, startRank)]->isEnPassantVulnerable())
        {
            m_PossibleMoves.push_back(fileRankToIndex(rightFile, enPassantRank));
        }
    }

    // Capture diagonally
    int captureLeftIndex = fileRankToIndex(startFile - 1, startRank + dir);
    int captureRightIndex = fileRankToIndex(startFile + 1, startRank + dir);

    if (startFile > 0 && isValidIndex(captureLeftIndex) && m_Pieces[captureLeftIndex] != nullptr &&
        m_Pieces[captureLeftIndex]->getColor() != pawn->getColor())
    {
        m_PossibleMoves.push_back(captureLeftIndex);
    }

    if (startFile < 7 && isValidIndex(captureRightIndex) && m_Pieces[captureRightIndex] != nullptr &&
        m_Pieces[captureRightIndex]->getColor() != pawn->getColor())
    {
        m_PossibleMoves.push_back(captureRightIndex);
    }
}

void Chess::Game::calculateRookMoves(int index)
{
    auto rook = m_Pieces[index];
    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    // Rook moves horizontally and vertically
    addMovesInDirection(startFile, startRank, 1, 0, rook->getColor());  // Right
    addMovesInDirection(startFile, startRank, -1, 0, rook->getColor()); // Left
    addMovesInDirection(startFile, startRank, 0, 1, rook->getColor());  // Up
    addMovesInDirection(startFile, startRank, 0, -1, rook->getColor()); // Down
}

void Chess::Game::calculateKnightMoves(int index)
{
    auto knight = m_Pieces[index];
    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    std::array<std::pair<int, int>, 8> knightMoves = {{{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}}};

    for (const auto &move : knightMoves)
    {
        int file = startFile + move.first;
        int rank = startRank + move.second;
        if (file >= 0 && file < 8 && rank >= 0 && rank < 8)
        {
            int targetIndex = fileRankToIndex(file, rank);
            if (m_Pieces[targetIndex] == nullptr || m_Pieces[targetIndex]->getColor() != knight->getColor())
            {
                m_PossibleMoves.push_back(targetIndex);
            }
        }
    }
}

void Chess::Game::calculateBishopMoves(int index)
{
    auto bishop = m_Pieces[index];
    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    // Bishop moves diagonally
    addMovesInDirection(startFile, startRank, 1, 1, bishop->getColor());   // Up-Right
    addMovesInDirection(startFile, startRank, 1, -1, bishop->getColor());  // Down-Right
    addMovesInDirection(startFile, startRank, -1, 1, bishop->getColor());  // Up-Left
    addMovesInDirection(startFile, startRank, -1, -1, bishop->getColor()); // Down-Left
}

void Chess::Game::calculateQueenMoves(int index)
{
    auto queen = m_Pieces[index];
    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    // Queen moves horizontally, vertically, and diagonally
    addMovesInDirection(startFile, startRank, 1, 0, queen->getColor());   // Right
    addMovesInDirection(startFile, startRank, -1, 0, queen->getColor());  // Left
    addMovesInDirection(startFile, startRank, 0, 1, queen->getColor());   // Up
    addMovesInDirection(startFile, startRank, 0, -1, queen->getColor());  // Down
    addMovesInDirection(startFile, startRank, 1, 1, queen->getColor());   // Up-Right
    addMovesInDirection(startFile, startRank, 1, -1, queen->getColor());  // Down-Right
    addMovesInDirection(startFile, startRank, -1, 1, queen->getColor());  // Up-Left
    addMovesInDirection(startFile, startRank, -1, -1, queen->getColor()); // Down-Left
}

void Chess::Game::calculateKingMoves(int index)
{
    auto king = m_Pieces[index];
    int startFile = indexToFile(index);
    int startRank = indexToRank(index);

    std::array<std::pair<int, int>, 8> kingMoves = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

    for (const auto &move : kingMoves)
    {
        int file = startFile + move.first;
        int rank = startRank + move.second;
        if (file >= 0 && file < 8 && rank >= 0 && rank < 8)
        {
            int targetIndex = fileRankToIndex(file, rank);
            if (m_Pieces[targetIndex] == nullptr || m_Pieces[targetIndex]->getColor() != king->getColor())
            {
                m_PossibleMoves.push_back(targetIndex);
            }
        }
    }

    // Castling
    if (!king->hasMoved())
    {
        // Kingside castling
        if (m_Pieces[fileRankToIndex(7, startRank)] != nullptr &&
            m_Pieces[fileRankToIndex(7, startRank)]->getType() == Piece::Type::Rook &&
            !m_Pieces[fileRankToIndex(7, startRank)]->hasMoved())
        {
            bool pathClear = true;
            for (int i = startFile + 1; i < 7; ++i)
            {
                if (m_Pieces[fileRankToIndex(i, startRank)] != nullptr)
                {
                    pathClear = false;
                    break;
                }
            }
            if (pathClear)
            {
                m_PossibleMoves.push_back(fileRankToIndex(startFile + 2, startRank));
            }
        }

        // Queenside castling
        if (m_Pieces[fileRankToIndex(0, startRank)] != nullptr &&
            m_Pieces[fileRankToIndex(0, startRank)]->getType() == Piece::Type::Rook &&
            !m_Pieces[fileRankToIndex(0, startRank)]->hasMoved())
        {
            bool pathClear = true;
            for (int i = startFile - 1; i > 0; --i)
            {
                if (m_Pieces[fileRankToIndex(i, startRank)] != nullptr)
                {
                    pathClear = false;
                    break;
                }
            }
            if (pathClear)
            {
                m_PossibleMoves.push_back(fileRankToIndex(startFile - 2, startRank));
            }
        }
    }
}
