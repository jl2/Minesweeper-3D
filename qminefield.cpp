/*
  qminefield.cpp
 
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

#include <QMainWindow>

#include <sstream>
#include <stdexcept>

#include "qminefield.h"

/*!
  Initializes the object and sets the OpenGL format.
*/
QMinefield::QMinefield(QWidget*) : mf(0), rotationX(0.0), rotationY(0.0),
				   rotationZ(0.0), translate(10.0), lost(false) {
  setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
}

/*!
  Frees memory for the minefield and cleans up OpenGL state
*/
QMinefield::~QMinefield() {
  for (size_t i=1;i<NUM_LISTS; ++i) {
    glDeleteLists(dispLists[i], 1);
  }

  if (mf)
    delete mf;

  glDeleteTextures(NUM_TEXTURES, textNames);
}

/*!
  Creates a new Minefield and resest the view
*/
void QMinefield::startNewGame(size_t w, size_t h, size_t d, size_t n) {
  if (mf)
    delete mf;
  clicked = false;
  lost = false;
  mf = new Minefield(w,h,d,n);
  resetView();
  //  updateGL();
}

/*!
  Loads the material arrays
*/
void QMinefield::initMaterials() {
  // lines
  mat_specular[LINE_MAT][0]=0.0;
  mat_specular[LINE_MAT][1]=0.0;
  mat_specular[LINE_MAT][2]=0.0;
  mat_specular[LINE_MAT][3]=1.0;
  
  mat_shininess[LINE_MAT][0]=80.0;

  mat_diffuse[LINE_MAT][0]=0.0;
  mat_diffuse[LINE_MAT][1]=0.0;
  mat_diffuse[LINE_MAT][2]=0.0;
  mat_diffuse[LINE_MAT][3]=1.0;
  
  mat_ambient[LINE_MAT][0] = 0.0;
  mat_ambient[LINE_MAT][1] = 0.0;
  mat_ambient[LINE_MAT][2] = 0.0;
  mat_ambient[LINE_MAT][3] = 1.0;

  // closed spots
  mat_specular[FILLED_BOX_MAT][0]=1.0;
  mat_specular[FILLED_BOX_MAT][1]=0.125;
  mat_specular[FILLED_BOX_MAT][2]=0.125;
  mat_specular[FILLED_BOX_MAT][3]=1.0;

  mat_shininess[FILLED_BOX_MAT][0]=100.0;
  
  mat_diffuse[FILLED_BOX_MAT][0]=0.5;
  mat_diffuse[FILLED_BOX_MAT][1]=0.5;
  mat_diffuse[FILLED_BOX_MAT][2]=0.5;
  mat_diffuse[FILLED_BOX_MAT][3]=1.0;
  
  mat_ambient[FILLED_BOX_MAT][0] = 0.130;
  mat_ambient[FILLED_BOX_MAT][1] = 0.130;
  mat_ambient[FILLED_BOX_MAT][2] = 0.130;
  mat_ambient[FILLED_BOX_MAT][3] = 1.0;

  // marked bombs
  mat_specular[MARKED_BOX_MAT][0]=0.125;
  mat_specular[MARKED_BOX_MAT][1]=0.125;
  mat_specular[MARKED_BOX_MAT][2]=0.125;
  mat_specular[MARKED_BOX_MAT][3]=1.0;
  
  mat_shininess[MARKED_BOX_MAT][0]=5.0;

  mat_diffuse[MARKED_BOX_MAT][0]=1.0;
  mat_diffuse[MARKED_BOX_MAT][1]=0.0;
  mat_diffuse[MARKED_BOX_MAT][2]=0.0;
  mat_diffuse[MARKED_BOX_MAT][3]=1.0;
  
  mat_ambient[MARKED_BOX_MAT][0] = 0.25;
  mat_ambient[MARKED_BOX_MAT][1] = 0.0;
  mat_ambient[MARKED_BOX_MAT][2] = 0.0;
  mat_ambient[MARKED_BOX_MAT][3] = 1.0;
  
  // # boxes
  mat_specular[NUMBER_BOX_MAT][0]=1.0;
  mat_specular[NUMBER_BOX_MAT][1]=1.0;
  mat_specular[NUMBER_BOX_MAT][2]=1.0;
  mat_specular[NUMBER_BOX_MAT][3]=0.25;
  
  mat_shininess[NUMBER_BOX_MAT][0]=25.0;

  mat_diffuse[NUMBER_BOX_MAT][0]=1.0;
  mat_diffuse[NUMBER_BOX_MAT][1]=1.0;
  mat_diffuse[NUMBER_BOX_MAT][2]=1.0;
  mat_diffuse[NUMBER_BOX_MAT][3]=0.75;
  
  mat_ambient[NUMBER_BOX_MAT][0] = 0.25;
  mat_ambient[NUMBER_BOX_MAT][1] = 0.25;
  mat_ambient[NUMBER_BOX_MAT][2] = 0.25;
  mat_ambient[NUMBER_BOX_MAT][3] = 1.0;
}

