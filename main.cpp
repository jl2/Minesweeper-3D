/*
  main.cpp
 
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

#include <QApplication>
#include <iostream>

#include "mainwindow.h"
#include "qminefield.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  if (!QGLFormat::hasOpenGL()) {
    std::cerr << "This system has no OpenGL support" << std::endl;
    return 1;
  }

  MainWindow *mainWin = new MainWindow;
  mainWin->resize(500,500);
  mainWin->show();

  return app.exec();
}
