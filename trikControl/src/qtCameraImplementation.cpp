/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include "qtCameraImplementation.h"
#include "cameraDeviceInterface.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraDevice>
#include <QMediaDevices>
#include <QtMultimedia/QImageCapture>
#include <QtMultimedia/QMediaCaptureSession>

#include <QsLog.h>

using namespace trikControl;

QtCameraImplementation::QtCameraImplementation(const QString & port)
{
	QLOG_INFO() << "Available cameras:" << QMediaDevices::videoInputs().count();
	for (auto & cameraInfo : QMediaDevices::videoInputs()) {
		if (cameraInfo.description() == port) {
				decltype(mCamera) tmp(new QCamera(cameraInfo));
				tmp.swap(mCamera);
				break;
		}
	}

	if (!mCamera) {
		QLOG_ERROR() << "Failed to initialize camera for " << port
				<< " from available cameras" << QMediaDevices::videoInputs();
	}
}

QVector<uint8_t> QtCameraImplementation::getPhoto()
{
	QMediaCaptureSession captureSession;
	auto camera = new QCamera;
	captureSession.setCamera(camera);

	auto imageCapture = new QImageCapture();
	captureSession.setImageCapture(imageCapture);

	camera->start();
	//on shutter button pressed
	QVector<uint8_t> imageByteVector;
	imageByteVector.append(imageCapture->capture());
	return imageByteVector;
	/*
	if(!mCamera)
		return QVector<uint8_t>();

	QScopedPointer<QCameraImageCapture> imageCapture (new QImageCameraCapture(mCamera));

	imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

	const auto & formats = imageCapture->supportedFormats();
	QLOG_INFO() << "Supported buffer formats: " << formats;

	auto camera = mCamera.data();
	QVector<uint8_t> imageByteVector;
	QTimer watchdog;
	QEventLoop eventLoop;

	QObject::connect(imageCapture.data(), &QCameraImageCapture::readyForCaptureChanged
		, &eventLoop, [this, &imageCapture, camera](bool ready) {
			if (ready) {
				camera->searchAndLock();
				imageCapture->capture(getTempDir().filePath("photo.jpg"));
				camera->unlock();
			}
		}
	);

	QObject::connect(imageCapture.data(), &QCameraImageCapture::imageCaptured
			, &eventLoop, [&imageByteVector] (int, const QImage &imgOrig) {
				imageByteVector = CameraDeviceInterface::qImageToQVector(imgOrig);
			}
	);

	mCamera->setCaptureMode(QCamera::CaptureStillImage);
	mCamera->start();

	watchdog.setInterval(1000);
	watchdog.setSingleShot(true);
	QObject::connect(&watchdog, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
	QObject::connect(imageCapture.data(), &QCameraImageCapture::imageAvailable
			, &eventLoop, [&eventLoop](int, const QVideoFrame &) {
				eventLoop.quit();
			}
		);

	eventLoop.exec();
	watchdog.stop();

	return imageByteVector;
	*/
}
