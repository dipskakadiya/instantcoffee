/*=============================================================================
 Copyright (c) 2010, Mihail Szabolcs
 All rights reserved.

 Redistribution and use in source and binary forms, with or
 without modification, are permitted provided that the following
 conditions are met:

   * 	Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

   * 	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in
		the documentation and/or other materials provided with the
		distribution.

   * 	Neither the name of the Instant Coffee nor the names of its contributors
		may be used to endorse or promote products derived from this
		software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	THE POSSIBILITY OF SUCH DAMAGE.

	This file is part of Instant Coffee.

==============================================================================*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QWebFrame>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *pParent) :
	QMainWindow(pParent),
	mUi(new Ui::MainWindow)
{
	mUi->setupUi(this);

	//! Attach API
	connect( mUi->webView->page()->mainFrame(),
			 SIGNAL(javaScriptWindowObjectCleared()),
			 this,
			 SLOT(attachAPI()) );

	QString lPath = QString("%1/showdown-v0.9/example/showdown-gui.html").
					arg(QApplication::applicationDirPath());

	qDebug("UI: %s",static_cast<const char *>(lPath.toAscii()));
	mUi->webView->setUrl(QUrl(lPath));
}

MainWindow::~MainWindow()
{
	delete mUi;
}

void MainWindow::changeEvent(QEvent *pEvent)
{
	QMainWindow::changeEvent(pEvent);
	switch (pEvent->type())
	{
		case QEvent::LanguageChange:
			mUi->retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::closeEvent(QCloseEvent *pEvent)
{
	if(QMessageBox::information(this,
								"Instant Coffee",
								"Are you sure you want to close Instant Coffee now?",
								QMessageBox::Yes,
								QMessageBox::No,0) == QMessageBox::Yes)
	{
		pEvent->accept();
		return;
	}

	pEvent->ignore();
}

void MainWindow::attachAPI(void)
{
	mUi->webView->page()->mainFrame()->addToJavaScriptWindowObject( tr("Coffee"), this );
}

void MainWindow::newFile(void)
{
	mUi->webView->page()->currentFrame()->evaluateJavaScript("newText();");
	mFileName = "";
	setWindowTitle("Instant Coffee [Untitled Markdown]");
}

void MainWindow::openFile(void)
{
	QString lOpenFileName = QFileDialog::getOpenFileName(this,"Open ...");
	if(lOpenFileName.isEmpty())
		return;

	QString lJs = QString("openText('%1');").arg(lOpenFileName);
	mUi->webView->page()->currentFrame()->evaluateJavaScript(lJs);

	QFileInfo lFileInfo(lOpenFileName);
	if(lFileInfo.exists())
	{
		mFileName = lOpenFileName;
		setWindowTitle(QString("Instant Coffee [%1]").arg(lFileInfo.fileName()));
	}
}

void MainWindow::saveFile(void)
{
	if(mFileName.isEmpty())
	{
		saveAsFile();
		return;
	}

	saveText(mFileName);
}

void MainWindow::saveAsFile(void)
{
	QString lSaveFileName = QFileDialog::getSaveFileName(this,"Save As ...");
	if(lSaveFileName.isEmpty())
		return;

	if(saveText(lSaveFileName))
	{
		QFileInfo lFileInfo(lSaveFileName);
		if(lFileInfo.exists())
		{
			mFileName = lSaveFileName;
			setWindowTitle(QString("Instant Coffee [%1]").arg(lFileInfo.fileName()));
		}
	}
}

void MainWindow::exportHTML(void)
{
	QString lSaveFileName = QFileDialog::getSaveFileName(this,"Export HTML ...");
	if(lSaveFileName.isEmpty())
		return;

	saveHTML(lSaveFileName);
}

void MainWindow::about(void)
{
	QMessageBox::information(this,
							 "Instant Coffee",
							 "Instant Coffee v0.1\nCopyright (c) 2010, Mihail Szabolcs");
}

QString MainWindow::fileContent(const QString &pFileName)
{
	QFile lFile(pFileName);
	if(lFile.open(QFile::ReadOnly | QFile::Text))
		return lFile.readAll();

	return "";
}

bool MainWindow::setFileContent(const QString &pFileName, const QString &pContent)
{
	QFile lFile(pFileName);
	if(lFile.open(QFile::WriteOnly | QFile::Text))
		return (lFile.write(pContent.toAscii())==pContent.size());

	return false;
}

bool MainWindow::saveText(const QString &pFileName)
{
	QString lJs = QString("saveText('%1');").arg(pFileName);
	return mUi->webView->page()->currentFrame()->evaluateJavaScript(lJs).toBool();
}

bool MainWindow::saveHTML(const QString &pFileName)
{
	QString lJs = QString("saveHTML('%1');").arg(pFileName);
	return mUi->webView->page()->currentFrame()->evaluateJavaScript(lJs).toBool();
}
