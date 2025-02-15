#include "Chess.h"

Chess::Game::Game(sf::RenderWindow &window, sf::Color whiteColor, sf::Color blackColor)
    : m_Pieces(), m_FenBuffer(STANDARD_FEN),
      m_WhiteColor(whiteColor), m_BlackColor(blackColor),
      m_WhiteScore(0), m_BlackScore(0)
{

    m_BoardSize = window.getSize().y;
    m_GUIOffset = window.getSize().x - m_BoardSize;
    float tileSize = m_BoardSize / 8.0f;

    m_Tile = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));

    m_SelectedBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_SelectedBox.setFillColor(sf::Color::Transparent);
    m_SelectedBox.setOutlineColor(sf::Color::Red);
    m_SelectedBox.setOutlineThickness(2.0f);

    m_PossibleMoveBox = sf::RectangleShape(sf::Vector2f(tileSize, tileSize));
    m_PossibleMoveBox.setFillColor(sf::Color(0x00ff0055));

    restart();
}

void Chess::Game::resize(const sf::Vector2u newSize)
{
    m_BoardSize = newSize.y;
    m_GUIOffset = newSize.x - m_BoardSize;
    float tileSize = m_BoardSize / 8.0f;

    m_Tile.setSize(sf::Vector2f(tileSize, tileSize));
    m_SelectedBox.setSize(sf::Vector2f(tileSize, tileSize));
    m_PossibleMoveBox.setSize(sf::Vector2f(tileSize, tileSize));

    for (auto piece : m_Pieces)
    {
        if (piece != nullptr)
            piece->resizeSprite(m_Tile.getSize());
    }
}

void Chess::Game::restart()
{
    // Clearing
    for (int i = 0; i < 64; i++)
    {
        if (m_Pieces[i] != nullptr)
        {
            delete m_Pieces[i];
            m_Pieces[i] = nullptr;
        }
    }

    // Updating board by parsing FEN string
    auto index = 0;
    for (auto symbol : m_FenBuffer)
    {
        if (symbol == '/')
            continue;

        if (symbol == ' ')
            index = 64;

        if (index < 64)
        {
            // Placing pieces
            if (std::isdigit(symbol))
            {
                index += symbol - '0';
            }
            else
            {
                auto posIndex = (7 - index / 8) * 8 + index % 8;
                m_Pieces[posIndex] = new Piece(symbol, m_Tile);
                index++;
            }
        }
        else
        {
            // Parsing game info
            if (symbol == 'w')
            {
                m_CurrentTurn = Piece::Color::White;
            }
            else if (symbol == 'b')
            {
                m_CurrentTurn = Piece::Color::Black;
            }

            // TODO:
        }
    }

    // Resetting game
    m_CurrentSelectedIndex = -1;
    m_WhiteScore = 0;
    m_BlackScore = 0;
}

