module;
#include "Core/CoreData.h"
#include "Core/Core.h"
#include <wx/wx.h>
#include "Gfx/WidgetsFwd.h"
#include "MyHealthGuru/MyHealthGuru.h"

#include <wx/aui/auibook.h>

#include <deque>
#include <cstdlib>

// clang-format on

export module MC.View;
import View;
// import Book;
// import DDT;
import Frame;
// import wxTypes;
// import wxUtil;
import SubPage;

namespace mc {
export class View : public CView {

 public:
   View();
   virtual ~View();

 protected:
   auto PageTypeAtIndex (int pageIndex) -> PageType::Type override;
   auto initBook (wxWindow *parent, int auiRow, int auiLayer = 0) -> void override;

   wx::SubPage *m_settings {};

   wxDECLARE_DYNAMIC_CLASS (View);
};

// Implementation moved from View.cpp to reduce module TU count and header churn.
wxIMPLEMENT_DYNAMIC_CLASS(View, CView);

inline View::View() : CView(CFrame::getAuiManager(wx::pFrame<CFrame>())) {
    wx::pView(static_cast<wxView*>(this));
}

inline View::~View() {  }

inline auto View::PageTypeAtIndex(int pageIndex) -> PageType::Type {
   return PageType::None;
}

// Forward-declared helper implemented via smaller module units to avoid heavy imports here.
void populateCategoryPages(wx::Book* book, wx::SubPage*& settings);
void populateSettingsPages(wx::Book* book, wx::SubPage*& settings);
inline void populatePages(wx::Book* book, wx::SubPage*& settings) {
   populateCategoryPages(book, settings);
   populateSettingsPages(book, settings);
}

inline auto View::initBook(wxWindow *parent, int auiRow, int auiLayer) -> void {
   CView::initBook(parent, auiRow, auiLayer);
   m_book->load();
   populatePages(m_book, m_settings);
}
} // namespace mc