/*!
  Initializes the light arrays
*/
void QMinefield::initLights() {
  light_position[0][0]=0.0;
  light_position[0][1]=0.0;
  light_position[0][2]=30.0;
  light_position[0][3]=1.0;
  
  light_position[1][0]=0.0;
  light_position[1][1]=0.0;
  light_position[1][2]=-30.0;
  light_position[1][3]=1.0;
  
  for (size_t i=0;i<NUM_LIGHTS; ++i) {
    light_color[i][0]=1.0;
    light_color[i][1]=1.0;
    light_color[i][2]=1.0;
    light_color[i][3]=1.0;
    lmodel_ambient[i][0]=0.4;
    lmodel_ambient[i][1]=0.4;
    lmodel_ambient[i][2]=0.4;
    lmodel_ambient[i][3]=1.0;
  }

  glEnable(GL_LIGHTING);
    
  glEnable(GL_LIGHT0);
    
//   glEnable(GL_LIGHT1);
}

/*!
  Initializes the display lists
*/
void QMinefield::initLists() {
  // Numbered cubes
  for (int i=1;i<=26; ++i) {
    dispLists[i] = glGenLists(1);
    if (dispLists[i]!=0) {
      glNewList(dispLists[i], GL_COMPILE);
      drawNumberBoxList(i);
      glEndList();
    }
  }

  // The grey box used for closed cells
  dispLists[GREY_BOX_DL] = glGenLists(1);
  if (dispLists[GREY_BOX_DL]!=0) {
    glNewList(dispLists[GREY_BOX_DL], GL_COMPILE);
    drawBoxList(FILLED_BOX_MAT);
    glEndList();
  }

  // Marked cells
  dispLists[RED_BOX_DL] = glGenLists(1);
  if (dispLists[RED_BOX_DL]!=0) {
    glNewList(dispLists[RED_BOX_DL], GL_COMPILE);
    drawBoxList(MARKED_BOX_MAT);
    glEndList();
  }
}

/*!
  Load the numbered images used as textures.
  Eventually all boxes will be drawn with a texture map,
  which will (greatly) simplify the drawing code.
*/
void QMinefield::loadTextMaps() {

  // texture holds the raw data
  GLubyte texture[NUM_TEXTURES][128][128][4];


  // Generate OpenGL textures
  glGenTextures(NUM_TEXTURES, textNames);
    
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  // Loop through all the textures
  for (size_t i=1;i<NUM_TEXTURES;++i) {
    // Get the file name
    std::ostringstream fname;
    fname << ":/images/" << i << ".png";
    // Load the texture
    if (textures[i].load(QString(fname.str().c_str()), 0)) {
      // Now fill the raw data array
      QRgb color;
      for (int j=0;j<128;++j) {
	for (int k=0;k<128;++k) {
	  color=textures[i].pixel(j,k);
	  texture[i][j][k][0] = qRed(color);
	  texture[i][j][k][1] = qGreen(color);
	  texture[i][j][k][2] = qBlue(color);
	  texture[i][j][k][3] = qAlpha(color);
	}
      }

      // Now bind the texture in OpenGL
      glBindTexture(GL_TEXTURE_2D, textNames[i]);
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      
      // Make the texture usable
      glTexImage2D(GL_TEXTURE_2D, 0,
		   GL_RGBA, 128,128, 0,
		   GL_RGBA, GL_UNSIGNED_BYTE,
		   texture[i]);
    }
  }
}

