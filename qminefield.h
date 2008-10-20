/*
  qminefield.h
 
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

#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>

#include <GL/gl.h>
#include <GL/glu.h>

#include "minefield.h"

// Some constants...
static const size_t NUM_MATERIALS=4;
static const size_t NUM_LIGHTS=1;
static const size_t NUM_TEXTURES=27;
static const size_t LINE_MAT=0;
static const size_t FILLED_BOX_MAT=1;
static const size_t MARKED_BOX_MAT=2;
static const size_t NUMBER_BOX_MAT=3;

static const size_t GREY_BOX_DL=27;
static const size_t RED_BOX_DL=28;
static const size_t NUM_LISTS=29;

/*!
  QMinefield is the QT widget that displays a minefield and lets the user play Minesweeper 3D
*/
class QMinefield : public QGLWidget {
  Q_OBJECT;

 public:
  QMinefield(QWidget *parent = 0);
  ~QMinefield();
  
  void startNewGame(size_t w, size_t h, size_t d, size_t n);

  void resetView();
  
 signals:
  // gameLost() is emitted when the game is lost
  void gameLost();
  // gameWon() is emitted when the game is won
  void gameWon();
  // bombMarked() is emitted when a bomb is marked
  void bombMarked(int num_bombs);

  void firstClick();
  
 protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
  
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

 private:
  // Draws the whole mine
  void drawMine();
  
  // Initializes a display list for a box with the given material
  void drawBoxList(size_t mat_idx);
  
  // Initializes a display list for a numbered box 
  void drawNumberBoxList(size_t tn);
  
  // Draws a cell at the given position
  void drawCell(size_t i, size_t j, size_t k);

  // Returns a unique integer for the cell at x,y,z
  size_t encodeBox(size_t x, size_t y, size_t z);

  // Converts a unique integer to a cell index
  void decodeBox(size_t val, size_t &x, size_t &y, size_t &z);

  // Returns the "name" of the cell at pos
  int nameAtPos(const QPoint &pos);

  // Initialization functions
  void initMaterials();
  void initLights();
  void initLists();
  void loadTextMaps();

  // Error handler for OpenGL errors
  void handleGLError(size_t ln);
  
  // Arrays to hold material properties
  GLfloat mat_specular[NUM_MATERIALS][4];
  GLfloat mat_shininess[NUM_MATERIALS][1];
  GLfloat mat_diffuse[NUM_MATERIALS][4];
  GLfloat mat_ambient[NUM_MATERIALS][4];

  // Arrays to hold light properties
  GLfloat light_position[NUM_LIGHTS][4];
  GLfloat light_color[NUM_LIGHTS][4];
  GLfloat lmodel_ambient[NUM_LIGHTS][4];


  // Arrays to hold textures
  QImage textures[NUM_TEXTURES];
  GLuint textNames[NUM_TEXTURES];

  // Array of display lists
  GLuint dispLists[NUM_LISTS];

  // The actual minefield
  Minefield *mf;

  // Stores last mouse position for rotation
  QPoint lastPos;

  // Rotation angles
  GLfloat rotationX;
  GLfloat rotationY;
  GLfloat rotationZ;

  // Zoom translation
  GLfloat translate;

  // Set to true when the game is lost
  bool lost;

  bool clicked;
};
