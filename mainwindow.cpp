/*
  mainwindow.cpp
 
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
#include <QSettings>

#include <sstream>
#include <cstdlib>

#include "mainwindow.h"

#include "qminefield.h"

/*!
  Performs initialization
*/
MainWindow::MainWindow() : QMainWindow(), lost(false) {
  
  std::srand(std::time(0));

  // Create QMinefield widget
  qmf = new QMinefield(this);
  
  promptExit = true;
  lost = true;
  // Default to easy difficulty
  difficulty=0;
  
  
  
  // Set difficulty parameters
  difficultySizes[DIF_EASY]=6;
  difficultySizes[DIF_MEDM]=11;
  difficultySizes[DIF_HARD]=15;
  
  difficultyBombs[DIF_EASY]=10;
  difficultyBombs[DIF_MEDM]=60;
  difficultyBombs[DIF_HARD]=160;

  qset = new QSettings(QSettings::IniFormat, QSettings::UserScope,
		       "Mine3D", "Mine3D");

  readHighScores();
  
  // Start a new game
  qmf->startNewGame(difficultySizes[difficulty],
		    difficultySizes[difficulty],
		    difficultySizes[difficulty],
		    difficultyBombs[difficulty]);

  // Make the QMinefield the central widget 
  setCentralWidget(qmf);

  // Initialize GUI stuff
  setWindowTitle(tr("Minesweeper 3D"));
  setWindowIcon(QIcon(":/images/icon.png"));
  createActions();
  createMenus();
  createToolbar();
  createStatusBar();
    
  start_time = 0;
  
  theTimer = new QTimer(this);

  // Connect some signals
  connect(qmf, SIGNAL(gameLost()), this, SLOT(loseGame()));
  connect(qmf, SIGNAL(gameWon()), this, SLOT(winGame()));
  connect(qmf, SIGNAL(bombMarked(int)), this, SLOT(updateStatusBar(int)));
  connect(qmf, SIGNAL(firstClick()), this, SLOT(startTimer()));
  
  connect(theTimer, SIGNAL(timeout()), this, SLOT(update()));

}

MainWindow::~MainWindow() {
  // Delete everything
  delete newGameAction;
  delete aboutAction;
  delete aboutQtAction;
  delete quitAction;
  delete resetViewAction;

  delete easyAction;
  delete medAction;
  delete hardAction;
  
  delete theToolbar;
  
  delete gameMenu;
  delete optionsMenu;
  delete helpMenu;
  delete statusLabel;
  delete tbIcon;

  delete qmf;
}

/*!
  Initializes all of the QActions used by the game
*/
void MainWindow::createActions() {
  // New game
  newGameAction = new QAction(tr("&New"), this);
  newGameAction->setIcon(QIcon(":/images/new.png"));
  newGameAction->setShortcut(tr("Ctrl+N"));
  newGameAction->setStatusTip(tr("Start a new game"));
  connect(newGameAction, SIGNAL(triggered()), this, SLOT(newGame()));

  // About
  aboutAction = new QAction(tr("About"), this);
  aboutAction->setIcon(QIcon(":/images/about.png"));
  aboutAction->setShortcut(tr("Ctrl+A"));
  aboutAction->setStatusTip(tr("About Minesweeper 3D"));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

  // About QT
  aboutQtAction = new QAction(tr("About QT"), this);
  aboutQtAction->setIcon(QIcon(":/images/about.png"));
  aboutQtAction->setShortcut(tr("Ctrl+A"));
  aboutQtAction->setStatusTip(tr("About Minesweeper 3D"));
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  // Exit
  quitAction = new QAction(tr("Exit"), this);
  quitAction->setIcon(QIcon(":/images/quit.png"));
  quitAction->setShortcut(tr("Ctrl+Q"));
  quitAction->setStatusTip(tr("Quit"));
  connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));//quit()));

  // Reset view
  resetViewAction = new QAction(tr("Reset View"), this);
  resetViewAction->setIcon(QIcon(":/images/view.png"));
  resetViewAction->setShortcut(tr("Ctrl+V"));
  resetViewAction->setStatusTip(tr("Reset the view"));
  connect(resetViewAction, SIGNAL(triggered()), this, SLOT(resetView()));

  // Set easy difficulty
  easyAction = new QAction(tr("Easy"), this);
  easyAction->setCheckable(true);
  easyAction->setStatusTip(tr("Start an easy game"));
  easyAction->setChecked(true);
  connect(easyAction, SIGNAL(triggered()), this, SLOT(startEasyGame()));

  // Set medium difficulty
  medAction = new QAction(tr("Medium"), this);
  medAction->setCheckable(true);
  medAction->setStatusTip(tr("Start a medium difficulty game"));
  connect(medAction, SIGNAL(triggered()), this, SLOT(startMediumGame()));

  // Set hard difficulty
  hardAction = new QAction(tr("Hard"), this);
  hardAction->setCheckable(true);
  hardAction->setStatusTip(tr("Start a hard game"));
  connect(hardAction, SIGNAL(triggered()), this, SLOT(startHardGame()));

  // Show High Scores dialog box
  highScoresAction = new QAction(tr("High Scores"), this);
  highScoresAction->setStatusTip(tr("Show high scores"));
  connect(highScoresAction, SIGNAL(triggered()), this, SLOT(showHighScores()));

  timeAction = new QAction(tr("0"), this);
  timeAction->setStatusTip(tr("Time"));
}

