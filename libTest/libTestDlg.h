
// libTestDlg.h : ͷ�ļ�
//

#pragma once


// ClibTestDlg �Ի���
class ClibTestDlg : public CDialogEx
{
// ����
public:
	ClibTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIBTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonTest();
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedButtonExit();
};