void Chess::Game::handleClick(sf::Vector2i mousePos)
{
    if (mousePos.x <= m_GUIOffset)
        return;

    int file = (mousePos.x - m_GUIOffset) / static_cast<int>(m_Tile.getSize().x);
    int rank = 7 - (mousePos.y / static_cast<int>(m_Tile.getSize().y));
    int targetIndex = rank * 8 + file;

    Piece *targetPiece = m_Pieces[targetIndex];

    if (currentSelectedPiece() == nullptr)
    {
        // No piece is currently selected
        if (targetPiece != nullptr && targetPiece->getColor() == m_CurrentTurn)
        {
            calculatePossibleMoves(targetIndex);
            m_CurrentSelectedIndex = targetIndex; // Selecting...
        }
    }
    else
    {
        // There's a selected piece
        if (targetPiece == nullptr)
        {
            // Moving to an empty spot
            if (std::find(m_PossibleMoves.begin(), m_PossibleMoves.end(), targetIndex) == m_PossibleMoves.end())
            {
                return; // Target index is not a valid move
            }

            // Check for castling
            if (currentSelectedPiece()->getType() == Piece::Type::King && !currentSelectedPiece()->hasMoved())
            {
                int fileDiff = targetIndex % 8 - m_CurrentSelectedIndex % 8;

                if (std::abs(fileDiff) == 2)
                {
                    // Castling move
                    int rookStartIndex = (fileDiff > 0) ? m_CurrentSelectedIndex + 3 : m_CurrentSelectedIndex - 4;
                    int rookEndIndex = (fileDiff > 0) ? m_CurrentSelectedIndex + 1 : m_CurrentSelectedIndex - 1;

                    registerCastlingMove(m_CurrentSelectedIndex, targetIndex, rookStartIndex, rookEndIndex);
                    m_CurrentSelectedIndex = -1;
                    return;
                }
            }
            else
            {
                // Check for en passant
                if (currentSelectedPiece()->getType() == Piece::Type::Pawn)
                {
                    int startRank = indexToRank(m_CurrentSelectedIndex);
                    int targetRank = indexToRank(targetIndex);

                    if (std::abs(startRank - targetRank) == 2)
                    {
                        currentSelectedPiece()->setEnPassantVulnerable(true);
                    }
                    else
                    {
                        currentSelectedPiece()->setEnPassantVulnerable(false);
                    }

                    // Capture en passant
                    int enPassantTarget = targetIndex + (m_CurrentTurn == Piece::Color::White ? -8 : 8);
                    if (targetPiece == nullptr && std::find(m_PossibleMoves.begin(), m_PossibleMoves.end(), targetIndex) != m_PossibleMoves.end() && m_Pieces[enPassantTarget] != nullptr && m_Pieces[enPassantTarget]->getType() == Piece::Type::Pawn && m_Pieces[enPassantTarget]->isEnPassantVulnerable())
                    {
                        registerEnPassantMove(m_CurrentSelectedIndex, targetIndex, enPassantTarget);
                        m_CurrentSelectedIndex = -1;
                        return;
                    }
                }
            }

            registerMove(m_CurrentSelectedIndex, targetIndex);
            m_CurrentSelectedIndex = -1;
        }
        else
        {
            // Selecting an occupied spot
            if (targetPiece->getColor() != currentSelectedPiece()->getColor())
            {
                // Different color: capturing
                if (std::find(m_PossibleMoves.begin(), m_PossibleMoves.end(), targetIndex) == m_PossibleMoves.end())
                {
                    return; // Target index is not a valid move
                }

                registerMove(m_CurrentSelectedIndex, targetIndex);
                m_CurrentSelectedIndex = -1;
            }
            else
            {
                // Same color: switching piece
                calculatePossibleMoves(targetIndex);
                m_CurrentSelectedIndex = targetIndex;
            }
        }
    }
}

void Chess::Game::registerMove(int from, int to)
{
    if (from < 0 || from >= 64 || to < 0 || to >= 64)
        return;

    if (m_Pieces[from] == nullptr)
        return;

    char pieceSymbol = m_Pieces[from]->getSymbol();
    char capturedSymbol = 'x';

    // Check for capture
    if (m_Pieces[to] != nullptr)
    {
        // Update scores
        if (m_CurrentTurn == Piece::Color::White)
        {
            m_WhiteScore += m_Pieces[to]->getValue();
        }
        else
        {
            m_BlackScore += m_Pieces[to]->getValue();
        }

        // Update symbol
        capturedSymbol = m_Pieces[to]->getSymbol();

        // Remove
        delete m_Pieces[to];
        m_Pieces[to] = nullptr;
    }

    // Move
    m_Pieces[from]->setMoved(true);
    std::swap(m_Pieces[from], m_Pieces[to]);

    // Switch turn
    m_CurrentTurn = m_CurrentTurn == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;

    // Register movement
    m_MovesHistory.push(Move{from, to, pieceSymbol, capturedSymbol});
}

