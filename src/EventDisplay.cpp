/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file EventDisplay.cpp
 */

#include "EventDisplay.h"
#include <JPetLoggerInclude.h>

namespace jpet_event_display
{

const char *filetypes[] = {
    "All files", "*",
    "ROOT files", "*.root",
    "Text files", "*.[tT][xX][tT]",
    0, 0};

EventDisplay::EventDisplay() 
{
  fGUIControls->eventNo = 0;
  fGUIControls->stepNo = 0;
  run();
  updateGUIControlls();
  fApplication->Run();
  DATE_AND_TIME();
  INFO("J-PET Event Display created");
  INFO("*********************");
}

EventDisplay::~EventDisplay() {
  fMainWindow->Cleanup();
}

void EventDisplay::run()
{
  fMainWindow = std::unique_ptr<TGMainFrame>(new TGMainFrame(gClient->GetRoot()));
  fMainWindow->SetCleanup(kDeepCleanup);
  fMainWindow->Connect("CloseWindow()", "jpet_event_display::EventDisplay", this, "CloseWindow()");
  const Int_t w_max = 1024;
  const Int_t h_max = 720;
  fMainWindow->SetWMSize(w_max, h_max); //this is the only way to set the size of the main frame
  fMainWindow->SetLayoutBroken(kTRUE);
  assert(fMainWindow != 0);
  const Int_t w_GlobalFrame = w_max;
  const Int_t h_GlobalFrame = h_max;
  const Int_t w_Frame1 = 180;
  const Int_t h_Frame1 = h_max - 12; // cause 2*5 margins from GlobalFrame and 2*1 margins from Frame1
  const Int_t w_Frame2 = w_max - w_Frame1;
  const Int_t h_Frame2 = h_max - 12;
  
  TGCompositeFrame *baseFrame =
      AddCompositeFrame(fMainWindow.get(), w_GlobalFrame, h_GlobalFrame - 10, kVerticalFrame);
  fMainWindow->AddFrame(baseFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

  //saving background color in fFrameBackgroundColor
  gClient->GetColorByName("#e6e6e6", fFrameBackgroundColor); 
  baseFrame->ChangeBackground(fFrameBackgroundColor);

  AddMenuBar(baseFrame);

  TGCompositeFrame *globalFrame =
      AddCompositeFrame(baseFrame, w_GlobalFrame, h_GlobalFrame - 10);

  TGGroupFrame *optionsFrame =
      AddGroupFrame(globalFrame, "Options", w_Frame1, h_Frame1);
  TGGroupFrame *displayFrame =
      AddGroupFrame(globalFrame, "Display", w_Frame2, h_Frame2);

  CreateOptionsFrame(optionsFrame); 
  CreateDisplayFrame(displayFrame);

  globalFrame->Resize(globalFrame->GetDefaultSize());
  baseFrame->Resize(baseFrame->GetDefaultSize());

  fMainWindow->SetWindowName("Single Strip Event Display ver 0.1");
  fMainWindow->MapSubwindows();
  fMainWindow->Resize(fMainWindow->GetDefaultSize());
  fMainWindow->MapWindow();
}

void EventDisplay::CreateDisplayFrame(TGGroupFrame* parentFrame) {
  fDisplayTabView = std::unique_ptr<TGTab>(new TGTab(parentFrame, 1, 1));
  fDisplayTabView->ChangeBackground(fFrameBackgroundColor);

  AddTab(fDisplayTabView, visualizator->getCanvas3d(), "3d view", "3dViewCanvas");
  AddTab(fDisplayTabView, visualizator->getCanvas2d(), "2d view", "2dViewCanvas");
  AddTab(fDisplayTabView, visualizator->getCanvasDiagrams(), "Diagram view", "diagramCanvas");

  fDisplayTabView->SetEnabled(1, kTRUE);
  parentFrame->AddFrame(
      fDisplayTabView.get(), new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,
                                   2, 2, 5, 1));
}

void EventDisplay::CreateOptionsFrame(TGGroupFrame* parentFrame)
{
  TGCompositeFrame *frame1_1 = 
    AddCompositeFrame(parentFrame, 1, 1, kVerticalFrame, kLHintsExpandX |kLHintsTop, 1, 1, 1, 1);

  TGCompositeFrame *frame1_1_2 = 
    AddCompositeFrame(frame1_1, 1, 1, kHorizontalFrame, kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2);

  AddButton(frame1_1_2, "Read Geometry", "handleMenu(=0)");
  AddButton(frame1_1_2, "Read Data", "handleMenu(=1)");

  TGCompositeFrame *frame1_2 = 
    AddCompositeFrame(parentFrame, 1, 1, kVerticalFrame, kLHintsExpandX | kLHintsExpandY, 1, 1, 1, 1);

  TGTab* pTab = new TGTab(frame1_2, 1, 1);
  pTab->ChangeBackground(fFrameBackgroundColor);

  TGCompositeFrame* tf1 = pTab->AddTab("Info");
  tf1->ChangeBackground(fFrameBackgroundColor);

  TGCompositeFrame* tabFrame1 = 
    AddCompositeFrame(tf1, 1, 1, kVerticalFrame, kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5);

  fInputInfo = std::unique_ptr<TGLabel>(new TGLabel(tabFrame1,
                                        "No file read.",
                                        TGLabel::GetDefaultGC()(),
                                        TGLabel::GetDefaultFontStruct(),
                                        kChildFrame,
                                        fFrameBackgroundColor));
  fInputInfo->SetTextJustify(kTextTop | kTextLeft);
  tabFrame1->AddFrame(fInputInfo.get(), new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));

