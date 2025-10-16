// TriviallogicGuiDlg.cpp : implementation file
#include "pch.h"
#include "framework.h"
#include "TriviallogicGui.h"
#include "TriviallogicGuiDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <numeric>
#include <atlimage.h> // CImage

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ------------------- CONSTRUCTOR -------------------
CTriviallogicGuiDlg::CTriviallogicGuiDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TRIVIALLOGICGUI_DIALOG, pParent)
    , m_pBgImagePtr(nullptr)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_playerName = _T("");
    m_lives = 3;
    m_score = 0;
    m_currentQuestionIndex = 0;
    m_maxQuestions = 10;
}

// ------------------- DDX -------------------
void CTriviallogicGuiDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    // required controls (should exist in your dialog)
    DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
    DDX_Control(pDX, IDC_BTN_SUBMIT, m_btnSubmit);
    DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
    DDX_Control(pDX, IDC_BTN_ADMIN, m_btnAdmin);
    DDX_Control(pDX, IDC_STATIC_QUESTION, m_staticQuestion);
    DDX_Control(pDX, IDC_EDIT_ANSWER, m_editAnswer);
    DDX_Control(pDX, IDC_BTN_ANSWER_SUBMIT, m_btnAnswerSubmit);
    DDX_Control(pDX, IDC_STATIC_LIVES, m_staticLives);
    DDX_Control(pDX, IDC_STATIC_SCORE, m_staticScore);
    DDX_Control(pDX, IDC_BTN_ROLL, m_btnRoll);
    DDX_Control(pDX, IDC_STATIC_ENTERNAME, m_staticEnterName);
    DDX_Control(pDX, IDC_BTN_RESTART, m_btnRestart);
    DDX_Control(pDX, IDC_BTN_VIEW_HIGHSCORES, m_btnViewHighScores);
    DDX_Control(pDX, IDC_BTN_CLEAR_HIGHSCORES, m_btnClearHighScores);

    // NOTE: admin controls are optional. We do not DDX_Control them here to avoid assertion
}

BEGIN_MESSAGE_MAP(CTriviallogicGuiDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BTN_SUBMIT, &CTriviallogicGuiDlg::OnBnClickedBtnSubmit)
    ON_BN_CLICKED(IDC_BTN_ADMIN, &CTriviallogicGuiDlg::OnBnClickedBtnAdmin)
    ON_BN_CLICKED(IDC_BTN_PLAY, &CTriviallogicGuiDlg::OnBnClickedBtnPlay)
    ON_BN_CLICKED(IDC_BTN_ANSWER_SUBMIT, &CTriviallogicGuiDlg::OnBnClickedBtnAnswerSubmit)
    ON_BN_CLICKED(IDC_BTN_RESTART, &CTriviallogicGuiDlg::OnBnClickedBtnRestart)
    ON_BN_CLICKED(IDC_BTN_ROLL, &CTriviallogicGuiDlg::OnBnClickedBtnRoll)
    ON_BN_CLICKED(IDC_BTN_VIEW_HIGHSCORES, &CTriviallogicGuiDlg::OnBnClickedBtnViewHighScores)
    ON_BN_CLICKED(IDC_BTN_CLEAR_HIGHSCORES, &CTriviallogicGuiDlg::OnBnClickedBtnClearHighScores)
    ON_BN_CLICKED(IDC_BTN_ADD_QUESTION, &CTriviallogicGuiDlg::OnBnClickedBtnAddSingleQuestion)
    ON_BN_CLICKED(IDC_BTN_ADD_MULTIPLE, &CTriviallogicGuiDlg::OnBnClickedBtnAddMultipleQuestions)
    ON_BN_CLICKED(IDC_BTN_DELETE_QUESTION, &CTriviallogicGuiDlg::OnBnClickedBtnRemoveQuestion)
    ON_BN_CLICKED(IDC_BUTTON_BACK, &CTriviallogicGuiDlg::OnBnClickedBtnBack)
    ON_STN_CLICKED(IDC_STATIC_QUESTION_FILE, &CTriviallogicGuiDlg::OnStnClickedStaticQuestionFile)
END_MESSAGE_MAP()

