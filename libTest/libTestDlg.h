
// libTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#define OPEN_LOGGER
#define INCLUDE_SOCKET_SOURCE
#define NO_INCLUDE_JNI_SOURCE
#define NO_INCLUDE_OPENSSL_SOURCE

#include <zhouyb_lib.h>
#include <application/driver/CommandDriver.h>
#include <application/driver/CommonCmdDriver.h>
using namespace zhou_yb::application::driver;

#include <application/tools/IConverter.h>

struct CmdInvoker
{
    string Name;
    string Cmd;
    ICommandHandler::CmdArgParser Arg;
};

// ClibTestDlg 对话框
class ClibTestDlg : public CDialogEx
{
// 构造
public:
	ClibTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIBTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    ConverterInvoker _cmdDriver;
    list<CmdInvoker> m_CmdConllection;

    BOOL _LoadFromIni(const char* path);
    void _LoadCmd(const char* cmdName);
    void _LoadCollection(const list<CmdInvoker>& collection);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonAddcmd();
	afx_msg void OnBnClickedButtonRemovecmd();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonSet();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonCall();
protected:
    CListBox m_CmdList;
    CListBox m_ArgList;
public:
    afx_msg void OnLbnSelchangeListCmd();
    afx_msg void OnLbnSelchangeListArg();
    afx_msg void OnLbnDblclkListCmd();
    afx_msg void OnBnClickedButtonSetcmd();
};
