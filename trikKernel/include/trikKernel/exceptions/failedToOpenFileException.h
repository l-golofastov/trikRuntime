/* Copyright 2015 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/QFile>

#include <QsLog.h>

#include "trikRuntimeException.h"

namespace trikKernel {

/// Exception that is thrown when file opening operation failed.
class FailedToOpenFileException : public TrikRuntimeException
{
public:
	/// Constructor.
	/// @param file - file that is failed to open.
	FailedToOpenFileException(QFile const &file)
		: mFile(file)
	{
		QLOG_ERROR() << "Failed to open file" << file.fileName()
				<< (file.openMode() | QIODevice::WriteOnly ? "for writing" : "for reading");
	}

	/// Returns file that is failed to open.
	QFile file() const
	{
		return mFile;
	}

private:
	QFile const mFile;
};

}
