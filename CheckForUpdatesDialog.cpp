#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVersionNumber>
#include <QDir>
#include <QProcess>
#include "CheckForUpdatesDialog.h"
#include "version.h"

CheckForUpdatesDialog::CheckForUpdatesDialog(SCheckForUpdates _checkForUpdates, QWidget *_parent)
:QDialog(_parent)
,checkForUpdates_(_checkForUpdates)
{
  ui_.setupUi(this);

  // remove question mark from the title bar
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  // fixes size
  setFixedSize(width(), height());

  // Setwork access manager
  networkManager_ = new QNetworkAccessManager(this);
  QObject::connect(networkManager_, &QNetworkAccessManager::finished, this, &CheckForUpdatesDialog::onDownloadFinished);
}

CheckForUpdatesDialog::~CheckForUpdatesDialog()
{
}

void CheckForUpdatesDialog::showEvent(QShowEvent* _event)
{
  readSettings();
  QDialog::activateWindow();

  setStatus(E_CFOS_DOWNLOADING_JSON);
}

void CheckForUpdatesDialog::closeEvent(QCloseEvent *_event)
{
  writeSettings();
  QDialog::closeEvent(_event);

  ui_.messageLabel->clear();
  setStatus(E_CFOS_NONE);
}

void CheckForUpdatesDialog::readSettings()
{
  QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
  settings.beginGroup("CheckForUpdatesDialog");
  const QByteArray geometry = settings.value("geometry").toByteArray();
  if(!geometry.isEmpty())
  {
    restoreGeometry(geometry);
  }
  settings.endGroup();
}

void CheckForUpdatesDialog::writeSettings()
{
  QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
  settings.beginGroup("CheckForUpdatesDialog");
  settings.setValue("geometry", saveGeometry());
  settings.endGroup();
}

void CheckForUpdatesDialog::setStatus(ECheckForUpdatesStatus _status)
{
  status_ = _status;

  // Cancel current download
  if(networkReply_)
  {
    networkReply_->abort();
    networkReply_->deleteLater();
    networkReply_ = nullptr;
  }

  if(_status == E_CFOS_NONE)
  {
    ui_.progressBar->hide();
    ui_.actionButton->setText("Recheck version");
    ui_.actionButton->show();
  }
  else if(_status == E_CFOS_ERROR)
  {
    ui_.messageLabel->setText(QString("Error: %1").arg(lastError_));
    ui_.progressBar->hide();
    ui_.actionButton->hide();
  }
  else if(_status == E_CFOS_DOWNLOADING_JSON)
  {
    ui_.messageLabel->setText("Checking for updates ...");
    ui_.progressBar->hide();
    ui_.actionButton->hide();

    // async download JSON and parse it
    QUrl updateUrl(checkForUpdates_.url);
    QNetworkRequest request(updateUrl);
    networkReply_ = networkManager_->get(request);
  }
  else if(_status == E_CFOS_READY_TO_DOWNLOAD)
  {
    ui_.progressBar->hide();
    ui_.actionButton->setText("Download");
    ui_.actionButton->show();
  }
  else if(_status == E_CFOS_DOWNLOADING_INSTALLER)
  {
    ui_.messageLabel->setText("Downloading installer ...");
    ui_.progressBar->setValue(0);
    ui_.progressBar->show();
    ui_.actionButton->hide();

    if(saveFile_)
    {
      saveFile_->close();
      saveFile_->deleteLater();
    }
    savePath_ = QDir::tempPath() + "/installer.exe";
    saveFile_ = new QFile(savePath_);
    saveFile_->open(QIODevice::WriteOnly);

    // async download JSON and parse it
    QUrl updateUrl(installerUrl_);
    QNetworkRequest request(updateUrl);
    networkReply_ = networkManager_->get(request);

    QObject::connect(networkReply_, &QNetworkReply::redirected, this, &CheckForUpdatesDialog::onRedirected);
    QObject::connect(networkReply_, &QNetworkReply::downloadProgress, [&] (qint64 bytesReceived, qint64 bytesTotal) { ui_.progressBar->setValue((bytesReceived * 100. / bytesTotal)); });
    QObject::connect(networkReply_, &QIODevice::readyRead, this, &CheckForUpdatesDialog::onReadyRead);
  }
  else if(status_ == E_CFOS_DOWNLOADED_INSTALLER)
  {
    ui_.messageLabel->setText("Close App and install installer.");
    ui_.progressBar->hide();
    ui_.actionButton->setText("Install");
    ui_.actionButton->show();
  }
}