// ------------------- INIT -------------------
BOOL CTriviallogicGuiDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // init display
    m_score = 0; m_lives = 3;
    CString tmp; tmp.Format(_T("Score: %d"), m_score); m_staticScore.SetWindowText(tmp);
    tmp.Format(_T("Lives: %d"), m_lives); m_staticLives.SetWindowText(tmp);

    // Load categories (do not load highscores.csv here)
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\sport.csv"));
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\geography.csv"));
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\history.csv"));
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\movie.csv"));
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\science.csv"));
    LoadQuestionsFromCSV(_T("C:\\Users\\USER\\source\\repos\\Triviallogic\\custom.csv"));

    // Hide game controls initially
    m_staticQuestion.ShowWindow(SW_HIDE);
    m_editAnswer.ShowWindow(SW_HIDE);
    m_btnAnswerSubmit.ShowWindow(SW_HIDE);
    m_staticLives.ShowWindow(SW_HIDE);
    m_staticScore.ShowWindow(SW_HIDE);
    m_btnRoll.ShowWindow(SW_HIDE);
    m_btnRestart.ShowWindow(SW_HIDE);

    // Attempt to subclass optional admin controls safely (no DDX for these)
    CWnd* p = nullptr;
    p = GetDlgItem(IDC_EDIT_ADMIN_FILES);
    if (p) m_editAdminFiles.SubclassWindow(p->GetSafeHwnd());
    p = GetDlgItem(IDC_EDIT_ADMIN_QUESTION);
    if (p) m_editAdminQuestion.SubclassWindow(p->GetSafeHwnd());
    p = GetDlgItem(IDC_EDIT_ADMIN_MULTIPLE_QUESTION);
    if (p) m_editAdminMultipleQuestion.SubclassWindow(p->GetSafeHwnd());
    p = GetDlgItem(IDC_EDIT_ADMIN_DELETE_ID);
    if (p) m_editAdminDeleteID.SubclassWindow(p->GetSafeHwnd());

    p = GetDlgItem(IDC_BTN_ADD_QUESTION);
    if (p) m_btnAddSingleQuestion.SubclassWindow(p->GetSafeHwnd());
    p = GetDlgItem(IDC_BTN_ADD_MULTIPLE);
    if (p) m_btnAddMultipleQuestions.SubclassWindow(p->GetSafeHwnd());
    p = GetDlgItem(IDC_BTN_DELETE_QUESTION);
    if (p) m_btnRemoveQuestion.SubclassWindow(p->GetSafeHwnd());

    // back button (optional)
    p = GetDlgItem(IDC_BUTTON_BACK);
    if (p) m_btnBack.SubclassWindow(p->GetSafeHwnd());

    // Hide admin statics if present (safe)
    if (GetDlgItem(IDC_STATIC_FILE)) GetDlgItem(IDC_STATIC_FILE)->ShowWindow(SW_HIDE);
    if (GetDlgItem(IDC_STATIC_QUESTION_FILE)) GetDlgItem(IDC_STATIC_QUESTION_FILE)->ShowWindow(SW_HIDE);
    if (GetDlgItem(IDC_STATIC_MULTIPLE)) GetDlgItem(IDC_STATIC_MULTIPLE)->ShowWindow(SW_HIDE);
    if (GetDlgItem(IDC_STATIC_DELETE)) GetDlgItem(IDC_STATIC_DELETE)->ShowWindow(SW_HIDE);

    // Hide admin controls initially
    ShowAdminControls(FALSE);

    // Disable play/admin until name entered
    m_btnPlay.EnableWindow(FALSE);
    m_btnAdmin.EnableWindow(FALSE);

    // Try to load the background image file (optional)
    // allocate CImage and keep pointer (avoid including CImage in header)
    CImage* img = new CImage();
    HRESULT hr = img->Load(L"C:\\Users\\USER\\source\\repos\\Triviallogic\\background.bmp");
    if (SUCCEEDED(hr)) {
        m_pBgImagePtr = img;
    }
    else {
        delete img;
        m_pBgImagePtr = nullptr;
        // Not fatal - no background
    }

    return TRUE;  // return TRUE unless you set the focus to a control
}

