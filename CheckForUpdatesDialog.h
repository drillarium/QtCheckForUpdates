#pragma once

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include "ui_CheckForUpdatesDialog.h"

// Convert Public Google Drive link to downloadable url
// https://drive.google.com/file/d/1hcELTT6TGwsduNXGzjdjMsNMNHPq7IPN/view?usp=sharing
// https://drive.google.com/uc?export=download&id=1hcELTT6TGwsduNXGzjdjMsNMNHPq7IPN

// https://drive.google.com/file/d/11SxVW5hbLIJjcbXHUN1u7IUMpxCLXqwm/view?usp=sharing
// https://drive.google.com/uc?export=download&id=11SxVW5hbLIJjcbXHUN1u7IUMpxCLXqwm

struct SCheckForUpdates
{
  QString url = "https://raw.githubusercontent.com/drillarium/QtCheckForUpdates/refs/heads/main/update.json";
  QString jsonFile = "update.json";
};

enum ECheckForUpdatesStatus
{
  E_CFOS_NONE,
  E_CFOS_ERROR,
  E_CFOS_DOWNLOADING_JSON,
  E_CFOS_READY_TO_DOWNLOAD,
  E_CFOS_DOWNLOADING_INSTALLER,
  E_CFOS_DOWNLOADED_INSTALLER,
};

class CheckForUpdatesDialog : public QDialog
{
  Q_OBJECT

public:
  CheckForUpdatesDialog(SCheckForUpdates _checkForUpdates, QWidget * _parent = nullptr);
  ~CheckForUpdatesDialog();

protected:
  void showEvent(QShowEvent *_event) override;
  void closeEvent(QCloseEvent *_event) override;
  void readSettings();
  void writeSettings();
  void setStatus(ECheckForUpdatesStatus _status);

protected slots:
  void onDownloadFinished(QNetworkReply *_reply);
  void onProcessAction();
  void onReadyRead();
  void onRedirected(const QUrl &_redirectUrl);

protected:
  ECheckForUpdatesStatus status_ = E_CFOS_DOWNLOADING_JSON;
  SCheckForUpdates checkForUpdates_;
  QNetworkAccessManager *networkManager_ = nullptr;
  QNetworkReply *networkReply_ = nullptr;
  QString lastError_;
  QString installerUrl_;
  QFile *saveFile_ = nullptr;
  QString savePath_;

private:
  Ui::CheckForUpdatesDialogClass ui_;
};
