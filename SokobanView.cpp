
// SokobanView.cpp : implementation of the CSokobanView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Sokoban.h"
#endif

#include "SokobanDoc.h"
#include "SokobanView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSokobanView

IMPLEMENT_DYNCREATE(CSokobanView, CView)

BEGIN_MESSAGE_MAP(CSokobanView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CSokobanView construction/destruction

CSokobanView::CSokobanView() noexcept
{
	// TODO: add construction code here

}

CSokobanView::~CSokobanView()
{
}

BOOL CSokobanView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSokobanView drawing

void CSokobanView::OnDraw(CDC* pDC)
{
	CSokobanDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	GetWindowRect(rect);


	if ((pDoc->scale * pDoc->max_len * 32 > rect.bottom - 20) || (pDoc->scale * pDoc->floor.size() * 32 > rect.right - 250))pDoc->scale--;
	if ((pDoc->scale * pDoc->max_len * 32 > rect.bottom - 20) || (pDoc->scale * pDoc->floor.size() * 32 > rect.right - 250))pDoc->scale--;
	if (pDoc->scale == 0)pDoc->scale++;

	CDC   MemDC;
	CBitmap   MemBitmap;  
	MemDC.CreateCompatibleDC(NULL);  
	MemBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());  
	CBitmap* pOldBit = MemDC.SelectObject(&MemBitmap);   
	MemDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));

	for (size_t i = 0; i < pDoc->floor.size(); i++) {
		for (size_t j = 0; j < pDoc->floor[i].size(); j++) {
			CBitmap aBitmap;
			switch (pDoc->floor[i][j].form) {
			case wall_character:
				aBitmap.LoadBitmap(IDB_WALL_H1);
				break;
			case box_character:
				if (pDoc->floor[i][j].state == 2) aBitmap.LoadBitmap(IDB_BOX_B2);
				else if (pDoc->floor[i][j].state == 3) aBitmap.LoadBitmap(IDB_ARRIVAL_B3);
				break;
			case dest_character: // &Space
				if (pDoc->floor[i][j].state == 3) aBitmap.LoadBitmap(IDB_DEST_D3);
				else if (pDoc->floor[i][j].state == 4) aBitmap.LoadBitmap(IDB_BLANK_S4);
				break;
			case worker_character:
				aBitmap.LoadBitmap(IDB_WORKER_W4);
				break;
			}
			CDC mapDC;
			mapDC.CreateCompatibleDC(&MemDC);
			mapDC.SelectObject(&aBitmap);
			MemDC.StretchBlt(j * 32 * pDoc->scale, i * 32 * pDoc->scale, 32 * pDoc->scale, 32 * pDoc->scale, &mapDC, 0, 0, 32, 32, SRCCOPY);
		}

	}

	CFont aFont;
	aFont.CreateFont(15 * pDoc->scale, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	MemDC.SelectObject(&aFont);

	CString txt;
	txt.Format(L"Level: %d", pDoc->n);
	MemDC.TextOut(pDoc->max_len * pDoc->scale * 32 + 5, 15 * (pDoc->scale - 1), txt);
	txt.Format(L"Destination: %d", pDoc->desk);
	MemDC.TextOut(pDoc->max_len * pDoc->scale * 32 + 5, 30 + 30 * (pDoc->scale - 1), txt);
	txt.Format(L"Arrival: %d", pDoc->arr);
	MemDC.TextOut(pDoc->max_len * pDoc->scale * 32 + 5, 60 + 45 * (pDoc->scale - 1), txt);
	txt.Format(L"Steps: %d", pDoc->steps);
	MemDC.TextOut(pDoc->max_len * pDoc->scale * 32 + 5,90 + 60 * (pDoc->scale - 1), txt);
	MemDC.TextOut(pDoc->max_len * pDoc->scale * 32 + 5, 120 + 75 * (pDoc->scale - 1), status);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	//MemBitmap.DeleteObject();
	//MemDC.DeleteDC();

}


// CSokobanView printing

BOOL CSokobanView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSokobanView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSokobanView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CSokobanView diagnostics

#ifdef _DEBUG
void CSokobanView::AssertValid() const
{
	CView::AssertValid();
}

void CSokobanView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSokobanDoc* CSokobanView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSokobanDoc)));
	return (CSokobanDoc*)m_pDocument;
}
#endif //_DEBUG