/*!
  Initializes OpenGL by enabling required features and loading materials/lights/display lists
*/
void QMinefield::initializeGL() {
  // Enable stuff
  qglClearColor(Qt::white);
  
  glShadeModel(GL_FLAT);
    
  glPolygonMode(GL_FRONT, GL_FILL);
    
  glEnable(GL_POLYGON_OFFSET_FILL);
    
  glEnable(GL_DEPTH_TEST);
    
//   glEnable(GL_LINE_SMOOTH);
    
//   glEnable(GL_BLEND);
    
//   glBlendFunc(GL_ONE, GL_ZERO);
  
  // Load materials/lights/textures
  initMaterials();
  initLights();
  loadTextMaps();

  // Generate display lists
  initLists();
}

/*!
  Generates a unique integer for the given cell
  This is used as the cell "name" for the OpenGL picking code
*/
size_t QMinefield::encodeBox(size_t x, size_t y, size_t z) {
  size_t retVal = 0;

  retVal += z;
  retVal *=500;
  retVal += y;
  retVal *=500;
  retVal += x;
  return retVal;
}

/*!
  Computes the unique cell index given the cells unique name
  This is used to tell which cell was picked
*/
void QMinefield::decodeBox(size_t val, size_t &x, size_t &y, size_t &z) {
  size_t temp = val;
  x = temp % 500;
  temp /= 500;
  y = temp %500;
  temp /= 500;
  z = temp;
}

/*!
  Loops through the mine and draws each cell
*/
void QMinefield::drawMine() {
  if (!mf) return;
  
  glPushMatrix();
  size_t d = mf->depth();
  size_t h = mf->height();
  size_t w = mf->width();
  
  for (size_t i=0; i<w; ++i) {
    for (size_t j=0; j<h; ++j) {
      for (size_t k=0; k<d; ++k) {
	drawCell(i,j,k);
      }
    }
  }
  glPopMatrix();
}

/*!
  Initializes a display list with the given material
*/
void QMinefield::drawBoxList(size_t mat_idx) {

  // Draw the box
  glBegin(GL_QUADS);
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[mat_idx]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[mat_idx]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[mat_idx]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[mat_idx]);

  glVertex3f( 0.5f, 0.5f,-0.5f);      // Top Right Of The Quad (Top)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Top Left Of The Quad (Top)
  glVertex3f(-0.5f, 0.5f, 0.5f);      // Bottom Left Of The Quad (Top)
  glVertex3f( 0.5f, 0.5f, 0.5f);      // Bottom Right Of The Quad (Top)

  glVertex3f( 0.5f,-0.5f, 0.5f);      // Top Right Of The Quad (Bottom)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Top Left Of The Quad (Bottom)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Bottom Left Of The Quad (Bottom)
  glVertex3f( 0.5f,-0.5f,-0.5f);      // Bottom Right Of The Quad (Bottom)

  glVertex3f( 0.5f, 0.5f, 0.5f);      // Top Right Of The Quad (Front)
  glVertex3f(-0.5f, 0.5f, 0.5f);      // Top Left Of The Quad (Front)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Bottom Left Of The Quad (Front)
  glVertex3f( 0.5f,-0.5f, 0.5f);      // Bottom Right Of The Quad (Front)

  glVertex3f( 0.5f,-0.5f,-0.5f);      // Top Right Of The Quad (Back)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Top Left Of The Quad (Back)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Bottom Left Of The Quad (Back)
  glVertex3f( 0.5f, 0.5f,-0.5f);      // Bottom Right Of The Quad (Back)

  glVertex3f(-0.5f, 0.5f, 0.5f);      // Top Right Of The Quad (Left)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Top Left Of The Quad (Left)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Bottom Left Of The Quad (Left)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Bottom Right Of The Quad (Left)

  glVertex3f( 0.5f, 0.5f,-0.5f);      // Top Right Of The Quad (Right)
  glVertex3f( 0.5f, 0.5f, 0.5f);      // Top Left Of The Quad (Right)
  glVertex3f( 0.5f,-0.5f, 0.5f);      // Bottom Left Of The Quad (Right)
  glVertex3f( 0.5f,-0.5f,-0.5f);      // Bottom
  glEnd();
  
  
  glLineWidth(2.0);
  
  // Draw the outline
  glBegin(GL_LINE_LOOP);

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[LINE_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[LINE_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[LINE_MAT]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[LINE_MAT]);
    
  glVertex3f( 0.5f, 0.5f,-0.5f);      // Top Right Of The Quad (Top)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Top Left Of The Quad (Top)
  glVertex3f(-0.5f, 0.5f, 0.5f);      // Bottom Left Of The Quad (Top)
  glVertex3f( 0.5f, 0.5f, 0.5f);      // Bottom Right Of The Quad (Top)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.5f,-0.5f, 0.5f);      // Top Right Of The Quad (Bottom)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Top Left Of The Quad (Bottom)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Bottom Left Of The Quad (Bottom)
  glVertex3f( 0.5f,-0.5f,-0.5f);      // Bottom Right Of The Quad (Bottom)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.5f, 0.5f, 0.5f);      // Top Right Of The Quad (Front)
  glVertex3f(-0.5f, 0.5f, 0.5f);      // Top Left Of The Quad (Front)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Bottom Left Of The Quad (Front)
  glVertex3f( 0.5f,-0.5f, 0.5f);      // Bottom Right Of The Quad (Front)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.5f,-0.5f,-0.5f);      // Top Right Of The Quad (Back)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Top Left Of The Quad (Back)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Bottom Left Of The Quad (Back)
  glVertex3f( 0.5f, 0.5f,-0.5f);      // Bottom Right Of The Quad (Back)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.5f, 0.5f, 0.5f);      // Top Right Of The Quad (Left)
  glVertex3f(-0.5f, 0.5f,-0.5f);      // Top Left Of The Quad (Left)
  glVertex3f(-0.5f,-0.5f,-0.5f);      // Bottom Left Of The Quad (Left)
  glVertex3f(-0.5f,-0.5f, 0.5f);      // Bottom Right Of The Quad (Left)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.5f, 0.5f,-0.5f);      // Top Right Of The Quad (Right)
  glVertex3f( 0.5f, 0.5f, 0.5f);      // Top Left Of The Quad (Right)
  glVertex3f( 0.5f,-0.5f, 0.5f);      // Bottom Left Of The Quad (Right)
  glVertex3f( 0.5f,-0.5f,-0.5f);      // Bottom
  glEnd();
    
  glLineWidth(1.0);
}

