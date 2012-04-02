#include "SetupWizard.h"
#include "MainWindow.h"

#include <QMessageBox>
#include <iostream>

SetupWizard::SetupWizard(MainWindow& mainWindow, bool startMain) :
	m_MainWindow(mainWindow),
	m_StartMain(startMain)
{
	setupUi(this);

#if !defined(Q_OS_WIN)
	m_pServiceRadioButton->setEnabled(false);
	m_pServiceRadioButton->setText(tr("Service (Windows only)"));
	m_pServiceLabel->setEnabled(false);
	m_pDesktopRadioButton->setChecked(true);
#endif

	connect(this, SIGNAL(finished(int)), this, SLOT(handlefinished()));
	connect(m_pServerRadioButton, SIGNAL(toggled(bool)), m_MainWindow.m_pGroupServer, SLOT(setChecked(bool)));
	connect(m_pClientRadioButton, SIGNAL(toggled(bool)), m_MainWindow.m_pGroupClient, SLOT(setChecked(bool)));
}

SetupWizard::~SetupWizard()
{
}

bool SetupWizard::validateCurrentPage()
{	
	QMessageBox message;
	message.setWindowTitle(tr("Setup Synergy"));
	message.setIcon(QMessageBox::Information);

	bool result = false;
	if (currentPage() == m_pNodePage)
	{
		result = m_pClientRadioButton->isChecked() ||
				 m_pServerRadioButton->isChecked();

		if (!result)
		{
			message.setText(tr("Please select an option."));
			message.exec();
		}
	}
	else if(currentPage() == m_pStartupPage)
	{
		result = m_pServiceRadioButton->isChecked() ||
				 m_pDesktopRadioButton->isChecked() ||
				 m_pNoneRadioButton->isChecked();

		if (!result)
		{
			message.setText(tr("Please select an option."));
			message.exec();
		}
	}
	return result;
}

void SetupWizard::handlefinished()
{
	close();

	AppConfig& appConfig = m_MainWindow.appConfig();

	if (m_pServiceRadioButton->isChecked())
	{
		appConfig.setProcessMode(Service);
	}
	else if (m_pDesktopRadioButton->isChecked())
	{
		appConfig.setProcessMode(Desktop);
		appConfig.setAutoStart(true);
		appConfig.setAutoConnect(true);
	}
	else if (m_pNoneRadioButton->isChecked())
	{
		// still use desktop mode, but don't auto start.
		appConfig.setProcessMode(Desktop);
	}

	appConfig.setWizardHasRun(true);

	appConfig.saveSettings();

	if (m_StartMain)
	{
		m_MainWindow.start();
	}
}