// ------------------- SYSTEM HANDLERS -------------------
void CTriviallogicGuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialogEx::OnSysCommand(nID, lParam);
}

void CTriviallogicGuiDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON), cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect; GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2, y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}


HCURSOR CTriviallogicGuiDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


// ------------------- BACKGROUND & CONTROL COLOR -------------------
BOOL CTriviallogicGuiDlg::OnEraseBkgnd(CDC* pDC)
{
    if (m_pBgImagePtr && !m_pBgImagePtr->IsNull())
    {
        CRect rect;
        GetClientRect(&rect);
        // Draw stretched to client rect
        m_pBgImagePtr->Draw(*pDC, rect);
        return TRUE; // we've handled background
    }
    return CDialogEx::OnEraseBkgnd(pDC);
}

HBRUSH CTriviallogicGuiDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (nCtlColor == CTLCOLOR_STATIC)
    {
        pDC->SetTextColor(RGB(255, 255, 255));  // White text
        pDC->SetBkMode(TRANSPARENT);

        // Use dialog background brush so text doesn't overlap
        static CBrush brush(RGB(0, 0, 0)); // or your background color
        return brush;
    }

    if (nCtlColor == CTLCOLOR_EDIT)
    {
        pDC->SetTextColor(RGB(255, 255, 255));
        pDC->SetBkColor(RGB(30, 30, 30));
        static CBrush brushEdit(RGB(30, 30, 30));
        return brushEdit;
    }

    return hbr;
}

// ------------------- BUTTON HANDLERS -------------------
void CTriviallogicGuiDlg::OnBnClickedBtnSubmit()
{
    CString name; m_editName.GetWindowText(name); name.Trim();
    if (name.IsEmpty()) { AfxMessageBox(_T("Please enter a valid name.")); return; }

    m_playerName = name;
    AfxMessageBox(_T("Welcome, ") + m_playerName);

    m_btnPlay.EnableWindow(TRUE);
    m_btnAdmin.EnableWindow(TRUE);
}

void CTriviallogicGuiDlg::OnBnClickedBtnAdmin()
{
    static bool visible = false;
    visible = !visible;
    ShowAdminControls(visible);
}

void CTriviallogicGuiDlg::OnBnClickedBtnPlay()
{
    // hide admin and home UI
    ShowAdminControls(FALSE);

    m_editName.ShowWindow(SW_HIDE); m_btnSubmit.ShowWindow(SW_HIDE);
    m_btnPlay.ShowWindow(SW_HIDE); m_btnAdmin.ShowWindow(SW_HIDE);
    m_staticEnterName.ShowWindow(SW_HIDE);
    m_btnViewHighScores.ShowWindow(SW_HIDE);
    m_btnClearHighScores.ShowWindow(SW_HIDE);
    if (m_btnBack.GetSafeHwnd()) m_btnBack.ShowWindow(SW_SHOW);

    // reveal game UI
    m_staticQuestion.ShowWindow(SW_SHOW);
    m_editAnswer.ShowWindow(SW_SHOW);
    m_btnAnswerSubmit.ShowWindow(SW_SHOW);
    m_staticLives.ShowWindow(SW_SHOW);
    m_staticScore.ShowWindow(SW_SHOW);
    m_btnRoll.ShowWindow(SW_SHOW);
    m_btnRestart.ShowWindow(SW_SHOW);

    // reset stats
    m_lives = 3; m_score = 0; m_currentQuestionIndex = 0;
    CString tmp; tmp.Format(_T("Score: %d"), m_score); m_staticScore.SetWindowText(tmp);
    tmp.Format(_T("Lives: %d"), m_lives); m_staticLives.SetWindowText(tmp);
}