  pTab->SetEnabled(1,kTRUE);
  frame1_2->AddFrame(pTab, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2, 2, 5, 1));

  TGCompositeFrame *frame1_3 = 
    AddCompositeFrame(parentFrame, 1, 1, kVerticalFrame, kLHintsExpandX| kLHintsBottom, 2, 2, 2, 2);

  TGCompositeFrame *frame1_3_1 = 
    AddCompositeFrame(frame1_3, 1, 1, kHorizontalFrame, kLHintsExpandX | kLHintsTop, 2, 2, 2, 2);

  AddButton(frame1_3_1, "&Next >", "doNext()");
  AddButton(frame1_3_1, "&Reset >", "doReset()");
  AddButton(frame1_3_1, "Show Data", "showData()");

  TGCompositeFrame *frame1_3_2 =
    AddCompositeFrame(frame1_3, 1, 1, kHorizontalFrame, kLHintsExpandX| kLHintsTop, 5, 5, 5, 5);

  TGLabel *labelStep = new TGLabel(frame1_3_2,"Step",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,fFrameBackgroundColor);
  labelStep->SetTextJustify(36);
  frame1_3_2->AddFrame(labelStep, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  int fMaxEvents = 1000; // temporary

  fNumberEntryStep = std::unique_ptr<TGNumberEntry>(new TGNumberEntry(frame1_3_2,
                                                  1, 5, -1, TGNumberFormat::kNESInteger,
                                                  TGNumberFormat::kNEAPositive,
                                                  TGNumberFormat::kNELLimitMinMax, 1,fMaxEvents));
  frame1_3_2->AddFrame(fNumberEntryStep.get(), new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  fNumberEntryStep->Connect("ValueSet(Long_t)", "jpet_event_display::EventDisplay", this, "updateGUIControlls()");

  TGLabel *labelEventNo = new TGLabel(frame1_3_2,"Event no",TGLabel::GetDefaultGC()(),TGLabel::GetDefaultFontStruct(),kChildFrame,fFrameBackgroundColor);
  labelEventNo->SetTextJustify(36);
  frame1_3_2->AddFrame(labelEventNo, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fNumberEntryEventNo = std::unique_ptr<TGNumberEntry>(new TGNumberEntry(frame1_3_2,
                                                      1, 5, -1, TGNumberFormat::kNESInteger,
                                                      TGNumberFormat::kNEANonNegative,
                                                      TGNumberFormat::kNELLimitMinMax, 0, fMaxEvents));
  frame1_3_2->AddFrame(fNumberEntryEventNo.get(), new TGLayoutHints(kLHintsExpandX));
  fNumberEntryEventNo->Connect("ValueSet(Long_t)", "jpet_event_display::EventDisplay", this, "updateGUIControlls()");

  fProgBar = std::unique_ptr<TGHProgressBar>(new TGHProgressBar(frame1_3,TGProgressBar::kFancy,250));
  fProgBar->SetBarColor("lightblue");
  fProgBar->ShowPosition(kTRUE,kFALSE,"%.0f events");
  fProgBar->SetRange(0,fMaxEvents);
  frame1_3->AddFrame(fProgBar.get(),new TGLayoutHints(kLHintsCenterX,5,5,3,4));
}

void EventDisplay::AddTab(std::unique_ptr<TGTab> &pTabViews,
                          std::unique_ptr<TRootEmbeddedCanvas> &saveCanvasPtr,
                          const char *tabName,
                          const char *canvasName) {
  TGCompositeFrame *tabView = pTabViews->AddTab(tabName);
  tabView->ChangeBackground(fFrameBackgroundColor);

  saveCanvasPtr = std::unique_ptr<TRootEmbeddedCanvas>(
      new TRootEmbeddedCanvas(canvasName, tabView, 600, 600));
  tabView->AddFrame(
      saveCanvasPtr.get(),
      new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));
}

void EventDisplay::AddButton(TGCompositeFrame *parentFrame,
                             const char *buttonText,
                             const char *signalFunction) {
  TGTextButton *button = new TGTextButton(parentFrame, buttonText);
  parentFrame->AddFrame(
      button, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 3, 4));
  button->Connect("Clicked()", "jpet_event_display::EventDisplay", this,
                  signalFunction);
  button->SetTextJustify(36);
  button->ChangeBackground(fFrameBackgroundColor);
}

TGGroupFrame* EventDisplay::AddGroupFrame(TGCompositeFrame *parentFrame,
                                         const char *frameName, Int_t width,
                                         Int_t height) {
  TGGroupFrame *frame = new TGGroupFrame(
      parentFrame, frameName, kVerticalFrame, TGGroupFrame::GetDefaultGC()(),
      TGGroupFrame::GetDefaultFontStruct(), fFrameBackgroundColor);
  frame->Resize(width, height);
  parentFrame->AddFrame(
      frame, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 1, 1, 1, 1));
  return frame;
}

