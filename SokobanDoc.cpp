
// SokobanDoc.cpp : implementation of the CSokobanDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Sokoban.h"
#endif

#include "SokobanDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSokobanDoc

IMPLEMENT_DYNCREATE(CSokobanDoc, CDocument)

BEGIN_MESSAGE_MAP(CSokobanDoc, CDocument)
    ON_COMMAND(ID_RESTART, &CSokobanDoc::OnRestart)
END_MESSAGE_MAP()


// CSokobanDoc construction/destruction

CSokobanDoc::CSokobanDoc() noexcept
{
	// TODO: add one-time construction code here

}

CSokobanDoc::~CSokobanDoc()
{
}

BOOL CSokobanDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
    SetModifiedFlag();
    n++;
    if (n == 151) {
        AfxMessageBox(L"pass all level", MB_OK | MB_ICONMASK);
        PostQuitMessage(0);
    }
	boxes = 0, desk = 0, max_len = 0;//map information
	arr = 0, steps = 0;//player status
    if (readMap(n)) OnNewDocument();
    UpdateAllViews(NULL);

	return TRUE;
}




// CSokobanDoc serialization

void CSokobanDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
        ar << n;
        ar << floor.size();
        for (size_t i = 0; i < floor.size(); i++) {
            ar << floor[i].size();
            for (size_t j = 0; j < floor[i].size(); j++) {
                ar << floor[i][j].form << floor[i][j].state;
            }
        }
        ar << arr << boxes << desk << local << max_len << scale << steps;

	}
	else
	{
		// TODO: add loading code here
        floor.clear();
        ar >> n;
        size_t row;
        ar >> row;
        for (size_t i = 0; i < row; i++) {
            size_t col;
            ar >> col;
            vector <CFloor> temp;
            for (size_t j = 0; j < col; j++) {
                char a; short b;
                ar >> a >> b;
                temp.push_back(CFloor(a, b));
             
            }
            floor.push_back(temp);
        }
        ar >> arr >> boxes >> desk >> local >> max_len >> scale >> steps;

	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSokobanDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSokobanDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSokobanDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSokobanDoc diagnostics

#ifdef _DEBUG
void CSokobanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSokobanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSokobanDoc commands


int CSokobanDoc::readMap(int n)
{
	// TODO: Add your implementation code here.
    FILE* fp;
    char txt[20];
    sprintf_s(txt, ".\\map\\map%03d.txt", n);
    fopen_s(&fp, txt, "r");

    //if (fp == 0)printf("aaa");
    int c;
    int i = 0, j = 0, w = 0;
    bool error = 0;
    floor.clear();
    vector <CFloor> temp;
    
    while (1) {
        c = fgetc(fp);
        switch (c) {
        case 'H':
            temp.push_back(CFloor(wall_character, 1));
            j++;
            break;
        case 'B':
            temp.push_back(CFloor(box_character, 2));
            j++; boxes++;
            break;
        case 'C':
            temp.push_back(CFloor(box_character, 3));
            j++; desk++; boxes++; arr++;
            break;
        case 'D':
            temp.push_back(CFloor(dest_character, 3));
            j++; desk++;
            break;
        case 'W':
            temp.push_back(CFloor(worker_character, 4));
            local.x = i; local.y = j;
            j++; w++;
            break;
        case ' ': case '0':
            temp.push_back(CFloor(' ', 4));
            j++;
            break;
        case '\n': case EOF:
            if (j == 0)break;
            floor.push_back(temp);
            temp.clear();
            if (j > max_len)max_len = j;
            j = 0; i++;

            break;
        default:
            error = 1;
            break;
        }
        if (c == EOF)break;

    }

    for (size_t i = 0; i < floor.size(); i++) {
        for (size_t j = floor[i].size(); j < max_len; j++)
        floor[i].push_back(CFloor(' ', 4));
    }

    if (error) {
        CString txt;
        txt.Format(L"Level %d : invalid characters in the map matrix\nGo to next level.", n);
        AfxMessageBox(txt, MB_OK);
        return 1;
    }
    if (boxes != desk) {
        CString txt;
        txt.Format(L"Level %d : The number of 'B' must be the same as 'D'.\nGo to next level.", n);
        AfxMessageBox(txt, MB_OK);
        return 1;
    }
    if (w != 1) {
        CString txt;
        txt.Format(L"Level %d : There should be exactly one 'W'.\nGo to next level.", n);
        AfxMessageBox(txt, MB_OK);
        return 1;
    }
    fclose(fp);

	
	return 0;
}


void CSokobanDoc::OnRestart()
{
    // TODO: Add your command handler code here
    n--;
    OnNewDocument();

}
