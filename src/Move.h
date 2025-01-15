#pragma once

#include "Piece.h"

namespace Chess
{
    struct Move
    {
        int from;
        int to;

        char movedPieceDescriptor;
        char capturedPieceDescriptor;
    };

    struct CastlingMove : public Move
    {
        int rookFrom;
        int rookTo;
    };

    struct EnPassantMove : public Move
    {
        int enPassantTarget;
    };
}