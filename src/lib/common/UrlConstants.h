/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2024 Synergy App Ltd
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <QString>

// important: this is used for settings paths on some platforms,
// and must not be a url. qt automatically converts this to reverse domain
// notation (rdn), e.g. org.deskflow
const auto kOrgDomain = QStringLiteral("deskflow.org");

const auto kUrlSourceQuery = QStringLiteral("source=gui");
const auto kUrlApp = QStringLiteral("https://%1").arg(kOrgDomain);
const auto kUrlHelp = QStringLiteral("%1/help?%2").arg(kUrlApp, kUrlSourceQuery);
const auto kUrlDownload = QStringLiteral("%1/download?%2").arg(kUrlApp, kUrlSourceQuery);
const auto kUrlWiki = QStringLiteral("%1/wiki?%2").arg(kUrlApp, kUrlSourceQuery);
// Jeff Mod: updates come from the fork instead of deskflow.org. The jeff-mod-release workflow publishes the
// rolling "jeff-mod" release: version.txt holds a plain version string (e.g. 1.27.0.30) and the zip holds the
// Windows build that the in-app updater installs.
const auto kUrlForkRepo = QStringLiteral("https://github.com/jlansman/deskflow/tree/custom/main");
const auto kUrlForkRelease = QStringLiteral("https://github.com/jlansman/deskflow/releases/download/jeff-mod");
const auto kUrlUpdateCheck = QStringLiteral("%1/version.txt").arg(kUrlForkRelease);
const auto kUrlUpdatePackage = QStringLiteral("%1/deskflow-jeffmod-win64.zip").arg(kUrlForkRelease);
// commits on the real Deskflow main branch that custom/main does not have yet
const auto kUrlUpstreamAhead =
    QStringLiteral("https://api.github.com/repos/jlansman/deskflow/compare/custom/main...deskflow:master");
const auto kUrlUpstreamAheadPage =
    QStringLiteral("https://github.com/jlansman/deskflow/compare/custom/main...deskflow:master");

#if defined(Q_OS_LINUX)
const auto kUrlGnomeTrayFix = QStringLiteral("https://extensions.gnome.org/extension/615/appindicator-support/");
#endif
