/*
  minefield.h
 
  Copyright (C) 2008 Jeremiah LaRocco

  This file is part of Minesweeper3D

  Minesweeper3D is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Minesweeper3D is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Minesweeper3D.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <cstddef>

// Possible states that a cell can be in
enum mf_state_t {open, closed, closed_bomb, marked_empty, marked_bomb};


class Minefield {
 public:
  
  Minefield(const size_t w=10, const size_t h=10, const size_t d=10, const size_t n=50);

  ~Minefield();

  // touch is called when a cell is clicked on.
  size_t touch(const size_t x, const size_t y, const size_t z);

  // Returns the state of a cell
  mf_state_t getState(const size_t x, const size_t y, const size_t z);

  // Obvious...
  size_t width() const { return wdth; }
  size_t height() const { return hght; }
  size_t depth() const { return dpth; }

  // Returns true when the user has marked all bombs or cleared all empty cells
  bool hasWon();

  // Marks a cell as being a bomb
  void mark(const size_t x, const size_t y, const size_t z);

  // Returns the number of bombs near a cell
  size_t bombsNear(const size_t x, const size_t y, const size_t z);

  // Returns the number of unmarked bombs
  size_t minesRemaining();
  
 protected:
  // Used internally to get/set states
  mf_state_t &state(const size_t x, const size_t y, const size_t z);
  
 private:
  // The array of cells
  mf_state_t *field;

  
  size_t wdth;
  size_t hght;
  size_t dpth;
  size_t num_bombs;

  // These are used to keep track of game status to determine winning/losing
  size_t num_cleared;
  size_t total_cells;
  size_t num_marked;
  size_t fake_marks;
  size_t real_marks;
};


#endif
