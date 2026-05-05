//
// Created by geoffrey on 8/3/24.
//

#ifndef APP_H
#define APP_H

#include "Core/Core.h"
#include "Gfx/wx.h"

#include <atomic>
#include <chrono>
#include <thread>

import Util;
import App;
import HtmlDialog;

namespace mc {

// class HtmlDialogInfo : public hs::HtmlDialogInfo {
//  public:
//    using hs::HtmlDialogInfo::HtmlDialogInfo;
//    // auto DialogBoxText (hs::DialogText which) -> bool override;
// };

class App : public CApp {
   std::string currentTheme;

 public:
   App();
   ~App() override;

   auto OnInit() -> bool override;
   auto DialogBoxText (const std::string & which) -> std::shared_ptr<hs::HtmlDialog::HtmlDialogInfo> override;

   void SignalWindowReady();

//   // Call this when your window is fully initialized and visible
//   void OnWindowFullyReady();

   wxDECLARE_DYNAMIC_CLASS (App);
};
} // namespace mc
#endif // APP_H
