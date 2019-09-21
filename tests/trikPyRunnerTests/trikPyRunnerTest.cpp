/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include "trikPyRunnerTest.h"

#include <QtCore/QEventLoop>
#include <QtCore/QFile>

#include <trikControl/brickFactory.h>
#include <trikKernel/fileUtils.h>
#include <testUtils/wait.h>
#include <QTimer>


using namespace tests;
constexpr auto EXIT_TIMEOUT = -93;
constexpr auto EXIT_SCRIPT_ERROR = 113;

void TrikPyRunnerTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
				   , "./test-model-config.xml", "./media/"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
	mScriptRunner->setDefaultRunner(trikScriptRunner::ScriptType::PYTHON);
// TODO:	mScriptRunner->registerUserFunction("assert", scriptAssert);
}

void TrikPyRunnerTest::TearDown()
{
}

int TrikPyRunnerTest::run(const QString &script)
{
	QEventLoop l;
	QTimer::singleShot(5000, &l, std::bind(&QEventLoop::exit, &l, EXIT_TIMEOUT));
	QObject::connect(&*mScriptRunner, &trikScriptRunner::TrikScriptRunnerInterface::completed, &l, &QEventLoop::quit);
	mScriptRunner->run(script, "_.py");
	auto exitCode = l.exec();
	return exitCode;
}

int TrikPyRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop l;
	QObject::connect(&*mScriptRunner, &trikScriptRunner::TrikScriptRunnerInterface::completed, &l, &QEventLoop::quit);
	mScriptRunner->runDirectCommand(script);
	l.exec();
	return mScriptRunner->wasError()? EXIT_SCRIPT_ERROR : EXIT_SUCCESS;
}

int TrikPyRunnerTest::runFromFile(const QString &fileName)
{
	auto fileContents = trikKernel::FileUtils::readFromFile("data/" + fileName);

#ifdef Q_OS_WIN
	fileContents = fileContents.replace("&&", ";");
#endif

	return run(fileContents);
}

trikScriptRunner::TrikScriptRunner &TrikPyRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikPyRunnerTest, abortBeforeRun)
{
	scriptRunner().abortAll();
}


TEST_F(TrikPyRunnerTest, sanityCheckPy)
{
	auto err = run("1 + 1");
	ASSERT_EQ(err, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, print)
{
	/// TODO: Check stdout, probably via sendMessage signal
	auto err = runDirectCommandAndWaitForQuit("print('Hello')");
	ASSERT_EQ(err, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, abortWhileTrue)
{
	QTimer t;
	t.setInterval(200);
	t.setSingleShot(true);
	using trikScriptRunner::TrikScriptRunnerInterface;
	QObject::connect(&scriptRunner(), &TrikScriptRunnerInterface::startedScript
					 , &t, QOverload<>::of(&QTimer::start));
	QObject::connect(&t, &QTimer::timeout, &scriptRunner(), &TrikScriptRunnerInterface::abort);
	auto err = run("print('before')\nwhile True: pass\nprint('after')");
	ASSERT_NE(err, EXIT_TIMEOUT);
	t.stop();
}

TEST_F(TrikPyRunnerTest, scriptWait)
{
	scriptRunner().run("script.wait(500)");
	tests::utils::Wait::wait(600);
}

TEST_F(TrikPyRunnerTest, directCommandContextWithTimersAndQtCore)
{
	auto err = runDirectCommandAndWaitForQuit("from PythonQt import QtCore");
	ASSERT_EQ(err, EXIT_SUCCESS);
	err = runDirectCommandAndWaitForQuit("QtCore.QTimer.singleShot(100, lambda _ : None)");
	ASSERT_EQ(err, EXIT_SUCCESS);
	err = runDirectCommandAndWaitForQuit("t=QtCore.QTimer()");
	ASSERT_EQ(err, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, propertyAndMethodWithSimpleType)
{
	auto exitCode = run("brick.gyroscope().read()");
	ASSERT_EQ(exitCode, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, brickMethodWithNonTrivialReturnTypeConversion)
{
	auto exitCode = run("brick.getStillImage()");
	ASSERT_EQ(exitCode, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, brickPropertyAndVectorArgument)
{
	auto exitCode = run("brick.display().show([0], 1, 1, 'grayscale8')");
	ASSERT_EQ(exitCode, EXIT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, DISABLED_fileTestPy)
{
	auto err = runFromFile("file-test.py");
	ASSERT_EQ(err, EXIT_SUCCESS);
}