/*!
  Initialize menus
 */
void MainWindow::createMenus() {
  // Game menu
  gameMenu = menuBar()->addMenu(tr("&File"));
  gameMenu->addAction(newGameAction);
  gameMenu->addSeparator();
  gameMenu->addAction(highScoresAction);
  gameMenu->addSeparator();
  gameMenu->addAction(quitAction);

  // Options menu
  optionsMenu = menuBar()->addMenu(tr("&Options"));
  optionsMenu->addAction(easyAction);
  optionsMenu->addAction(medAction);
  optionsMenu->addAction(hardAction);

  // Help menu
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addSeparator();
  helpMenu->addAction(aboutQtAction);
}

/*!
  Initialize toolbar
*/
void MainWindow::createToolbar() {
  theToolbar = addToolBar(tr("Game"));
  theToolbar->addAction(newGameAction);
  theToolbar->addSeparator();
  theToolbar->addAction(resetViewAction);
  theToolbar->addAction(timeAction);
}

/*!
  Initialize status bar
*/
void MainWindow::createStatusBar() {
  // Make the label as large as it will ever be
  statusLabel = new QLabel("10000 Bombs Left");
  statusLabel->setAlignment(Qt::AlignHCenter);
  // Then set its minimum size and add to the status bar
  statusLabel->setMinimumSize(statusLabel->sizeHint());
  statusBar()->addWidget(statusLabel);

  // Now fill in the actual text
  std::ostringstream fname;
  fname << difficultyBombs[difficulty] << " Bombs Left";
  statusLabel->setText(QString(fname.str().c_str()));
}

/*!
  Updates the status bar to display number of bombs left.
 */
void MainWindow::updateStatusBar(int num_bombs) {
  std::ostringstream fname;
  fname << num_bombs << " Bombs Left";
  statusLabel->setText(QString(fname.str().c_str()));
}

/*!
  Creates a new game in response to the newGame action being triggered
 */
