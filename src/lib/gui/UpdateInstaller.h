/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QWidget;

//! Downloads the latest Jeff Mod build and installs it (Windows only).
/*!
Downloads the zip published by the jeff-mod-release workflow, then starts an elevated
PowerShell script that stops Deskflow, copies the files over the install folder, restarts
the service and relaunches the app. On other platforms it just opens the fork's page.
*/
class UpdateInstaller : public QObject
{
  Q_OBJECT
public:
  explicit UpdateInstaller(QWidget *parent);

  //! Ask for confirmation, then download and install \p version.
  void start(const QString &version);

private:
  void download();
  void install(const QString &zipPath);

  QWidget *m_parent;
  QNetworkAccessManager *m_network;
};