// CSokobanView message handlers


void CSokobanView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CSokobanDoc* pDoc = GetDocument();
	GetWindowRect(rect);

	switch (nChar) {
	case VK_ADD:
		if (pDoc->scale < 3)pDoc->scale++;
		break;
	case VK_SUBTRACT:
		if (pDoc->scale > 1) pDoc->scale--;
		break;
	case 'H': case 'A': case VK_LEFT:
		move(LEFT);
		break;
	case 'J': case 'W': case VK_UP:
		move(UP);
		break;
	case 'K': case 'S': case VK_DOWN:
		move(DOWN);
		break;
	case 'L': case 'D': case VK_RIGHT:
		move(RIGHT);
		break;
	case 'Z':
		pDoc->OnNewDocument();
		break;
	}

	if (pDoc->arr == pDoc->desk) {
		pDoc->UpdateAllViews(NULL);
		AfxMessageBox(L"PASS!\nGo to next level.", MB_OK|MB_ICONMASK);
		pDoc->OnNewDocument();
	}

	pDoc->UpdateAllViews(NULL);


	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


char CSokobanView::nextToWorker(Direction d)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();

	LONG i = pDoc->local.x, j = pDoc->local.y;
	if (d == LEFT) return pDoc->floor[i][--j].form;
	if (d == UP)   return pDoc->floor[--i][j].form;
	if (d == DOWN) return pDoc->floor[++i][j].form;
	if (d == RIGHT)return pDoc->floor[i][++j].form;

	return 0;
}


char CSokobanView::nextNextToWorker(Direction d)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();

	LONG i = pDoc->local.x, j = pDoc->local.y;
	if (d == LEFT) return pDoc->floor[i][j - 2].form;
	if (d == UP)   return pDoc->floor[i - 2][j].form;
	if (d == DOWN) return pDoc->floor[i + 2][j].form;
	if (d == RIGHT)return pDoc->floor[i][j + 2].form;

	return 0;
}


void CSokobanView::move(Direction D)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();

	status.Format(L"");
	char d = nextToWorker(D);
	if (d == ' ')moveWorker(D);
	else if ((d == 'B') && (nextNextToWorker(D) == ' '))moveBox(D);
	else status.Format(L"Warning!");
}


void CSokobanView::moveWorker(Direction d)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();
	LONG& i = pDoc->local.x, & j = pDoc->local.y;
	pDoc->steps++;
	if (d == LEFT)      sortWorker(i, j, 0, -1);
	else if (d == UP)   sortWorker(i, j, -1, 0);
	else if (d == DOWN) sortWorker(i, j, 1, 0);
	else if (d == RIGHT)sortWorker(i, j, 0, 1);

}


void CSokobanView::moveBox(Direction d)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();
	LONG& i = pDoc->local.x, & j = pDoc->local.y;
	if (d == LEFT) {
		sortBox(i, j - 1, 0, -1);
		moveWorker(LEFT);
	}
	else if (d == UP) {
		sortBox(i - 1, j, -1, 0);
		moveWorker(UP);
	}
	else if (d == DOWN) {
		sortBox(i + 1, j, 1, 0);
		moveWorker(DOWN);
	}
	else if (d == RIGHT) {
		sortBox(i, j + 1, 0, 1);
		moveWorker(RIGHT);
	}

}


void CSokobanView::sortWorker(LONG & i, LONG & j, int movei, int movej)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();
	pDoc->floor[i][j].form = dest_character;
	i += movei; j += movej;
	pDoc->floor[i][j].form = worker_character;
	if (pDoc->floor[i][j].state == 2)pDoc->floor[i][j].state = 4;

}


void CSokobanView::sortBox(LONG i, LONG j, int movei, int movej)
{
	// TODO: Add your implementation code here.
	CSokobanDoc* pDoc = GetDocument();
	if ((pDoc->floor[i][j].state == 2) && (pDoc->floor[i + movei][j + movej].state == 3))pDoc->arr++;
	else if ((pDoc->floor[i][j].state == 3) && (pDoc->floor[i + movei][j + movej].state != 3))pDoc->arr--;
	pDoc->floor[i][j].form = dest_character;
	i += movei; j += movej;
	pDoc->floor[i][j].form = box_character;
	if (pDoc->floor[i][j].state == 4) pDoc->floor[i][j].state = 2;
}
