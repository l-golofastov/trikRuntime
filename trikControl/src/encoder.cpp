/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "encoder.h"

#include <trikKernel/configurer.h>

#include "i2cCommunicator.h"

using namespace trikControl;

Encoder::Encoder(QString const &port, trikKernel::Configurer const &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
	, mI2cCommandNumber(configurer.attributeByPort(port, "i2cCommandNumber").toInt(nullptr, 0))
	, mRawToDegrees(configurer.attributeByPort(port, "rawToDegrees").toDouble())
{
}

void Encoder::reset()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber);
	command[1] = static_cast<char>(0x00);

	mCommunicator.send(command);
}

int Encoder::read()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber);
	int data = mCommunicator.read(command);

	return mRawToDegrees * data;
}

int Encoder::readRawData()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber);

	return mCommunicator.read(command);
}