/*!
  Draw a box with a numbered cube with a texture map
*/
void QMinefield::drawNumberBoxList(size_t tn) {

  if (tn==0) return;
  

  // Enable texturing and transparency
  glEnable(GL_TEXTURE_2D);
  
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, textNames[tn]);

  
  // Draw the box
  glBegin(GL_QUADS);
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[NUMBER_BOX_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[NUMBER_BOX_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[NUMBER_BOX_MAT]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[NUMBER_BOX_MAT]);

  glTexCoord2f(0.0,0.0); glVertex3f( 0.125f, 0.125f,-0.125f);      // Top Right Of The Quad (Top)
  glTexCoord2f(0.0,1.0); glVertex3f(-0.125f, 0.125f,-0.125f);      // Top Left Of The Quad (Top)
  glTexCoord2f(1.0,1.0); glVertex3f(-0.125f, 0.125f, 0.125f);      // Bottom Left Of The Quad (Top)
  glTexCoord2f(1.0,0.0); glVertex3f( 0.125f, 0.125f, 0.125f);      // Bottom Right Of The Quad (Top)

  glTexCoord2f(0.0,0.0); glVertex3f( 0.125f,-0.125f, 0.125f);      // Top Right Of The Quad (Bottom)
  glTexCoord2f(0.0,1.0); glVertex3f(-0.125f,-0.125f, 0.125f);      // Top Left Of The Quad (Bottom)
  glTexCoord2f(1.0,1.0); glVertex3f(-0.125f,-0.125f,-0.125f);      // Bottom Left Of The Quad (Bottom)
  glTexCoord2f(1.0,0.0); glVertex3f( 0.125f,-0.125f,-0.125f);      // Bottom Right Of The Quad (Bottom)

  glTexCoord2f(0.0,0.0); glVertex3f( 0.125f, 0.125f, 0.125f);      // Top Right Of The Quad (Front)
  glTexCoord2f(0.0,1.0); glVertex3f(-0.125f, 0.125f, 0.125f);      // Top Left Of The Quad (Front)
  glTexCoord2f(1.0,1.0); glVertex3f(-0.125f,-0.125f, 0.125f);      // Bottom Left Of The Quad (Front)
  glTexCoord2f(1.0,0.0); glVertex3f( 0.125f,-0.125f, 0.125f);      // Bottom Right Of The Quad (Front)

  glTexCoord2f(0.0,0.0); glVertex3f( 0.125f,-0.125f,-0.125f);      // Top Right Of The Quad (Back)
  glTexCoord2f(0.0,1.0); glVertex3f(-0.125f,-0.125f,-0.125f);      // Top Left Of The Quad (Back)
  glTexCoord2f(1.0,1.0); glVertex3f(-0.125f, 0.125f,-0.125f);      // Bottom Left Of The Quad (Back)
  glTexCoord2f(1.0,0.0); glVertex3f( 0.125f, 0.125f,-0.125f);      // Bottom Right Of The Quad (Back)

  glTexCoord2f(0.0,0.0); glVertex3f(-0.125f, 0.125f, 0.125f);      // Top Right Of The Quad (Left)
  glTexCoord2f(0.0,1.0); glVertex3f(-0.125f, 0.125f,-0.125f);      // Top Left Of The Quad (Left)
  glTexCoord2f(1.0,1.0); glVertex3f(-0.125f,-0.125f,-0.125f);      // Bottom Left Of The Quad (Left)
  glTexCoord2f(1.0,0.0); glVertex3f(-0.125f,-0.125f, 0.125f);      // Bottom Right Of The Quad (Left)


  glTexCoord2f(0.0,0.0); glVertex3f( 0.125f, 0.125f,-0.125f);      // Top Right Of The Quad (Right)
  glTexCoord2f(0.0,1.0); glVertex3f( 0.125f, 0.125f, 0.125f);      // Top Left Of The Quad (Right)
  glTexCoord2f(1.0,1.0); glVertex3f( 0.125f,-0.125f, 0.125f);      // Bottom Left Of The Quad (Right)
  glTexCoord2f(1.0,0.0); glVertex3f( 0.125f,-0.125f,-0.125f);      // Bottom
  
  glEnd();
  
  // Disable texturing and transparency
  glDisable(GL_TEXTURE_2D);
    
  glBlendFunc(GL_ONE, GL_ZERO);
  
  glLineWidth(2.0);
  
  // Draw the outline
  glBegin(GL_LINE_LOOP);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse[LINE_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular[LINE_MAT]);
//   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess[LINE_MAT]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient[LINE_MAT]);
    
  glVertex3f( 0.1f, 0.1f,-0.1f);      // Top Right Of The Quad (Top)
  glVertex3f(-0.1f, 0.1f,-0.1f);      // Top Left Of The Quad (Top)
  glVertex3f(-0.1f, 0.1f, 0.1f);      // Bottom Left Of The Quad (Top)
  glVertex3f( 0.1f, 0.1f, 0.1f);      // Bottom Right Of The Quad (Top)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.1f,-0.1f, 0.1f);      // Top Right Of The Quad (Bottom)
  glVertex3f(-0.1f,-0.1f, 0.1f);      // Top Left Of The Quad (Bottom)
  glVertex3f(-0.1f,-0.1f,-0.1f);      // Bottom Left Of The Quad (Bottom)
  glVertex3f( 0.1f,-0.1f,-0.1f);      // Bottom Right Of The Quad (Bottom)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.1f, 0.1f, 0.1f);      // Top Right Of The Quad (Front)
  glVertex3f(-0.1f, 0.1f, 0.1f);      // Top Left Of The Quad (Front)
  glVertex3f(-0.1f,-0.1f, 0.1f);      // Bottom Left Of The Quad (Front)
  glVertex3f( 0.1f,-0.1f, 0.1f);      // Bottom Right Of The Quad (Front)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.1f,-0.1f,-0.1f);      // Top Right Of The Quad (Back)
  glVertex3f(-0.1f,-0.1f,-0.1f);      // Top Left Of The Quad (Back)
  glVertex3f(-0.1f, 0.1f,-0.1f);      // Bottom Left Of The Quad (Back)
  glVertex3f( 0.1f, 0.1f,-0.1f);      // Bottom Right Of The Quad (Back)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f(-0.1f, 0.1f, 0.1f);      // Top Right Of The Quad (Left)
  glVertex3f(-0.1f, 0.1f,-0.1f);      // Top Left Of The Quad (Left)
  glVertex3f(-0.1f,-0.1f,-0.1f);      // Bottom Left Of The Quad (Left)
  glVertex3f(-0.1f,-0.1f, 0.1f);      // Bottom Right Of The Quad (Left)
  glEnd();
      
  glBegin(GL_LINE_LOOP);
  glVertex3f( 0.1f, 0.1f,-0.1f);      // Top Right Of The Quad (Right)
  glVertex3f( 0.1f, 0.1f, 0.1f);      // Top Left Of The Quad (Right)
  glVertex3f( 0.1f,-0.1f, 0.1f);      // Bottom Left Of The Quad (Right)
  glVertex3f( 0.1f,-0.1f,-0.1f);      // Bottom
  glEnd();
    
  glLineWidth(1.0);
}

