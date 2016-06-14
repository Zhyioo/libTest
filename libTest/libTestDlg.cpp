
// libTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "libTest.h"
#include "libTestDlg.h"
#include "afxdialogex.h"

#define OPEN_LOGGER
#define NO_INCLUDE_JNI_SOURCE
#define NO_INCLUDE_OPENSSL_SOURCE

#include <zhouyb_lib.h>
#include <zhouyb_src.h>

#include <application/pboc/pboc_app.cpp>

#include <application/driver/CommandDriver.h>
#include <application/driver/CommandDriver.cpp>

#include <application/driver/ICBC_MT_CmdDriver.h>
using namespace zhou_yb::application::driver;

class ICBC_Test : public CommandDriver<SplitArgParser>
{
protected:
    LoggerInvoker _logInvoker;
    LastErrInvoker _objErr;
    LastErrExtractor _lastErr;

    HidDevice _dev;
    HidCmdAdapter<HidDevice, 0, 1> _hidCmdAdapter;

    ICBC_MT_CmdDriver<SplitArgParser> _icbc;
public:
    ICBC_Test()
    {
        _objErr.Invoke(_lasterr, _errinfo);
        _lastErr.IsFormatMSG = false;
        _lastErr.IsLayerMSG = true;
        _lastErr.Select(_dev, "DEV");
        _lastErr.Select(_icbc, "APP");
        _lastErr.Select(_objErr, "MT");

        _logInvoker.select(_dev);
        _logInvoker.select(_icbc);

        list<Ref<ComplexCommand> > cmds = _icbc.GetCommand("");
        Registe(cmds);

        _Registe("EnumCommand", (*this), &ICBC_Test::EnumCommand);
        _Registe("OnCommand", (*this), &ICBC_Test::OnCommand);
        _Registe("LastError", (*this), &ICBC_Test::LastError);

        _Registe("Open", (*this), &ICBC_Test::Open);
        _Registe("Close", (*this), &ICBC_Test::Close);

        _hidCmdAdapter.SelectDevice(_dev);
        _icbc.SelectDevice(_hidCmdAdapter);
    }
    LC_CMD_LASTERR(_lastErr);
    LC_CMD_LOGGER(_logInvoker);
    LC_CMD_METHOD(Open)
    {
        HidDeviceHelper::OpenDevice<HidDevice>(_dev, "BP8903-H002");
        _dev.SetWaitTimeout(1000);
        return true;
    }
    LC_CMD_METHOD(Close)
    {
        _dev.Close();
        return true;
    }
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClibTestDlg 对话框
HwndLogger _hWndLogger;
FileLogger _fileLogger;
LoggerAdapter _devlog;
LoggerAdapter _log;

ICBC_Test _icbc_mt;

ClibTestDlg::ClibTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LIBTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClibTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ClibTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_TEST, &ClibTestDlg::OnBnClickedButtonTest)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &ClibTestDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, &ClibTestDlg::OnBnClickedButtonExit)
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
    _hWndLogger.Open(GetDlgItem(IDC_EDIT_OUTPUT)->m_hWnd);
    _devlog.Select(_fileLogger);
    _log.Select(_devlog);
    _log.Select(_hWndLogger);

    _icbc_mt.SelectLogger(_devlog);

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

void ClibTestDlg::OnBnClickedButtonTest()
{
    // TODO: 在此添加控件通知处理程序代码
    _fileLogger.Open(WinHelper::GetSystemPath());
    // Test
    {
        LOG_FUNC_NAME();
        CString str;
        GetDlgItemText(IDC_EDIT_INPUT, str);

        ByteBuilder recv(64);
        if(!_icbc_mt.TransmitCommand("OnCommand", str.GetString(), recv))
        {
            recv.Clear();
            _icbc_mt.TransmitCommand("OnCommand", "LastError", recv);

            _log << "False" << endl;
        }
        else
        {
            _log << "True" << endl;
        }
        _log.WriteLine(recv.GetString());
    }
    _fileLogger.Close();
}

void ClibTestDlg::OnBnClickedButtonClear()
{
    // TODO: 在此添加控件通知处理程序代码
    SetDlgItemText(IDC_EDIT_OUTPUT, _T(""));
}

void ClibTestDlg::OnBnClickedButtonExit()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
}
