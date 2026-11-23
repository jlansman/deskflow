/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2012 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class VersionChecker : public QObject
{
  Q_OBJECT
public:
  explicit VersionChecker(QObject *parent = nullptr);
  void checkLatest() const;
  void checkUpstream();
public Q_SLOTS:
  void replyFinished(QNetworkReply *reply);
Q_SIGNALS:
  void updateFound(const QString &version);
  void upstreamCommitsFound(int count);
  void upToDate();
  void checkFailed(const QString &error);
  void upstreamCheckFailed(const QString &error);

private:
  static int compareVersions(const QString &left, const QString &right);

  /**
   * \brief Converts a string stage to a integer value
   * \param stage The string containing the stage version
   * \return An integer representation of the stage, the higher the number the
   * more recent the version
   */
  static int getStageVersion(QString stage);
  QNetworkAccessManager *m_network = nullptr;
  QNetworkAccessManager *m_upstreamNetwork = nullptr;
};