void CheckForUpdatesDialog::onDownloadFinished(QNetworkReply *_reply)
{
  if(_reply->error() != QNetworkReply::NoError)
  {
    lastError_ = _reply->errorString();
    setStatus(E_CFOS_ERROR);   
    _reply->deleteLater();
    networkReply_ = nullptr;
    return;
  }

  if(status_ == E_CFOS_DOWNLOADING_JSON)
  {
    QByteArray responseData = _reply->readAll();
    if(responseData.isEmpty())
    {
      lastError_ = "No data received form the server.";
      setStatus(E_CFOS_ERROR);
      _reply->deleteLater();
      networkReply_ = nullptr;
      return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj = jsonDoc.object();
    QString latestVersion = jsonObj["version"].toString();
    QString downloadUrl = jsonObj["url"].toString();

    QVersionNumber currentVersion(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
    QVersionNumber serverVersion = QVersionNumber::fromString(latestVersion);

    if(serverVersion > currentVersion)
    {
      installerUrl_ = downloadUrl;
      ui_.messageLabel->setText(QString("Version %1 available.").arg(latestVersion));
      setStatus(E_CFOS_READY_TO_DOWNLOAD);
    }
    else
    {
      ui_.messageLabel->setText("You are running the latest version.");
      setStatus(E_CFOS_NONE);
    }
  }
  else if(status_ == E_CFOS_DOWNLOADING_INSTALLER)
  {
    if(saveFile_)
    {
      saveFile_->close();
      saveFile_->deleteLater();
      saveFile_ = nullptr;
    }
    saveFile_ = nullptr;

    setStatus(E_CFOS_DOWNLOADED_INSTALLER);
  }

  _reply->deleteLater();
  networkReply_ = nullptr;
}

#include <Windows.h>

void CheckForUpdatesDialog::onProcessAction()
{
  if(status_ == E_CFOS_NONE)
  {
    setStatus(E_CFOS_DOWNLOADING_JSON);
  }
  else if(status_ == E_CFOS_READY_TO_DOWNLOAD)
  {
    setStatus(E_CFOS_DOWNLOADING_INSTALLER);
  }
  else if(status_ == E_CFOS_DOWNLOADED_INSTALLER)
  {
#ifdef Q_OS_WIN
    // Requesting elevation
    HINSTANCE result = ::ShellExecuteA(0, "runas", savePath_.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
    if(result <= (HINSTANCE) 32) {
      // error handling
    }
    else
    {
      QCoreApplication::quit();
    }
#endif
  }
}

void CheckForUpdatesDialog::onReadyRead()
{
  if(saveFile_)
  {
    saveFile_->write(networkReply_->readAll());
  }
}

void CheckForUpdatesDialog::onRedirected(const QUrl &_redirectUrl)
{
  if(!_redirectUrl.isEmpty())
  {
    networkReply_->deleteLater();

    QNetworkRequest newRequest(_redirectUrl);
    networkReply_ = networkManager_->get(newRequest);

    // Reconnect signals for the new reply
    QObject::connect(networkReply_, &QNetworkReply::redirected, this, &CheckForUpdatesDialog::onRedirected);
    QObject::connect(networkReply_, &QNetworkReply::downloadProgress, [&](qint64 bytesReceived, qint64 bytesTotal) { ui_.progressBar->setValue((bytesReceived * 100. / bytesTotal)); });
    QObject::connect(networkReply_, &QNetworkReply::readyRead, this, &CheckForUpdatesDialog::onReadyRead);
  }
}