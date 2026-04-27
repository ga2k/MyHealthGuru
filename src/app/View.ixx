module;
#include "Core/CoreData.h"
#include "Core/Core.h"
#include <wx/wx.h>
#include "Gfx/WidgetsFwd.h"
#include "MyCare/MyCare.h"

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
//    const char* compiled = wx::compiledBackend();
//    const char* want = std::getenv("MYCARE_UI_BACKEND");
//    if (want && want[0] != '\0') {
//       // Very simple, case-insensitive compare for common values
//       auto eq = [](char a, char b){ return (a==b) || (std::tolower(a)==std::tolower(b)); };
//       auto cmp = [&](const char* a, const char* b){
//          if (!a || !b) return false;
//          while (*a && *b) { if (!eq(*a++, *b++)) return false; }
//          return *a=='\0' && *b=='\0';
//       };
//       if (!cmp(want, compiled)) {
//          wxString msg;
//          msg.Printf("Requested UI backend '%s' via MYCARE_UI_BACKEND, but this build is compiled for '%s'.\n"
//                     "Runtime switching between GTK and Qt backends is not supported.\n"
//                     "Please run the matching build (GTK or Qt6).", wxString::FromUTF8(want), wxString::FromUTF8(compiled));
//          wxLogWarning("%s", msg); // Log warning
// #ifdef __WXGTK__
//          // Avoid showing a blocking dialog too early in app startup in some environments
//          wxMessageBox(msg, "Backend mismatch", wxOK | wxICON_WARNING);
// #else
//          wxMessageBox(msg, "Backend mismatch", wxOK | wxICON_WARNING);
// #endif
//       }
//    }
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
