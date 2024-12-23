#include "checkForUpdates.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("NRDMultimedia");
  QCoreApplication::setApplicationName("TestCheckForUpdates");

  checkForUpdates w;
  w.show();
  return a.exec();
}
