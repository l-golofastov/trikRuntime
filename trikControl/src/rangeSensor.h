/* Copyright 2014 CyberTech Labs Ltd.
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

#include <QtCore/QThread>

#include "sensor.h"

namespace trikControl {

class RangeSensorWorker;

/// TRIK range sensor.
class RangeSensor : public Sensor
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param eventFile - event file for this sensor.
	RangeSensor(QString const &eventFile);

	~RangeSensor() override;

signals:
	/// Emitted when new data is received from a sensor.
	void newData(int distance, int rawDistance);

public slots:
	/// Initializes sensor and begins receiving events from it.
	void init();

	/// Returns current raw reading of a sensor.
	int read() override;

	/// Returns current real raw reading of a sensor.
	int readRawData() override;

	/// Stops sensor until init() will be called again.
	void stop();

private:
	/// Worker object that handles sensor in separate thread.
	QScopedPointer<RangeSensorWorker> mSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}