/* Copyright 2013 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <qglobal.h>

#include <QtQml/QJSEngine>

class QJSEngine;

namespace trikScriptRunner {

/// Helper class that registers converters from and to script values for a given script engine.
template<typename T> class Scriptable
{
public:
	Scriptable() = delete;

	/// Registers converters from and to script values for a given script engine.
	static void registerMetatype(QJSEngine *engine)
	{
		Q_UNUSED(engine)
	}

private:
	static QJSValue toScriptValue(QJSEngine *engine, T* const &in)
	{
		Q_UNUSED(engine)
		return engine->newQObject(in);
	}

	static void fromScriptValue(const QJSValue &object, T* &out)
	{
		out = qobject_cast<T*>(object.toQObject());
	}
};

}
