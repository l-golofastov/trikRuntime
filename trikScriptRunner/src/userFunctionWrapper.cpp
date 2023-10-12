#include "userFunctionWrapper.h"

#include "scriptEngineWorker.h"
#include <QTimer>

#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtCore/QTextStream>
#include <QtCore/QMetaMethod>
#include <QtCore/QStringBuilder>
#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include "trikScriptRunnerInterface.h"
#include "scriptable.h"
#include "utils.h"
#include "userFunctionWrapper.h"

using namespace trikScriptRunner;

constexpr auto scriptEngineWorkerName = "__scriptEngineWorker";

UserFunctionWrapper::UserFunctionWrapper(QJSEngine * engine)
	: mEngine(engine) {}

QJSValue UserFunctionWrapper::getUserFunctionValue()
{
	auto engine = qjsEngine(this);
	if(!engine) return QJSValue();

	userFunctionValue = engine->evaluate("function(){ return this.userFunction(); }");

	return userFunctionValue;
}

QJSValue UserFunctionWrapper::include(QJSValue args)
{
	QJSValueList context = ScriptEngineWorker::toJSValueList(args);
	const auto &filename = context.value(0).toString();

	const auto & scriptValue = mEngine->globalObject().property(scriptEngineWorkerName);
	if (auto scriptWorkerValue = qobject_cast<ScriptEngineWorker *> (scriptValue.toQObject())) {
		auto connection = (QThread::currentThread() != mEngine->thread()) ?
					Qt::BlockingQueuedConnection : Qt::DirectConnection;
		QMetaObject::invokeMethod(scriptWorkerValue, [scriptWorkerValue, filename, this]()
					{scriptWorkerValue->evalInclude(filename, mEngine);}, connection);
	}

	return QJSValue();
}

QJSValue UserFunctionWrapper::print(QJSValue arg)
{
	QJSValueList context;
	context << arg;
	QString result;
	result.reserve(100000);
	int argumentCount = context.size();
	for (int i = 0; i < argumentCount; ++i) {
		std::function<QString(const QVariant &)> prettyPrinter
			= [&prettyPrinter](QVariant const & elem) {
			auto const &arrayPrettyPrinter = [&prettyPrinter](const QVariantList &array) {
				qint32 arrayLength = array.length();

				if (arrayLength == 0) {
					return QString("[]");
				}

				QString res;
				res.reserve(100000);
				res.append("[" % prettyPrinter(array.first()));

				for(auto i = 1; i < arrayLength; ++i) {
					res.append(", " % prettyPrinter(array.at(i)));
				}

				res.append("]");
				return res;
			};

			return (elem.canConvert(QMetaType::QVariantList) && elem.userType() != QMetaType::QString)
				? arrayPrettyPrinter(elem.toList())
				: elem.toString();
		};
		QJSValue argument = context.value(i);
		result.append(prettyPrinter(argument.toVariant()));
	}

	auto scriptValue = mEngine->globalObject().property("script");

	if (auto script = qobject_cast<TrikScriptControlInterface*> (scriptValue.toQObject())) {
		result.append('\n');
		QTimer::singleShot(0, script, [script, result](){ Q_EMIT script->textInStdOut(result);});
		/// In case of user loop with `print' this gives some time for events to be processed
		script->wait(0);
	}

	return mEngine->toScriptValue(result);
}
