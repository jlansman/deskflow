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
// Jeff Mod: update notices come from the fork instead of deskflow.org. The check expects a plain version
// string (e.g. 1.27.0.30); bump jeff-mod-version.txt on custom/main to make installed copies show "Update available".
const auto kUrlForkRepo = QStringLiteral("https://github.com/jlansman/deskflow/tree/custom/main");
const auto kUrlUpdateCheck =
    QStringLiteral("https://raw.githubusercontent.com/jlansman/deskflow/custom/main/jeff-mod-version.txt");

#if defined(Q_OS_LINUX)
const auto kUrlGnomeTrayFix = QStringLiteral("https://extensions.gnome.org/extension/615/appindicator-support/");
#endif
