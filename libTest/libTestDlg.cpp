
// libTestDlg.cpp : ʵ���ļ�
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


// ClibTestDlg �Ի���
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


// ClibTestDlg ��Ϣ�������

BOOL ClibTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    _hWndLogger.Open(GetDlgItem(IDC_EDIT_LOGGER)->m_hWnd);
    _devlog.Select(_fileLogger);
    _log.Select(_devlog);
    _log.Select(_hWndLogger);

    _h002.SelectLogger(_devlog);

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
    // ѡ��ǰ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    SetDlgItemText(IDC_EDIT_LOGGER, _T(""));
}

void ClibTestDlg::OnBnClickedButtonExit()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnOK();
}

void ClibTestDlg::OnBnClickedButtonAddcmd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnBnClickedButtonRemovecmd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnBnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnBnClickedButtonRemove()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnBnClickedButtonSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnBnClickedButtonOpen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    // �Զ���ʾ��ǰĿ¼�µĵ�һ��ini�ļ�
    m_CmdList.ResetContent();
    m_CmdConllection.clear();

    _LoadFromIni("");
}

void ClibTestDlg::OnBnClickedButtonScan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void ClibTestDlg::OnLbnSelchangeListCmd()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnBnClickedButtonCall();
}
