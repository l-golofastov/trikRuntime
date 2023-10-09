/* Copyright 2014 - 2015 Dmitry Mordvinov, CyberTech Labs Ltd.
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

#include "utils.h"

#include <QtCore/QDateTime>
#include <QRegularExpression>
#include <QtQml/QJSValueIterator>

using namespace trikScriptRunner;

QJSValue Utils::clone(const QJSValue &prototype, QJSEngine * const engine)
{
	QJSValue copy;
	if (prototype.isCallable()) {
		// Functions can not be copied across script engines, so they actually will not be copied.
		return prototype;
	} else if (prototype.isArray()) {
		copy = engine->newArray();
		QJSValueIterator iterator(prototype);
		int i = 0;
		while (iterator.hasNext()) {
			copy.setProperty(i, engine->toScriptValue(iterator.value()));
			iterator.next();
			i++;
		}
	} else if (prototype.isBool()) {
		copy = QJSValue(prototype.toBool());
	} else if (prototype.isNumber()) {
		copy = QJSValue(prototype.toNumber());
	} else if (prototype.isString()) {
		copy = QJSValue(prototype.toString());
	} else if (prototype.isRegExp()) {
		auto prototypeRegExp = engine->fromScriptValue<QRegularExpression>(prototype);
		copy = engine->toScriptValue(prototypeRegExp);
	} else if (prototype.isDate()) {
		copy = engine->toScriptValue(prototype.toDateTime());
	} else if (prototype.isQObject()) {
		copy = engine->newQObject(prototype.toQObject());
	} else if (prototype.isQMetaObject()) {
		copy = engine->newQMetaObject(prototype.toQMetaObject());
	} else if (prototype.isNull()) {
		copy = QJSValue();
	} else if (prototype.isObject()) {
		if (prototype.toString() == "[object Math]"
				|| prototype.toString() == "[object Object]"
				|| prototype.toString() == "[object JSON]"
				)
		{
			// Do not copy intrinsic objects, we will not be able to copy their functions properly anyway.
			return prototype;
		}

		copy = engine->newObject();
		QJSValueIterator iterator(prototype);
		while (iterator.hasNext()) {
			copy.setProperty(iterator.name(), engine->toScriptValue(iterator.value()));
			iterator.next();
		}

	} else {
		copy = prototype;
	}

	copyRecursivelyTo(prototype, copy, engine);
	return copy;
}

bool Utils::hasProperty(const QJSValue &object, const QString &property)
{
	QJSValueIterator iterator(object);
	while (iterator.hasNext()) {
		iterator.next();
		if (iterator.name() == property) {
			return true;
		}
	}

	return false;
}

void Utils::copyRecursivelyTo(const QJSValue &prototype, QJSValue &target, QJSEngine *engine)
{
	QJSValueIterator iterator(prototype);
	while (iterator.hasNext()) {
		iterator.next();
		QJSValue const value = clone(iterator.value(), engine);
		// Functions will not be copied to a new engine.
		if (engine) {
			target.setProperty(iterator.name(), value);
		}
	}
}
