
#pragma once

#define StyleTile 0
#define StyleCenter 1
#define StyleStretch 2

class CBDialog : public CDialog
{
// Construction
public:
	int SetBitmap(UINT nIDResource);
	void SetBitmapStyle(int style);

	CBDialog(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_bitmap;
	int m_style;
};
