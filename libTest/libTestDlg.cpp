
// libTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "libTest.h"
#include "libTestDlg.h"
#include "afxdialogex.h"

#include <zhouyb_src.h>

#include <application/driver/ICBC_MT/WinICBC_MT_Driver.h>
#include <application/pboc/pboc_app.cpp>
#include <application/lc/lc_src.h>
#include <application/driver/CommandDriver.cpp>
#include <application/printer/ICBC_XmlPrinter.cpp>
#include <xml/xml_src.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClibTestDlg 对话框
HwndLogger _hWndLogger;
FileLogger _fileLogger;
LoggerAdapter _devlog;
LoggerAdapter _log;

WinICBC_MT_Driver<ArgParser> _h002;

ClibTestDlg::ClibTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LIBTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClibTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CMD, m_CmdList);
    DDX_Control(pDX, IDC_LIST_ARG, m_ArgList);
}

BEGIN_MESSAGE_MAP(ClibTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &ClibTestDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, &ClibTestDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_ADDCMD, &ClibTestDlg::OnBnClickedButtonAddcmd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVECMD, &ClibTestDlg::OnBnClickedButtonRemovecmd)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &ClibTestDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &ClibTestDlg::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_SET, &ClibTestDlg::OnBnClickedButtonSet)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &ClibTestDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &ClibTestDlg::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_CALL, &ClibTestDlg::OnBnClickedButtonCall)
    ON_LBN_SELCHANGE(IDC_LIST_CMD, &ClibTestDlg::OnLbnSelchangeListCmd)
    ON_LBN_SELCHANGE(IDC_LIST_ARG, &ClibTestDlg::OnLbnSelchangeListArg)
    ON_LBN_DBLCLK(IDC_LIST_CMD, &ClibTestDlg::OnLbnDblclkListCmd)
END_MESSAGE_MAP()


// ClibTestDlg 消息处理程序

BOOL ClibTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    _hWndLogger.Open(GetDlgItem(IDC_EDIT_LOGGER)->m_hWnd);
    _devlog.Select(_fileLogger);
    _log.Select(_devlog);
    _log.Select(_hWndLogger);

    _h002.SelectLogger(_devlog);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ClibTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ClibTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL ClibTestDlg::_LoadFromIni(const char* path)
{
    IniFile ini;
    list<string> files;
    WinHelper::EnumFiles(".\\", &files, NULL, "*.ini");
    if(files.size() < 1)
        return false;

    if(!ini.Open(files.front().c_str()))
        return false;

    IniFile::iterator itr;
    IniGroup::iterator itemItr;
    for(itr = ini.begin();itr != ini.end(); ++itr)
    {
        if(StringConvert::Compare(itr->Name.c_str(), "Cmd", true))
        {
            m_CmdConllection.push_back();
            string name = (*itr)["Name"].Value;
            string cmd = (*itr)["Cmd"].Value;
            if(name.length() < 1)
                name = cmd;
            m_CmdConllection.back().Name = name;
            m_CmdConllection.back().Cmd = cmd;
            ArgParser arg;
            arg.Parse((*itr)["Arg"].Value.c_str());

            string key;
            string val;

            while(arg.EnumValue(&val, &key))
            {
                m_CmdConllection.back().Arg.PushValue(key, val);
            }
        }
    }
    _LoadCollection(m_CmdConllection);
    return true;
}

void ClibTestDlg::_LoadCmd(const char* cmdName)
{
    m_CmdList.AddString(cmdName);
    // 选择当前项
    if(m_CmdList.GetCount() < 2)
        m_CmdList.SetCurSel(0);
}

void ClibTestDlg::_LoadCollection(const list<CmdInvoker>& collection)
{
    list<CmdInvoker>::const_iterator itr;
    for(itr = collection.begin();itr != collection.end(); ++itr)
    {
        _LoadCmd(itr->Name.c_str());
    }
}

void ClibTestDlg::OnBnClickedButtonCall()
{
	// TODO: 在此添加控件通知处理程序代码
    int index = m_CmdList.GetCurSel();
    if(index < 0)
        return;
    list<CmdInvoker>::iterator itr = list_helper<CmdInvoker>::index_of(m_CmdConllection, index);
    if(itr == m_CmdConllection.end())
        return;
    ByteBuilder recv(32);
    ByteBuilder arg(32);
    ArgParser::ToString(itr->Arg, arg);

    _log << "---" << itr->Cmd << "---" << endl;
    _log.WriteLine("ARG:");
    _log.WriteLine(arg.GetString());

    if(!_h002.TransmitCommand(itr->Cmd.c_str(), arg, recv))
    {
        _log.WriteLine("False");
        return;
    }
    _log.WriteLine("True:");
    _log.WriteLine(recv.GetString());
}

void ClibTestDlg::OnBnClickedButtonClear()
{
    // TODO: 在此添加控件通知处理程序代码
    SetDlgItemText(IDC_EDIT_LOGGER, _T(""));
}

void ClibTestDlg::OnBnClickedButtonExit()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
}

void ClibTestDlg::OnBnClickedButtonAddcmd()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnBnClickedButtonRemovecmd()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnBnClickedButtonAdd()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnBnClickedButtonRemove()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnBnClickedButtonSet()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
    // 自动显示当前目录下的第一个ini文件
    m_CmdList.ResetContent();
    m_CmdConllection.clear();

    _LoadFromIni("");
}

void ClibTestDlg::OnBnClickedButtonScan()
{
	// TODO: 在此添加控件通知处理程序代码
}

void ClibTestDlg::OnLbnSelchangeListCmd()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_CmdList.GetCurSel();
    if(index < 0)
        return;
    list<CmdInvoker>::iterator itr = list_helper<CmdInvoker>::index_of(m_CmdConllection, index);
    if(itr == m_CmdConllection.end())
        return;
    SetDlgItemText(IDC_EDIT_KEY, "");
    SetDlgItemText(IDC_EDIT_VALUE, "");
    m_ArgList.ResetContent();

    string key;
    string val;
    while(itr->Arg.EnumValue(&val, &key))
    {
        m_ArgList.AddString(key.c_str());
    }
    itr->Arg.EnumValue(NULL, NULL);
    m_ArgList.SetCurSel(0);
    OnLbnSelchangeListArg();
}

void ClibTestDlg::OnLbnSelchangeListArg()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_CmdList.GetCurSel();
    if(index < 0)
        return;
    list<CmdInvoker>::iterator itr = list_helper<CmdInvoker>::index_of(m_CmdConllection, index);
    if(itr == m_CmdConllection.end())
        return;

    index = m_ArgList.GetCurSel();
    if(index < 0)
        return;

    string key;
    string val;
    int i = 0;
    while(itr->Arg.EnumValue(&val, &key))
    {
        if(index == i)
        {
            SetDlgItemText(IDC_EDIT_KEY, key.c_str());
            SetDlgItemText(IDC_EDIT_VALUE, val.c_str());
            break;
        }
        ++i;
    }
    itr->Arg.EnumValue(NULL, NULL);
}

void ClibTestDlg::OnLbnDblclkListCmd()
{
    // TODO: 在此添加控件通知处理程序代码
    OnBnClickedButtonCall();
}
