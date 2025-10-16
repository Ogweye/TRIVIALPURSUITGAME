#pragma once

#include "afxwin.h"
#include <vector>
#include <map>
#include <string>

class CTriviallogicGuiDlg : public CDialogEx
{
public:
    CTriviallogicGuiDlg(CWnd* pParent = nullptr);   // standard constructor

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TRIVIALLOGICGUI_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

protected:
    HICON m_hIcon;

    // player / game state
    CString m_playerName;
    int m_lives;
    int m_score;
    int m_maxQuestions;
    int m_currentQuestionIndex;
    CString m_currentCategory;

    // question storage
    std::map<CString, std::vector<CString>> m_categoryQuestions;
    std::map<CString, std::vector<CString>> m_categoryAnswers;

    // active list when a category (or Random) is chosen
    std::vector<CString> m_currentQuestions;
    std::vector<CString> m_currentAnswers;

    // Controls (required ones)
    CEdit   m_editName;
    CButton m_btnSubmit;
    CButton m_btnPlay;
    CButton m_btnAdmin;
    CStatic m_staticQuestion;
    CEdit   m_editAnswer;
    CButton m_btnAnswerSubmit;
    CStatic m_staticLives;
    CStatic m_staticScore;
    CButton m_btnRoll;
    CStatic m_staticEnterName;
    CButton m_btnRestart;
    CButton m_btnViewHighScores;
    CButton m_btnClearHighScores;
    CButton m_btnBack; // optional back button

    // Admin optional controls (subclassed only if present)
    CEdit   m_editAdminFiles;
    CEdit   m_editAdminQuestion;
    CEdit   m_editAdminMultipleQuestion;
    CEdit   m_editAdminDeleteID;
    CButton m_btnAddSingleQuestion;
    CButton m_btnAddMultipleQuestions;
    CButton m_btnRemoveQuestion;

    // Score entry
    struct ScoreEntry {
        CString name;
        int score;
    };

    // background image
    class CImage* m_pBgImagePtr; // pointer to avoid heavy include in header

protected:
    virtual BOOL OnInitDialog() override;
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    DECLARE_MESSAGE_MAP()

public:
    // handlers
    afx_msg void OnBnClickedBtnSubmit();
    afx_msg void OnBnClickedBtnAdmin();
    afx_msg void OnBnClickedBtnPlay();
    afx_msg void OnBnClickedBtnAnswerSubmit();
    afx_msg void OnBnClickedBtnRestart();
    afx_msg void OnBnClickedBtnRoll();

    afx_msg void OnBnClickedBtnViewHighScores();
    afx_msg void OnBnClickedBtnClearHighScores();

    afx_msg void OnBnClickedBtnAddSingleQuestion();
    afx_msg void OnBnClickedBtnAddMultipleQuestions();
    afx_msg void OnBnClickedBtnRemoveQuestion();

    afx_msg void OnBnClickedBtnBack();

    // background paint and control coloring
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

    // game helpers
    void ShowNextQuestion();
    void LoadQuestionsFromCSV(const CString& filePath);
    void ShuffleVector(std::vector<CString>& qVec, std::vector<CString>& aVec);

    // high score functions
    void AddHighScore(const CString& playerName, int score); // will update existing player if higher score
    void ViewHighScores();
    void ClearHighScores();

    // admin UI helper
    void ShowAdminControls(BOOL show);
    void ShowHomeScreen();

    afx_msg void OnStnClickedStaticQuestionFile();
};
