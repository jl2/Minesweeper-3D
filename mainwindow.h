/*
  mainwindow.h
 
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <ctime>

class QAction;
class QMinefield;
class QLabel;
class QMinefield;
class QIcon;
class QMenu;
class QToolBar;
class QCloseEvent;
class QSettings;
class QTimer;

// Some constants...
static const size_t NUM_DIFFICULTIES = 3;

static const size_t DIF_EASY = 0;
static const size_t DIF_MEDM = 1;
static const size_t DIF_HARD = 2;


class MainWindow : public QMainWindow {
  Q_OBJECT;

 public:
  MainWindow();
  ~MainWindow();

  private slots:
  void newGame();
  void about();
  void resetView();
  void winGame();
  void loseGame();
  void startEasyGame();
  void startMediumGame();
  void startHardGame();
  void showHighScores();
  void updateStatusBar(int num_bombs);

  void readHighScores();
  void startTimer();

  void update();
  
 protected:
  // Initialization functions
  void createActions();
  void createMenus();
  void createToolbar();
  void createStatusBar();
  void closeEvent(QCloseEvent *event);

 private:
  QAction *newGameAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
  QAction *quitAction;
  QAction *resetViewAction;
  
  QAction *easyAction;
  QAction *medAction;
  QAction *hardAction;

  QAction *highScoresAction;


  QAction *timeAction;
  
  QToolBar *theToolbar;
  
  QMenu *gameMenu;
  QMenu *optionsMenu;
  QMenu *helpMenu;
  QLabel *statusLabel;
  QIcon *tbIcon;

  QSettings *qset;

  QMinefield *qmf;

  bool promptExit;

  // Current difficulty level
  int difficulty;
  
  // Parameters for each difficulty level
  int difficultySizes[NUM_DIFFICULTIES];
  int difficultyBombs[NUM_DIFFICULTIES];

  std::time_t start_time;

  // This could be implemented a little better
  int best_times[NUM_DIFFICULTIES];
  QString best_names[NUM_DIFFICULTIES];
  QDateTime best_dates[NUM_DIFFICULTIES];
  
  // True = game lost
  bool lost;

  QTimer *theTimer;
  
};

#endif
