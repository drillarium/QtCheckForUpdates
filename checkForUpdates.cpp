#include "checkForUpdates.h"

checkForUpdates::checkForUpdates(QWidget *parent)
:QMainWindow(parent)
{
  ui.setupUi(this);

  SCheckForUpdates cfu;
  checkForUpdatesDialog_ = new CheckForUpdatesDialog(cfu, this);
}

checkForUpdates::~checkForUpdates()
{

}

void checkForUpdates::onCheckForUpdates()
{
  if(checkForUpdatesDialog_->isVisible())
  {
    checkForUpdatesDialog_->raise();
    checkForUpdatesDialog_->activateWindow();
  }
  else
  {
    checkForUpdatesDialog_->show();
  }
}