/*!
  Called automatically when the window is rezied
*/
void QMinefield::resizeGL(int width, int height) {
  glViewport(0,0, (GLsizei) width, (GLsizei)height);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(80, 1.0, 1.0, 180);
    
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*!
  Determines the unique name of the cell at pos
*/
int QMinefield::nameAtPos(const QPoint &pos) {

  const int MaxSize = 2048;
  GLuint buffer[MaxSize];
  GLint viewport[4];

  // Picking requires drawing the scene in GL_SELECT mode,
  // which requires a little setup
  
  // For details on how this work, see the "OpenGL Programming Guide"
  // All editions should give the details, but the fifth edition,
  // pages 570-580 is where I got it.

  // The general idea is to draw a tiny portion of the screen around
  // the mouse position in GL_SELECT mode, then switch back to GL_RENDER
  // mode, which will return the number of hits and populate "buffer".
  // Then search for the closest hit.

  glGetIntegerv(GL_VIEWPORT, viewport);

  glSelectBuffer(MaxSize, buffer);
  
  glRenderMode(GL_SELECT);
  
  glInitNames();
  glPushName(0);

  // setup the viewing transformation
  glPushMatrix();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // Make it use a 2.0x2.0 area round the mouse click
  gluPickMatrix(GLdouble(pos.x()), GLdouble(viewport[3]-pos.y()),
		2.0,2.0, viewport);
  // The regular view transformation
  gluPerspective(80, 1.0, 1.0, 180);

  // Switch to GL_MODELVIEW and draw the scene
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  paintGL();

  // Reset the view and model transforms
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // Switch back to GL_RENDER and get the number of hits
  int numHits = glRenderMode(GL_RENDER);

  // Return -1 if no hits
  if (numHits <=0)
    return -1;

  int closest = 0;

  // Loop through the returned hits to determine the closest one
  for (int i=0;i<numHits;++i) {
    if (buffer[4*i+1]<buffer[4*closest+1] ||
	buffer[4*i+2]<buffer[4*closest+2]) {
      closest = i;
    }
  }

  // Return the closest hit
  return buffer[4*closest + 3];
}

/*!
  Determines what (if anything) should be drawn at a cell locoation, and draws it.
*/
void QMinefield::drawCell(size_t x, size_t y, size_t z) {

  glPushMatrix();
    
  double xx = 2.0*double(x-0.5)/mf->width() - 1.0;
  double yy = 2.0*double(y-0.5)/mf->height() - 1.0;
  double zz = 2.0*double(z-0.5)/mf->depth() - 1.0;
  
  glScalef(10.0,10.0,10.0);
  
  glTranslated(xx, yy, zz);
  
  glScalef(1.99/mf->width(),
	   1.99/mf->height(),
	   1.99/mf->depth());

  size_t temp;
  switch (mf->getState(x,y,z)) {
  case open:
    // If the cell is near a bomb, display a numbered cube
    // Otherwise draw nothing
    temp = mf->bombsNear(x,y,z);

    if (temp>0 && !lost) {
      glLoadName(encodeBox(x,y,z));
      glCallList(dispLists[temp]);
    }
    break;


  case closed:
    // Draw a filled box
    if (lost) break;
    // Fall through on purpose
  case closed_bomb:
    glLoadName(encodeBox(x,y,z));
    
    if (!lost)
      glCallList(dispLists[GREY_BOX_DL]);
    else
      glCallList(dispLists[RED_BOX_DL]);
    break;
    
  case marked_empty:
    // Only draw marks if the game isn't over
    if (lost) break;
    // Fall through on purpose
  case marked_bomb:
    // Draw a red box
    glLoadName(encodeBox(x,y,z));
    glCallList(dispLists[RED_BOX_DL]);
    break;
    
  default:
    break;
  }
  glPopMatrix();
}

/*!
  Called by the system to draw the display
 */
void QMinefield::paintGL() {

  // Rotate/translate the projection matrix
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    
  glTranslatef(0.0,0.0,-(translate+5));
  glRotatef(rotationX, 1.0, 0.0, 0.0);
  glRotatef(rotationY, 0.0, 1.0, 0.0);
  glRotatef(rotationZ, 0.0, 0.0, 1.0);

  // Switch to modelview mode and draw the scene
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Setup the lights
  glLightfv(GL_LIGHT0, GL_POSITION, light_position[0]);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color[0]);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_color[0]);
  