void CTriviallogicGuiDlg::OnBnClickedBtnAnswerSubmit()
{
    if (m_currentQuestionIndex >= (int)m_currentQuestions.size()) return;

    CString userAnswer; m_editAnswer.GetWindowText(userAnswer); userAnswer.Trim();

    if (userAnswer.CompareNoCase(m_currentAnswers[m_currentQuestionIndex]) == 0)
    {
        ++m_score;
        CString strScore; strScore.Format(_T("Score: %d"), m_score); m_staticScore.SetWindowText(strScore);
        AfxMessageBox(_T("Correct!"));
    }
    else
    {
        --m_lives; if (m_lives < 0) m_lives = 0;
        CString strLives; strLives.Format(_T("Lives: %d"), m_lives); m_staticLives.SetWindowText(strLives);

        if (m_lives <= 0)
        {
            AddHighScore(m_playerName, m_score);
            CString msg; msg.Format(_T("Game Over!\nYour score: %d\nClick Restart to try again."), m_score);
            AfxMessageBox(msg);
            return;
        }
        else
        {
            AfxMessageBox(_T("Wrong answer!"));
        }
    }

    ++m_currentQuestionIndex;
    if (m_currentQuestionIndex < (int)m_currentQuestions.size())
    {
        ShowNextQuestion();
    }
    else
    {
        AddHighScore(m_playerName, m_score);
        CString msg; msg.Format(_T("You finished all questions!\nYour score: %d"), m_score);
        AfxMessageBox(msg);
    }
}

void CTriviallogicGuiDlg::OnBnClickedBtnRestart()
{
    // go back to home screen (safe and simple)
    ShowHomeScreen();
}

// ------------------- ROLL BUTTON -------------------
void CTriviallogicGuiDlg::OnBnClickedBtnRoll()
{
    std::vector<CString> categories = {
        _T("sport"), _T("history"), _T("movie"),
        _T("science"), _T("geography"), _T("custom"), _T("Random")
    };

    CMenu menu; menu.CreatePopupMenu();
    for (int i = 0; i < (int)categories.size(); ++i)
        menu.AppendMenu(MF_STRING, 1000 + i, categories[i]);

    POINT pt; GetCursorPos(&pt);
    int cmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, this);
    if (cmd < 1000 || cmd >= 1000 + (int)categories.size()) return;

    CString selectedCategory = categories[cmd - 1000];

    m_currentQuestions.clear();
    m_currentAnswers.clear();

    if (selectedCategory == _T("Random"))
    {
        for (auto& cat : categories)
        {
            if (cat == _T("Random")) continue;
            auto itq = m_categoryQuestions.find(cat);
            auto ita = m_categoryAnswers.find(cat);
            if (itq != m_categoryQuestions.end() && ita != m_categoryAnswers.end())
            {
                m_currentQuestions.insert(m_currentQuestions.end(), itq->second.begin(), itq->second.end());
                m_currentAnswers.insert(m_currentAnswers.end(), ita->second.begin(), ita->second.end());
            }
        }
    }
    else
    {
        auto itq = m_categoryQuestions.find(selectedCategory);
        auto ita = m_categoryAnswers.find(selectedCategory);
        if (itq != m_categoryQuestions.end() && ita != m_categoryAnswers.end())
        {
            m_currentQuestions = itq->second;
            m_currentAnswers = ita->second;
        }
    }

    if (!m_currentQuestions.empty())
    {
        std::vector<int> indices(m_currentQuestions.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::random_device rd; std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g);

        std::vector<CString> qNew, aNew;
        for (int i : indices)
        {
            qNew.push_back(m_currentQuestions[i]);
            aNew.push_back(m_currentAnswers[i]);
        }

        if ((int)qNew.size() > m_maxQuestions)
        {
            qNew.resize(m_maxQuestions);
            aNew.resize(m_maxQuestions);
        }

        m_currentQuestions = std::move(qNew);
        m_currentAnswers = std::move(aNew);
    }

    m_currentQuestionIndex = 0;
    ShowNextQuestion();

    m_editAnswer.ShowWindow(SW_SHOW);
    m_btnAnswerSubmit.ShowWindow(SW_SHOW);
    m_btnRestart.ShowWindow(SW_SHOW);
}

// ------------------- GAME LOGIC -------------------
void CTriviallogicGuiDlg::ShowNextQuestion()
{
    if (m_currentQuestionIndex < (int)m_currentQuestions.size())
    {
        m_staticQuestion.SetWindowText(m_currentQuestions[m_currentQuestionIndex]);
        m_editAnswer.SetWindowText(_T(""));
        m_staticQuestion.ShowWindow(SW_SHOW);
    }
}