void Chess::Game::registerCastlingMove(int kingFrom, int kingTo, int rookFrom, int rookTo)
{
    if (kingFrom < 0 || kingFrom >= 64 || kingTo < 0 || kingTo >= 64 || rookFrom < 0 || rookFrom >= 64 || rookTo < 0 || rookTo >= 64)
        return;

    if (m_Pieces[kingFrom] == nullptr || m_Pieces[rookFrom] == nullptr)
        return;

    char kingSymbol = m_Pieces[kingFrom]->getSymbol();
    char rookSymbol = m_Pieces[rookFrom]->getSymbol();

    // Move the king
    m_Pieces[kingFrom]->setMoved(true);
    std::swap(m_Pieces[kingFrom], m_Pieces[kingTo]);

    // Move the rook
    m_Pieces[rookFrom]->setMoved(true);
    std::swap(m_Pieces[rookFrom], m_Pieces[rookTo]);

    // Switch turn
    m_CurrentTurn = m_CurrentTurn == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;

    // Register movement
    m_MovesHistory.push(CastlingMove{kingFrom, kingTo, kingSymbol, rookSymbol, rookFrom, rookTo});
}

void Chess::Game::registerEnPassantMove(int pawnFrom, int pawnTo, int capturedIndex)
{
    if (pawnFrom < 0 || pawnFrom >= 64 || pawnTo < 0 || pawnTo >= 64 || capturedIndex < 0 || capturedIndex >= 64)
        return;

    if (m_Pieces[pawnFrom] == nullptr || m_Pieces[capturedIndex] == nullptr)
        return;

    char pieceSymbol = m_Pieces[pawnFrom]->getSymbol();
    char capturedSymbol = m_Pieces[capturedIndex]->getSymbol();

    // Move the pawn
    m_Pieces[pawnFrom]->setMoved(true);
    std::swap(m_Pieces[pawnFrom], m_Pieces[pawnTo]);

    // Capture the pawn
    delete m_Pieces[capturedIndex];
    m_Pieces[capturedIndex] = nullptr;

    // Switch turn
    m_CurrentTurn = m_CurrentTurn == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;

    // Register movement
    m_MovesHistory.push(EnPassantMove{pawnFrom, pawnTo, pieceSymbol, capturedSymbol, capturedIndex});
}

void Chess::Game::undoLastMove()
{
    if (m_MovesHistory.size() == 0)
        return;

    std::variant<Move, CastlingMove, EnPassantMove> lastMove = m_MovesHistory.top();

    if (std::holds_alternative<Move>(lastMove))
    {
        auto move = std::get<Move>(lastMove);
        std::swap(m_Pieces[move.to], m_Pieces[move.from]);
        if (move.otherPieceSymbol != 'x')
        {
            m_Pieces[move.to] = new Piece(move.otherPieceSymbol, m_Tile);
            if (m_CurrentTurn == Piece::Color::White)
            {
                m_BlackScore -= m_Pieces[move.to]->getValue();
            }
            else
            {
                m_WhiteScore -= m_Pieces[move.to]->getValue();
            }
        }
        m_Pieces[move.from]->setMoved(false);
    }
    else if (std::holds_alternative<CastlingMove>(lastMove))
    {
        auto move = std::get<CastlingMove>(lastMove);
        std::swap(m_Pieces[move.to], m_Pieces[move.from]);
        std::swap(m_Pieces[move.rookTo], m_Pieces[move.rookFrom]);
        m_Pieces[move.from]->setMoved(false);
        m_Pieces[move.rookFrom]->setMoved(false);
    }
    else if (std::holds_alternative<EnPassantMove>(lastMove))
    {
        auto move = std::get<EnPassantMove>(lastMove);
        std::swap(m_Pieces[move.to], m_Pieces[move.from]);
        m_Pieces[move.enPassantTarget] = new Piece(move.otherPieceSymbol, m_Tile);
        if (m_CurrentTurn == Piece::Color::White)
        {
            m_BlackScore -= m_Pieces[move.enPassantTarget]->getValue();
        }
        else
        {
            m_WhiteScore -= m_Pieces[move.enPassantTarget]->getValue();
        }
        m_Pieces[move.from]->setMoved(false);
    }

    m_MovesHistory.pop();
    m_CurrentTurn = m_CurrentTurn == Piece::Color::White ? Piece::Color::Black : Piece::Color::White;
}