void MainWindow::newGame() {
  // Prompt first
  if (!start_time || lost || QMessageBox::warning(this, tr("Minesweeper 3D"),
				   tr("Really start a new game?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape)==QMessageBox::Yes) {
    lost = false;
    qmf->startNewGame( difficultySizes[difficulty], difficultySizes[difficulty], difficultySizes[difficulty], difficultyBombs[difficulty]);
    start_time = 0;
  }
}

/*!
  Display the about box
*/
void MainWindow::about() {
  QMessageBox::about(this, tr("About Minesweeper 3D"),
		     tr("<h2>Minsweeper 3D 0.1</h2>"
			"<p>Copyright &copy; 2008 Jeremiah LaRocco</p>"
			"<p>3D Minsweeper</p>"));
}

/*!
  Prompt before closing
 */
void MainWindow::closeEvent(QCloseEvent *event) {
  if (QMessageBox::warning(this, tr("Minesweeper 3D"),
			   tr("Really quit?"),
			   QMessageBox::Yes | QMessageBox::Default,
			   QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape)==QMessageBox::Yes) {
    delete qmf;
    event->accept();
  } else {
    event->ignore();
  }
}

/*!
  Reset the view
 */
void MainWindow::resetView() {
  if (qmf) qmf->resetView();
}

/*!
  Triggered when the user has won the game
 */
void MainWindow::winGame() {
  
  if (qmf) {
    theTimer->stop();
    std::time_t end_time = std::time(0);

    double elapsed = difftime(end_time, start_time);

    if (elapsed < best_times[difficulty]) {
      QString difs[] = {tr("easy"), tr("medm"), tr("hard")};
      
      qset->setValue(difs[difficulty] + tr("_name"), getenv("USERNAME"));
      qset->setValue(difs[difficulty] + tr("_time"), elapsed);
      qset->setValue(difs[difficulty] + tr("_date"), QDateTime::currentDateTime());
      qset->sync();
      readHighScores();
      
      if (QMessageBox::information(this, tr("Minesweeper 3D"),
				   tr("You've set the high score for this level!  Play again?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No) == QMessageBox::Yes) {
	qmf->startNewGame(difficultySizes[difficulty],
			  difficultySizes[difficulty],
			  difficultySizes[difficulty],
			  difficultyBombs[difficulty]);
	start_time = 0;
      } else {
	exit(0);
      }

    } else {
      if (QMessageBox::information(this, tr("Minesweeper 3D"),
				   tr("You've actually won!  Play again?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No) == QMessageBox::Yes) {
	qmf->startNewGame(difficultySizes[difficulty],
			  difficultySizes[difficulty],
			  difficultySizes[difficulty],
			  difficultyBombs[difficulty]);
	start_time = 0;
      } else {
	exit(0);
      }
    }
  }
}

/*!
  Triggered when the user has lost the game
 */
void MainWindow::loseGame() {
  lost = true;
  theTimer->stop();
}

/*!
  Starts a game on the easy difficulty level
 */
void MainWindow::startEasyGame() {
  
  if (!start_time || lost || QMessageBox::warning(this, tr("Minesweeper 3D"),
				   tr("Really start a new game?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape)==QMessageBox::Yes) {
    difficulty = DIF_EASY;
    medAction->setChecked(false);
    hardAction->setChecked(false);
    lost = false;
    qmf->startNewGame( difficultySizes[difficulty], difficultySizes[difficulty], difficultySizes[difficulty], difficultyBombs[difficulty]);
    start_time = 0;
    updateStatusBar(difficultyBombs[difficulty]);
  }
}

/*!
  Starts a game on the medium difficulty level
*/
void MainWindow::startMediumGame() {
  if (!start_time || lost || QMessageBox::warning(this, tr("Minesweeper 3D"),
				   tr("Really start a new game?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape)==QMessageBox::Yes) {
    easyAction->setChecked(false);
    hardAction->setChecked(false);
    difficulty = DIF_MEDM;
    lost = false;
    qmf->startNewGame( difficultySizes[difficulty], difficultySizes[difficulty], difficultySizes[difficulty], difficultyBombs[difficulty]);
    start_time = 0;
    updateStatusBar(difficultyBombs[difficulty]);
  }
}

/*!
  Starts a game on the hard difficulty level
 */
void MainWindow::startHardGame() {
  if (!start_time || lost || QMessageBox::warning(this, tr("Minesweeper 3D"),
				   tr("Really start a new game?"),
				   QMessageBox::Yes | QMessageBox::Default,
				   QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape)==QMessageBox::Yes) {
    easyAction->setChecked(false);
    medAction->setChecked(false);
    difficulty = DIF_HARD;
    lost = false;
    qmf->startNewGame( difficultySizes[difficulty], difficultySizes[difficulty], difficultySizes[difficulty], difficultyBombs[difficulty]);
    start_time = 0;
    updateStatusBar(difficultyBombs[difficulty]);
  }
}

void MainWindow::readHighScores() {
  qset->sync();
  best_times[DIF_EASY] = qset->value("easy_time", 1000).toInt();
  best_times[DIF_MEDM] = qset->value("medm_time", 10000).toInt();
  best_times[DIF_HARD] = qset->value("hard_time", 10000).toInt();

  best_names[DIF_EASY] = qset->value("easy_name", tr("Anon")).toString();
  best_names[DIF_MEDM] = qset->value("medm_name", tr("Anon")).toString();
  best_names[DIF_HARD] = qset->value("hard_name", tr("Anon")).toString();
  
  best_dates[DIF_EASY] = qset->value("easy_date", QDateTime(QDate(2000, 1,1))).toDateTime();
  best_dates[DIF_MEDM] = qset->value("medm_date", QDateTime(QDate(2000, 1,1))).toDateTime();
  best_dates[DIF_HARD] = qset->value("hard_date", QDateTime(QDate(2000, 1,1))).toDateTime();
}


/*!
  Displays the high scores dialog box.
 */
void MainWindow::showHighScores() {

  readHighScores();
  QMessageBox::information(this, tr("Minesweeper 3D"),
			   QString(tr("Easy   : %1 : %2 : %3\n"
				      "Medium : %4 : %5 : %6\n"
				      "Hard   : %7 : %8 : %9\n"))
			   .arg(best_times[DIF_EASY]).arg(best_names[DIF_EASY]).arg(best_dates[DIF_EASY].toString())
			   .arg(best_times[DIF_MEDM]).arg(best_names[DIF_MEDM]).arg(best_dates[DIF_MEDM].toString())
			   .arg(best_times[DIF_HARD]).arg(best_names[DIF_HARD]).arg(best_dates[DIF_HARD].toString()),
			   QMessageBox::Ok | QMessageBox::Default);
}

/*!
  Set the game start time.
  */
void MainWindow::startTimer() {
  start_time = std::time(0);
  theTimer->start(1000);
}

/*!
  Update the timer
  */
void MainWindow::update() {
  if (start_time) {
    std::time_t end_time = std::time(0);
    double elapsed = difftime(end_time, start_time);
    
    timeAction->setText(tr("%1").arg(elapsed));
  } else {
    timeAction->setText(tr("0"));
  }
}
