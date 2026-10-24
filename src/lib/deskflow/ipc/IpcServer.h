/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025-2026 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <QHash>
#include <QObject>
#include <QSet>

class QLocalServer;
class QLocalSocket;

namespace deskflow::core::ipc {

class IpcServer : public QObject
{
  Q_OBJECT

public:
  explicit IpcServer(QObject *parent, const QString &serverName, const QString &typeName);
  ~IpcServer() override;

  void listen();
  void broadcastCommand(const QString &command, const QString &args = "");

Q_SIGNALS:
  void logLevelChanged(const QString &logLevel);
  void configFileChanged(const QString &configFile);
  void startProcessRequested();
  void stopProcessRequested();
  void clearSettingsRequested();

protected:
  /**!
   * Write a message to the client socket and append a newline character.
   *
   * \param clientSocket The client socket to write to.
   * \param message The message to write (without trailing newline).
   */
  void writeToClientSocket(QLocalSocket *&clientSocket, const QString &message) const;

private:
  void processMessage(QLocalSocket *clientSocket, const QString &message);
  virtual void processCommand(QLocalSocket *clientSocket, const QString &command, const QStringList &parts) = 0;
  void handleNewConnection();
  void handleReadyRead();
  void handleDisconnected();
  void handleErrorOccurred();

  QLocalServer *m_server;
  QSet<QLocalSocket *> m_clients;
  QString m_serverName;
  QStringList m_pendingMessages;
  QByteArray m_typeName;

  // latest value of each "sticky" (state-describing) command, kept up to date on every
  // broadcast regardless of whether it actually reached a live client, and replayed in
  // full to every newly-connecting client so a client that connects (or reconnects) after
  // a state change -- including one whose delivery silently failed to a stale socket --
  // still ends up in sync instead of stuck showing whatever it last knew.
  QHash<QString, QString> m_lastStickyMessage;
};

} // namespace deskflow::core::ipc
