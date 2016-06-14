
// libTestDlg.cpp : ʵ���ļ�
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


// ClibTestDlg �Ի���
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


// ClibTestDlg ��Ϣ�������

BOOL ClibTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    _hWndLogger.Open(GetDlgItem(IDC_EDIT_OUTPUT)->m_hWnd);
    _devlog.Select(_fileLogger);
    _log.Select(_devlog);
    _log.Select(_hWndLogger);

    _icbc_mt.SelectLogger(_devlog);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void ClibTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR ClibTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void ClibTestDlg::OnBnClickedButtonTest()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    SetDlgItemText(IDC_EDIT_OUTPUT, _T(""));
}

void ClibTestDlg::OnBnClickedButtonExit()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnOK();
}
