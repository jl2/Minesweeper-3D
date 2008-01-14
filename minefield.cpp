/*
  minefield.cpp
 
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

#include <cstdlib>
#include <stdexcept>

#include "minefield.h"

/*!
  The constructor allocates the minefield and populates it with mines
  It also initializes all private variables.
*/
Minefield::Minefield(const size_t w, const size_t h, const size_t d,
		     const size_t n): wdth(w), hght(h), dpth(d),
				      num_bombs(n), num_cleared(0),
				      total_cells(w*h*d), num_marked(0), fake_marks(0), real_marks(0) {
  field = new mf_state_t[w*h*d];

  // Clear the field
  for (size_t i=0;i<w;++i) {
    for (size_t j=0;j<h;++j) {
      for (size_t k=0;k<d;++k) {
	state(i,j,k) = closed;
      }
    }
  }

  // Now populate it
  for (size_t bn = 0; bn<n;++bn) {
    size_t tx;
    size_t ty;
    size_t tz;

    // Find clear spot
    do {
      tx = rand()%w;
      ty = rand()%h;
      tz = rand()%d;
    } while (state(tx,ty,tz) == closed_bomb);
    
    state(tx,ty,tz) = closed_bomb;
  }
  
}

/*!
  Deallocates the minefield
*/
Minefield::~Minefield() {
  delete[] field;
}

/*!
  Returns the current state of the given cell.
*/
mf_state_t Minefield::getState(const size_t x, const size_t y, const size_t z) {
  return state(x,y,z);
}

/*!
  Returns the number of mines remaining.
*/
size_t Minefield::minesRemaining() {
  return (num_bombs-(fake_marks+real_marks));
}

/*!
  state() is used by the Minefield class to access individual mine cells.
  All access to the field array (except allocation/deallocation) should go through
  this function.
*/
inline mf_state_t &Minefield::state(const size_t x, const size_t y, const size_t z) {
  if (x<wdth && y<hght && z < dpth)
    return field[(wdth*hght*z)+wdth*y+x];
  else
    throw std::runtime_error("Invalid index");
}

/*!
  touch() should be called when the user clicks on a cell.
  The function recursively clears the clicked cell and the surrounding cells.
  If the cell is not a bomb, it's state is set to open.
  If the cell is not near any bombs, its neighbors on the top, bottom,
  front, back, left, and right are also touched.

  The function returns number of opened cells.
*/
size_t Minefield::touch(const size_t x, const size_t y, const size_t z) {
  
  // Check for invalid index
  if (x >= wdth || y >= hght || z >= dpth) {
    throw std::runtime_error("Invalid index");
  }

  // Check for a bomb
  if (state(x,y,z) != closed) return 0;

  // Valid index, and not a bomb, so open it
  size_t retVal = 0;
  state(x,y,z) = open;
  ++num_cleared;
  ++retVal;

  // Now try the neighbors
  size_t bn = bombsNear(x,y,z);

  if (bn==0) {
    if (dpth>(x+1))
      retVal += touch(x+1, y, z);
    
    if ((x-1)<dpth)
      retVal += touch(x-1,y,z);
    
    if (hght>(y+1))
      retVal += touch(x,y+1,z);
    
    if ((y-1)<hght)
      retVal += touch(x,y-1,z);
    
    if (dpth>(z+1))
      retVal += touch(x,y,z+1);
    
    if ((z-1)<dpth)
      retVal += touch(x,y,z-1);
  }

  return retVal;
}

/*!
  bombsNear() returns the number of bombs near the given cell.
*/
size_t Minefield::bombsNear(const size_t x, const size_t y, const size_t z) {

  // (x_min, y_min, z_min) is the minimum cell index to check
  // (x_max, y_max, z_max is the maximum cell index to check
  size_t x_min = x-1;
  size_t x_max = x+1;

  if (x_min>x) x_min = x;
  if (x_max>=wdth) x_max = x;

  size_t y_min = y-1;
  size_t y_max = y+1;
  if (y_min>y) y_min = y;
  if (y_max>=hght) y_max = y;
  
  size_t z_min = z-1;
  size_t z_max = z+1;
  if (z_min>z) z_min = z;
  if (z_max>=dpth) z_max = z;
  
  size_t numBombs = 0;

  // Loop from minimum to maximum, check for bombs
  for (size_t i=x_min; i<=x_max;++i) {
    for (size_t j=y_min; j<=y_max; ++j) {
      for (size_t k=z_min; k<=z_max; ++k) {
	if (state(i,j,k)==closed_bomb ||
	    state(i,j,k)==marked_bomb) {
	  ++numBombs;
	}
      }
    }
  }
  return numBombs;
}

/*!
  hasWon() returns true when the game has been won.
*/
bool Minefield::hasWon() {
  // A "win" occurs when all bombs are marked and there are no closed cells
  
  //if (real_marks == num_bombs) return true;
  if (num_cleared == (total_cells-num_bombs)) return true;
  return false;
}

/*!
  mark() marks a cell as a bomb
*/

void Minefield::mark(const size_t x, const size_t y, const size_t z) {
  // Check for valid index
  if (x >= wdth || y >= hght || z >= dpth) {
    throw std::runtime_error("Invalid index");
  }

  // New state depends on existing state...
  mf_state_t cs = state(x,y,z);
  switch (cs) {
  case closed:
    state(x,y,z) = marked_empty;
    ++fake_marks;
    break;
  case closed_bomb:
    state(x,y,z) = marked_bomb;
    ++real_marks;
    break;
  case marked_empty:
    state(x,y,z) = closed;
    --fake_marks;
    break;
  case marked_bomb:
    state(x,y,z) = closed_bomb;
    --real_marks;
    break;
  default:
    break;
  }
}

