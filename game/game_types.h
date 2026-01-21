#pragma once

enum class GameMode {
Classic3x3 = 0,
Score10x10 = 1,
Ultimate = 2
};

enum class FillMode {
Free = 0,
TopDownRows = 1,
LeftRightCols = 2,
RandomRow = 3,
RandomCol = 4,
RandomRowOrCol = 5
};

enum class PlayerType {
Human = 0,
Computer = 1
};