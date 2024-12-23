#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_checkForUpdates.h"
#include "CheckForUpdatesDialog.h"

class checkForUpdates : public QMainWindow
{
  Q_OBJECT

public:
  checkForUpdates(QWidget *parent = nullptr);
  ~checkForUpdates();

protected slots:
  void onCheckForUpdates();

protected:
  CheckForUpdatesDialog *checkForUpdatesDialog_ = nullptr;

private:
  Ui::checkForUpdatesClass ui;
};