//   glLightfv(GL_LIGHT1, GL_POSITION, light_position[1]);
//   glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color[1]);
//   glLightfv(GL_LIGHT1, GL_SPECULAR, light_color[1]);
  
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient[0]);
  glLoadIdentity();

  // Draw the mine
  if (mf)
    drawMine();

  // Reset to how we found things
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
    
  glMatrixMode(GL_MODELVIEW);
  glFlush();
}

/*!
  Handles mouse clicks
 */
void QMinefield::mousePressEvent(QMouseEvent *event) {
  // If the minefield isn't initialized, there's nothing to do
  if (!mf) return;

  if (!clicked) emit firstClick();

  clicked = true;

  // Try to get the name at this position
  int temp = nameAtPos(event->pos());
  
  // Set the last postion for rotations
  lastPos = event->pos();
  
  size_t x,y,z;
  // If a name was returned, get the cell index
  if (temp>=0) {
    decodeBox(temp, x,y,z);
  }
  
  if (event->buttons() & Qt::LeftButton) {
    
    if (temp>=0) {

      // Clicked on a bomb, so lose the game
      if (mf->getState(x,y,z)==closed_bomb) {
	
	lost = true;
	updateGL();
	emit gameLost();
      } else {
	// Clicked on an empty cell, so touch it
	mf->touch(x,y,z);
	
	// Check for a win
	if (mf->hasWon()) {
	  emit gameWon();
	}
      }
    }
  } else if (event->buttons() & Qt::RightButton) {
    // Marked a cell
    if (temp>=0) {
      mf->mark(x,y,z);
      emit bombMarked(mf->minesRemaining());
    }
  }
  
  // Update the display
  updateGL();
}

