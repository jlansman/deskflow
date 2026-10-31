/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "UpdateInstaller.h"

#include "common/UrlConstants.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QUrl>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <shellapi.h>
#endif

namespace {

const auto kUpdateScript = QStringLiteral(R"PS(param([string]$Zip, [string]$InstallDir)
$ErrorActionPreference = 'Stop'
$work = Join-Path $env:TEMP 'deskflow-update'
Start-Transcript -Path (Join-Path $work 'update.log') -Force | Out-Null
try {
  $stage = Join-Path $work 'stage'
  if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
  Expand-Archive -Path $Zip -DestinationPath $stage -Force
  if (-not (Test-Path (Join-Path $stage 'deskflow-core.exe'))) { throw 'The download does not contain deskflow-core.exe' }

  Stop-Service Deskflow -Force -ErrorAction SilentlyContinue
  for ($i = 0; $i -lt 10; $i++) {
    $p = Get-Process deskflow, deskflow-core, deskflow-daemon -ErrorAction SilentlyContinue
    if (-not $p) { break }
    $p | Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 1
  }

  $backup = $InstallDir.TrimEnd('\') + '-previous'
  if (Test-Path $backup) { Remove-Item $backup -Recurse -Force }
  Copy-Item $InstallDir $backup -Recurse
  Copy-Item (Join-Path $stage '*') $InstallDir -Recurse -Force

  # a virus scanner can briefly deny the first start of freshly written files, so retry
  for ($i = 0; $i -lt 6; $i++) {
    try { Start-Service Deskflow -ErrorAction Stop; break }
    catch { if ($i -eq 5) { throw }; Start-Sleep -Seconds 3 }
  }
  # explorer starts the app un-elevated, like a normal launch
  Start-Process explorer.exe -ArgumentList ('"' + (Join-Path $InstallDir 'deskflow.exe') + '"')
}
catch {
  Write-Output $_
  Start-Service Deskflow -ErrorAction SilentlyContinue
  Add-Type -AssemblyName PresentationFramework
  [void][System.Windows.MessageBox]::Show("Deskflow update failed: $($_.Exception.Message)`nThe previous files are in $InstallDir-previous.", 'Deskflow update')
}
finally {
  Stop-Transcript | Out-Null
}
)PS");

} // namespace

UpdateInstaller::UpdateInstaller(QWidget *parent)
    : QObject(parent),
      m_parent(parent),
      m_network(new QNetworkAccessManager(this))
{
  // do nothing
}

void UpdateInstaller::start(const QString &version)
{
#ifdef Q_OS_WIN
  const auto answer = QMessageBox::question(
      m_parent, tr("Update Deskflow"),
      tr("Install Jeff Mod %1?\n\nDeskflow will close and restart. Windows will ask for permission to continue.")
          .arg(version)
  );
  if (answer == QMessageBox::Yes)
    download();
#else
  Q_UNUSED(version)
  QDesktopServices::openUrl(QUrl(kUrlForkRepo));
#endif
}

void UpdateInstaller::download()
{
  auto request = QNetworkRequest(QUrl(kUrlUpdatePackage));
  request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
  auto *reply = m_network->get(request);

  auto *progress = new QProgressDialog(tr("Downloading update..."), tr("Cancel"), 0, 0, m_parent);
  progress->setWindowModality(Qt::WindowModal);
  progress->setAttribute(Qt::WA_DeleteOnClose);
  progress->setMinimumDuration(0);
  connect(reply, &QNetworkReply::downloadProgress, progress, [progress](qint64 received, qint64 total) {
    progress->setMaximum(total > 0 ? static_cast<int>(total / 1024) : 0);
    progress->setValue(static_cast<int>(received / 1024));
  });
  connect(progress, &QProgressDialog::canceled, reply, &QNetworkReply::abort);

  connect(reply, &QNetworkReply::finished, this, [this, reply, progress]() {
    reply->deleteLater();
    // closing the dialog must not look like a user cancel
    progress->disconnect(reply);
    progress->close();

    if (reply->error() == QNetworkReply::OperationCanceledError)
      return;
    if (reply->error() != QNetworkReply::NoError) {
      QMessageBox::warning(m_parent, tr("Update Deskflow"), tr("Download failed: %1").arg(reply->errorString()));
      return;
    }

    const QDir work(QDir::tempPath() + QStringLiteral("/deskflow-update"));
    work.mkpath(QStringLiteral("."));
    const auto zipPath = work.filePath(QStringLiteral("deskflow-jeffmod-win64.zip"));
    QFile zip(zipPath);
    if (!zip.open(QIODevice::WriteOnly) || zip.write(reply->readAll()) < 0) {
      QMessageBox::warning(m_parent, tr("Update Deskflow"), tr("Could not save the download to %1").arg(zipPath));
      return;
    }
    zip.close();
    install(zipPath);
  });
}

void UpdateInstaller::install([[maybe_unused]] const QString &zipPath)
{
#ifdef Q_OS_WIN
  const QDir work(QDir::tempPath() + QStringLiteral("/deskflow-update"));
  const auto scriptPath = work.filePath(QStringLiteral("apply-update.ps1"));
  QFile script(scriptPath);
  if (!script.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QMessageBox::warning(m_parent, tr("Update Deskflow"), tr("Could not write %1").arg(scriptPath));
    return;
  }
  script.write(kUpdateScript.toUtf8());
  script.close();

  const auto installDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
  const auto args = QStringLiteral("-NoProfile -ExecutionPolicy Bypass -File \"%1\" -Zip \"%2\" -InstallDir \"%3\"")
                        .arg(QDir::toNativeSeparators(scriptPath), QDir::toNativeSeparators(zipPath), installDir);

  // "runas" shows the UAC prompt; the script stops this app, so no need to quit first
  const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
      nullptr, L"runas", L"powershell.exe", reinterpret_cast<LPCWSTR>(args.utf16()), nullptr, SW_HIDE
  ));
  if (result <= 32) {
    QMessageBox::warning(
        m_parent, tr("Update Deskflow"), tr("The update was not started (permission was not granted).")
    );
    return;
  }
  QApplication::quit();
#endif
}
