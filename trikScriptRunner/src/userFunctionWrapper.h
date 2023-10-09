#pragma once

#include <QObject>
#include <trikScriptRunnerInterface.h>
#include "scriptEngineWorker.h"

namespace trikScriptRunner {

/// This class is used for wrapping given C++ function as invokable from script.
/// Called only when evalSystemJs() is processing.
class UserFunctionWrapper : public QObject
{
	Q_OBJECT

public:
	/// Consctructor.
	/// @param function - C++ function to wrap.
	explicit UserFunctionWrapper(QJSEngine * engine = nullptr);

	Q_INVOKABLE TrikScriptRunnerInterface::script_function_type userFunction;

	Q_INVOKABLE QJSValue include(QJSValue args);

	Q_INVOKABLE QJSValue print(QJSValue args);

	QJSValue userFunctionValue;

	QJSValue getUserFunctionValue();

	QJSEngine *mEngine;
};

}