/*!
  Handles mouse moves
 */
void QMinefield::mouseMoveEvent(QMouseEvent *event) {

  
  GLfloat dx = GLfloat(event->x() - lastPos.x())/width();
  GLfloat dy = GLfloat(event->y() - lastPos.y())/height();

  // Rotate depending on which mouse button is clicked
  if (event->buttons() & Qt::LeftButton) {
    rotationX += 180*dy;
    rotationY += 180*dx;
    updateGL();
  } else if (event->buttons() & Qt::RightButton) {
    rotationX += 180*dy;
    rotationZ += 180*dx;
    updateGL();
  }
  
  // Save the current position
  lastPos = event->pos();
}

/*!
  Handle zooming
 */
void QMinefield::wheelEvent(QWheelEvent *event) {
  translate += event->delta()*(-0.125*0.5*0.5);
  
  if (translate<11.0) translate = 11.0;
  updateGL();
}

/*!
  Reset the view to the original setting.
*/
void QMinefield::resetView() {
  translate=25;
  rotationX = 27.2457;
  rotationY = -46.44;
  rotationZ = 0.0;
  updateGL();
}

/*!
  Debug function that will display information about OpenGL errors.
  The argument is the line number of the error
*/
inline void QMinefield::handleGLError(size_t ln) {
  GLenum ec = glGetError();
  // Do nothing if there's no error
  if (ec== GL_NO_ERROR) return;
  
  // Get the error information in a string
  std::ostringstream err;
  err << (const char*)gluErrorString(ec) << " : " << ln;

  // Print info to stdout and throw an exception
  //std::cout << err.str() << "\n";
  throw new std::runtime_error(err.str());
}