// ------------------- LOAD CSV -------------------
void CTriviallogicGuiDlg::LoadQuestionsFromCSV(const CString& filePath)
{
    CT2A convertedPath(filePath);
    std::ifstream file(convertedPath);
    if (!file.is_open()) return;

    std::string filename(convertedPath);
    CString category;
    if (filename.find("sport") != std::string::npos) category = _T("sport");
    else if (filename.find("geography") != std::string::npos) category = _T("geography");
    else if (filename.find("history") != std::string::npos) category = _T("history");
    else if (filename.find("movie") != std::string::npos) category = _T("movie");
    else if (filename.find("science") != std::string::npos) category = _T("science");
    else category = _T("custom");

    std::string line;
    bool firstLine = true;
    while (std::getline(file, line))
    {
        if (firstLine) { firstLine = false; continue; } // skip header
        std::stringstream ss(line);
        std::string id, question, answer;
        if (std::getline(ss, id, ','))
        {
            if (!std::getline(ss, question, ',')) question.clear();
            if (!std::getline(ss, answer)) answer.clear();

            m_categoryQuestions[category].push_back(CString(question.c_str()));
            m_categoryAnswers[category].push_back(CString(answer.c_str()));
        }
    }
    file.close();
}

// ------------------- HIGH SCORES -------------------
// Add or update player's high score (keeps highest)
void CTriviallogicGuiDlg::AddHighScore(const CString& playerName, int score)
{
    const std::string path = "C:\\Users\\USER\\source\\repos\\Triviallogic\\highscores.csv";
    // read existing into map
    std::map<std::string, int> table;
    {
        std::ifstream fin(path);
        std::string line;
        while (std::getline(fin, line))
        {
            std::stringstream ss(line);
            std::string name; std::string scoreStr;
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, scoreStr)) continue;
            try {
                int s = std::stoi(scoreStr);
                table[name] = s;
            }
            catch (...) { continue; }
        }
    }

    CT2CA cconv(playerName);
    std::string name = (std::string)cconv;
    auto it = table.find(name);
    if (it == table.end() || it->second < score)
    {
        table[name] = score; // insert or replace with higher score
    }

    // write back (sorted desc by score)
    std::vector<std::pair<std::string, int>> entries(table.begin(), table.end());
    std::sort(entries.begin(), entries.end(), [](auto& a, auto& b) { return a.second > b.second; });

    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) return;
    for (auto& e : entries) fout << e.first << "," << e.second << "\n";
    fout.close();
}

// Read and display high scores
void CTriviallogicGuiDlg::ViewHighScores()
{
    const std::string path = "C:\\Users\\USER\\source\\repos\\Triviallogic\\highscores.csv";
    std::ifstream fin(path);
    if (!fin.is_open()) { AfxMessageBox(_T("No high scores yet.")); return; }

    std::vector<ScoreEntry> scores;
    std::string line;
    while (std::getline(fin, line))
    {
        std::stringstream ss(line);
        std::string name; std::string scoreStr;
        if (!std::getline(ss, name, ',')) continue;
        if (!std::getline(ss, scoreStr)) continue;
        try {
            ScoreEntry e; e.name = CString(name.c_str()); e.score = std::stoi(scoreStr);
            scores.push_back(e);
        }
        catch (...) { continue; }
    }
    fin.close();

    if (scores.empty()) { AfxMessageBox(_T("No high scores yet.")); return; }

    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });

    CString out = _T("=== High Scores ===\n");
    for (auto& s : scores)
    {
        CString line; line.Format(_T("%s : %d\n"), s.name, s.score);
        out += line;
    }
    AfxMessageBox(out);
}

void CTriviallogicGuiDlg::ClearHighScores()
{
    const std::string path = "C:\\Users\\USER\\source\\repos\\Triviallogic\\highscores.csv";
    std::ofstream fout(path, std::ios::trunc);
    fout.close();
    AfxMessageBox(_T("High scores cleared!"));
}

