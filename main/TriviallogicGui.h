
// TriviallogicGui.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTriviallogicGuiApp:
// See TriviallogicGui.cpp for the implementation of this class
//

class CTriviallogicGuiApp : public CWinApp
{
public:
	CTriviallogicGuiApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnAdmin();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnAnswerSubmit();
	afx_msg void OnBnClickedBtnClearHighscores();
	afx_msg void OnBnClickedBtnViewHighscores();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedBtnAddMultipleQuestions();
	afx_msg void OnBnClickedBtnAddSingleQuestion();
	afx_msg void OnBnClickedBtnRemoveQuestion();
	afx_msg void OnBnClickedButtonBack();
};

extern CTriviallogicGuiApp theApp;