void Chess::Game::prepareGUI()
{
    // Preparing UI
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_GUIOffset, m_BoardSize));

    ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // RESET
    ImGui::TextColored(ImColor(255, 255, 128), "Reset");
    if (ImGui::Button("Reset"))
    {
        m_FenBuffer = STANDARD_FEN;
        restart();
    };
    if (ImGui::Button("Undo"))
    {
        undoLastMove();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // RESET WITH FEN
    ImGui::TextColored(ImColor(255, 255, 128), "Restart with FEN");
    ImGui::InputText("FEN", m_FenBuffer.data(), m_FenBuffer.size());
    if (ImGui::Button("Restart"))
    {
        // TODO: Validate
        restart();
    };
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // INFORMATIONS
    ImGui::TextColored(ImColor(255, 255, 128), "Informations:");
    ImGui::Text("Turn: %s", m_CurrentTurn == Chess::Piece::Color::White ? "White" : "Black");
    ImGui::TextColored(ImColor(255, 255, 128), "Scores:");
    ImGui::Text("White: %u", m_WhiteScore);
    ImGui::Text("Black: %u", m_BlackScore);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // DEBUG
    ImGui::TextColored(ImColor(255, 255, 128), "Debug:");
    if (ImGui::Button("Print moves history"))
    {
        auto tempStack = m_MovesHistory;

        std::cout << "Moves history:" << std::endl;
        while (!tempStack.empty())
        {
            auto move = tempStack.top();
            tempStack.pop();

            if (std::holds_alternative<Move>(move))
            {
                auto m = std::get<Move>(move);
                std::cout << "Move from " << m.from << " to " << m.to << " with piece " << m.movedPieceSymbol << " capturing " << m.otherPieceSymbol << std::endl;
            }
            else if (std::holds_alternative<CastlingMove>(move))
            {
                auto m = std::get<CastlingMove>(move);
                std::cout << "Castling move: King from " << m.from << " to " << m.to << " and Rook from " << m.rookFrom << " to " << m.rookTo << std::endl;
            }
            else if (std::holds_alternative<EnPassantMove>(move))
            {
                auto m = std::get<EnPassantMove>(move);
                std::cout << "En Passant move: Pawn from " << m.from << " to " << m.to << " capturing at " << m.enPassantTarget << std::endl;
            }
        }
        std::cout << std::endl;
    }

    ImGui::End();
}

void Chess::Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform.translate(sf::Vector2f(m_GUIOffset, 0));

    // Rendering Board
    for (int i = 0; i < 64; i++)
    {
        int file = indexToFile(i);
        int rank = indexToRank(i);

        // Drawing tile
        m_Tile.setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
        m_Tile.setFillColor((file + rank) % 2 == 0 ? m_BlackColor : m_WhiteColor);
        target.draw(m_Tile, states);

        // Drawing piece
        auto piece = m_Pieces[i];
        if (piece != nullptr)
        {
            piece->setPosition(sf::Vector2f(file * m_Tile.getSize().x, (7 - rank) * m_Tile.getSize().y));
            target.draw(*piece, states);
        }
    }

    // Drawing overlays
    if (m_CurrentSelectedIndex != -1)
    {
        int file = indexToFile(m_CurrentSelectedIndex);
        int rank = indexToRank(m_CurrentSelectedIndex);

        // Selected overlay
        m_SelectedBox.setPosition(sf::Vector2f(file * m_SelectedBox.getSize().x, (7 - rank) * m_SelectedBox.getSize().y));
        target.draw(m_SelectedBox, states);

        // Movements overlays
        for (auto movePos : m_PossibleMoves)
        {
            int moveFile = indexToFile(movePos);
            int moveRank = indexToRank(movePos);

            m_PossibleMoveBox.setPosition(sf::Vector2f(moveFile * m_PossibleMoveBox.getSize().x, (7 - moveRank) * m_PossibleMoveBox.getSize().y));
            target.draw(m_PossibleMoveBox, states);
        }
    }
}