void CTriviallogicGuiDlg::OnBnClickedBtnViewHighScores() { ViewHighScores(); }
void CTriviallogicGuiDlg::OnBnClickedBtnClearHighScores() { ClearHighScores(); }

// ------------------- ADMIN: Add / Remove -------------------
void CTriviallogicGuiDlg::OnBnClickedBtnAddSingleQuestion()
{
    if (!m_editAdminFiles.GetSafeHwnd() || !m_editAdminQuestion.GetSafeHwnd() || !m_editAdminMultipleQuestion.GetSafeHwnd())
    {
        AfxMessageBox(_T("Admin controls not available."));
        return;
    }

    CString cFile, cQuestion, cAnswer;
    m_editAdminFiles.GetWindowText(cFile); cFile.Trim();
    m_editAdminQuestion.GetWindowText(cQuestion); cQuestion.Trim();
    m_editAdminMultipleQuestion.GetWindowText(cAnswer); cAnswer.Trim();

    if (cFile.IsEmpty() || cQuestion.IsEmpty() || cAnswer.IsEmpty()) { AfxMessageBox(_T("Fill all fields.")); return; }

    CT2A pathA(cFile);
    std::string path = (std::string)pathA;

    // determine next id
    int id = 1;
    {
        std::ifstream fin(path);
        std::string tmp;
        while (std::getline(fin, tmp)) ++id;
    }

    std::ofstream fout(path, std::ios::app);
    if (!fout.is_open()) { AfxMessageBox(_T("Failed to open file.")); return; }

    CT2CA q(cQuestion), a(cAnswer);
    fout << id << "," << (std::string)q << "," << (std::string)a << "\n";
    fout.close();

    AfxMessageBox(_T("Question added."));
}

void CTriviallogicGuiDlg::OnBnClickedBtnRemoveQuestion()
{
    if (!m_editAdminFiles.GetSafeHwnd() || !m_editAdminDeleteID.GetSafeHwnd()) { AfxMessageBox(_T("Admin controls not available.")); return; }

    CString cFile, cId; m_editAdminFiles.GetWindowText(cFile); cFile.Trim();
    m_editAdminDeleteID.GetWindowText(cId); cId.Trim();
    if (cFile.IsEmpty() || cId.IsEmpty()) { AfxMessageBox(_T("Provide file and ID.")); return; }

    int id = _ttoi(cId); if (id <= 0) { AfxMessageBox(_T("Invalid ID.")); return; }
    CT2A pathA(cFile); std::string path = (std::string)pathA;

    std::ifstream fin(path); if (!fin.is_open()) { AfxMessageBox(_T("Failed to open file.")); return; }
    std::vector<std::string> rows; std::string line;
    while (std::getline(fin, line)) rows.push_back(line);
    fin.close();

    if (id > (int)rows.size()) { AfxMessageBox(_T("ID out of range.")); return; }
    rows.erase(rows.begin() + (id - 1));

    // reindex and write
    std::ofstream fout(path, std::ios::trunc);
    if (!fout.is_open()) { AfxMessageBox(_T("Failed to write file.")); return; }

    for (size_t i = 0; i < rows.size(); ++i)
    {
        std::stringstream ss(rows[i]);
        std::string oldId, question, answer;
        if (std::getline(ss, oldId, ','))
        {
            if (!std::getline(ss, question, ',')) question.clear();
            if (!std::getline(ss, answer)) answer.clear();
            if (!answer.empty())
                fout << (i + 1) << "," << question << "," << answer << "\n";
            else
                fout << (i + 1) << "," << question << "\n";
        }
    }
    fout.close();
    AfxMessageBox(_T("Deleted & reindexed."));
}

