#!/usr/bin/env bash
#Copyright (C) 2024 The Qt Company Ltd
#SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set -ex

# binutlis is installed with an error: The formula built, but is not symlinked into /usr/local
# To avoid stopping the configuration due to this problem, "|| true" is added.
brew install binutils || true