TGCompositeFrame *EventDisplay::AddCompositeFrame(TGCompositeFrame *parentFrame,
                                                  Int_t width, Int_t height,
                                                  Int_t options, ULong_t hints,
                                                  Int_t padleft,Int_t padright,
                                                  Int_t padtop, Int_t padbottom) {
  TGCompositeFrame *frame =
      new TGCompositeFrame(parentFrame, width, height, options);
  frame->ChangeBackground(fFrameBackgroundColor);
  parentFrame->AddFrame(
      frame, new TGLayoutHints(hints, padleft, padright, padtop, padbottom));
  return frame;
}

void EventDisplay::AddMenuBar(TGCompositeFrame *parentFrame)
{
  TGMenuBar *fMenuBar = new TGMenuBar(parentFrame, 1, 1, kHorizontalFrame);
  fMenuBar->ChangeBackground(fFrameBackgroundColor);

  TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 0, 0, 0, 0);
  TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 0, 0, 0);

  TGPopupMenu *fMenuFile = new TGPopupMenu(gClient->GetRoot());
  fMenuFile->AddEntry(" &Open Geometry...\tCtrl+O", E_OpenGeometry);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry(" &Open Data...\tCtrl+O", E_OpenData);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry(" E&xit\tCtrl+Q", E_Close);
  fMenuFile->Associate(fMainWindow.get());
  fMenuFile->Connect("Activated(Int_t)", "jpet_event_display::EventDisplay", this, "handleMenu(Int_t)");

  fMenuFile->DisableEntry(1);

  fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);

  parentFrame->AddFrame(fMenuBar, fMenuBarLayout);
}

void EventDisplay::CloseWindow() { gApplication->Terminate(); }

void EventDisplay::handleMenu(Int_t id)
{
  switch (id)
  {
    case E_OpenGeometry:
    {
      TString dir("");
      fFileInfo->fFileTypes = filetypes;
      fFileInfo->fIniDir = StrDup(dir);
      new TGFileDialog(gClient->GetRoot(), fMainWindow.get(), kFDOpen, fFileInfo.get());
      if(fFileInfo->fFilename == 0)
        return;
      assert(visualizator);
      visualizator->loadGeometry(fFileInfo->fFilename);
      visualizator->drawOnlyGeometry();
    }
    break;
    case E_OpenData:
    {
      TString dir("");
      fFileInfo->fFileTypes = filetypes;
      fFileInfo->fIniDir = StrDup(dir);
      new TGFileDialog(gClient->GetRoot(), fMainWindow.get(), kFDOpen, fFileInfo.get());
      if(fFileInfo->fFilename == 0)
        return;
      assert(dataProcessor);
      dataProcessor->openFile(fFileInfo->fFilename);
      /*switch(dataProcessor->getCurrentFileType())
      {
        case DataProcessor::FileTypes::fTimeWindow:
          AddTab(fDisplayTabView, visualizator->getCanvas2d(), "2d view", "2dViewCanvas");
          break;
        case DataProcessor::FileTypes::fRawSignal:
          AddTab(fDisplayTabView, visualizator->getCanvas2d(), "2d view", "2dViewCanvas");
          AddTab(fDisplayTabView, visualizator->getCanvasDiagrams(), "Diagram view", "diagramCanvas");
          break;
        default:
          break;
      }*/
      drawSelectedStrips();
    }
    break;
    case E_Close:
    {
      CloseWindow();
    }
    break;
  }
  return;
}

void EventDisplay::updateGUIControlls() 
{
  fGUIControls->eventNo = fNumberEntryEventNo->GetIntNumber();
  fGUIControls->stepNo = fNumberEntryStep->GetIntNumber();
}

void EventDisplay::doReset() {
  fNumberEntryEventNo->SetIntNumber(1);
  fNumberEntryStep->SetIntNumber(1);
  updateProgressBar(0);
  showData();
}

void EventDisplay::doNext()
{
  updateGUIControlls();
  fNumberEntryEventNo->SetIntNumber(fGUIControls->eventNo + fGUIControls->stepNo);
  showData();
}

void EventDisplay::showData()
{
  updateGUIControlls();
  dataProcessor->nthEvent(fGUIControls->eventNo);
  drawSelectedStrips();
  updateProgressBar();
  fInputInfo->ChangeText(dataProcessor->getDataInfo().c_str());
}

void EventDisplay::drawSelectedStrips()
{
  std::map<int, std::vector<int> > selection = dataProcessor->getActiveScintillators();
  visualizator->drawStrips(selection);
  visualizator->drawDiagram(dataProcessor->getDataForDiagram());
}

void EventDisplay::setMaxProgressBar (Int_t maxEvent) {
  fProgBar->Format(Form("%%.0f/%d events",maxEvent));
  fProgBar->SetRange(0.0f,Float_t(maxEvent));
}

}