void CTriviallogicGuiDlg::OnBnClickedBtnAddMultipleQuestions()
{
    if (!m_editAdminFiles.GetSafeHwnd() || !m_editAdminMultipleQuestion.GetSafeHwnd()) { AfxMessageBox(_T("Admin controls not available.")); return; }

    CString cFile; m_editAdminFiles.GetWindowText(cFile); cFile.Trim();
    CString cBlock; m_editAdminMultipleQuestion.GetWindowText(cBlock); cBlock.Trim();
    if (cFile.IsEmpty() || cBlock.IsEmpty()) { AfxMessageBox(_T("Provide file and block.")); return; }

    CT2A pathA(cFile); std::string path = (std::string)pathA;

    int id = 1;
    {
        std::ifstream fin(path);
        std::string tmp; while (std::getline(fin, tmp)) ++id;
    }

    std::ofstream fout(path, std::ios::app);
    if (!fout.is_open()) { AfxMessageBox(_T("Failed to open file.")); return; }

    int pos = 0;
    CString line = cBlock.Tokenize(_T("\r\n"), pos);
    while (!line.IsEmpty())
    {
        CT2CA conv(line);
        std::string sline = (std::string)conv;
        fout << id++ << "," << sline << "\n";
        line = cBlock.Tokenize(_T("\r\n"), pos);
    }
    fout.close();
    AfxMessageBox(_T("Appended multiple."));
}

// ------------------- ADMIN UI SHOW/HIDE -------------------
void CTriviallogicGuiDlg::ShowAdminControls(BOOL show)
{
    int cmd = show ? SW_SHOW : SW_HIDE;

    if (m_editAdminFiles.GetSafeHwnd()) m_editAdminFiles.ShowWindow(cmd);
    if (m_editAdminQuestion.GetSafeHwnd()) m_editAdminQuestion.ShowWindow(cmd);
    if (m_editAdminMultipleQuestion.GetSafeHwnd()) m_editAdminMultipleQuestion.ShowWindow(cmd);
    if (m_editAdminDeleteID.GetSafeHwnd()) m_editAdminDeleteID.ShowWindow(cmd);

    if (m_btnAddSingleQuestion.GetSafeHwnd()) m_btnAddSingleQuestion.ShowWindow(cmd);
    if (m_btnAddMultipleQuestions.GetSafeHwnd()) m_btnAddMultipleQuestions.ShowWindow(cmd);
    if (m_btnRemoveQuestion.GetSafeHwnd()) m_btnRemoveQuestion.ShowWindow(cmd);

    if (GetDlgItem(IDC_STATIC_FILE)) GetDlgItem(IDC_STATIC_FILE)->ShowWindow(cmd);
    if (GetDlgItem(IDC_STATIC_QUESTION_FILE)) GetDlgItem(IDC_STATIC_QUESTION_FILE)->ShowWindow(cmd);
    if (GetDlgItem(IDC_STATIC_MULTIPLE)) GetDlgItem(IDC_STATIC_MULTIPLE)->ShowWindow(cmd);
    if (GetDlgItem(IDC_STATIC_DELETE)) GetDlgItem(IDC_STATIC_DELETE)->ShowWindow(cmd);
}

void CTriviallogicGuiDlg::ShowHomeScreen()
{
    // hide everything game/admin related
    m_staticQuestion.ShowWindow(SW_HIDE);
    m_editAnswer.ShowWindow(SW_HIDE);
    m_btnAnswerSubmit.ShowWindow(SW_HIDE);
    m_staticLives.ShowWindow(SW_HIDE);
    m_staticScore.ShowWindow(SW_HIDE);
    m_btnRoll.ShowWindow(SW_HIDE);
    m_btnRestart.ShowWindow(SW_HIDE);
    if (m_btnBack.GetSafeHwnd()) m_btnBack.ShowWindow(SW_HIDE);

    ShowAdminControls(FALSE);

    // show home controls
    m_editName.ShowWindow(SW_SHOW);
    m_btnSubmit.ShowWindow(SW_SHOW);
    m_btnPlay.ShowWindow(SW_SHOW);
    m_btnAdmin.ShowWindow(SW_SHOW);
    m_staticEnterName.ShowWindow(SW_SHOW);
    m_btnViewHighScores.ShowWindow(SW_SHOW);
    m_btnClearHighScores.ShowWindow(SW_SHOW);
}

void CTriviallogicGuiDlg::OnBnClickedBtnBack()
{
    ShowHomeScreen();
}
void CTriviallogicGuiDlg::OnStnClickedStaticQuestionFile()
{
    // code here
}

