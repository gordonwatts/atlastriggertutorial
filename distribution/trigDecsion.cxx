//
// Do a quick look at the triggers that have fired, including dumping all of them out.
//

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"

#include <iostream>

using namespace std;
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;

// Config
const char* APP_NAME = "trigDecsion";
const char* OUTPUT_FILE = "trigDecsion.root";

// Helpers
TChain *getFilesFromCommandLine(int argc, char* argv[]);
void dumpTriggerInfo(TrigDecisionTool &trigDecisionTool);

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getFilesFromCommandLine(argc, argv);
  if (chain == nullptr) {
    return 1;
  }

  //
  // Add initialization code for tools here
  //

  // The TEvent object
  TEvent event(TEvent::kBranchAccess);
  RETURN_CHECK( APP_NAME, event.readFrom(chain));

  // The configuration tool.
  xAODConfigTool configTool("xAODConfigTool");
  ToolHandle<TrigConf::ITrigConfigTool> configHandle(&configTool);
  configHandle->initialize();
   
  // The decision tool
  TrigDecisionTool trigDecTool("TrigDecTool");
  trigDecTool.setProperty("ConfigTool",configHandle);
  //trigDecTool.setProperty("OutputLevel", MSG::VERBOSE);
  trigDecTool.setProperty("TrigDecisionKey","xTrigDecision");
  trigDecTool.initialize();

  //// Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }

  auto hEventCounter = new TH1F("eventCounter", "Number of Events Processed", 1, 0.0, 1.0);
  auto hLevelPassed = new TH1F("passEvents", "Events passing each level", 4, 0.0, 4.0);
  hLevelPassed->GetXaxis()->SetBinLabel(1, "L1");
  hLevelPassed->GetXaxis()->SetBinLabel(2, "L2");
  hLevelPassed->GetXaxis()->SetBinLabel(3, "EF");
  hLevelPassed->GetXaxis()->SetBinLabel(4, "HLT");

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    // Count
    hEventCounter->Fill(0.5);

    // Did we pass a trigger level?
    if (trigDecTool.isPassed("L1_.*"))
      hLevelPassed->Fill(0.5);
    if (trigDecTool.isPassed("L2_.*"))
      hLevelPassed->Fill(1.5);
    if (trigDecTool.isPassed("EF_.*"))
      hLevelPassed->Fill(2.5);
    if (trigDecTool.isPassed("HLT_.*"))
      hLevelPassed->Fill(3.5);

    if (entry == 1) {
      dumpTriggerInfo(trigDecTool);
    }
  }

  outputFile->Write();
  outputFile->Close();
}

// Dump out all chains in an event
void dumpTriggerInfo(TrigDecisionTool &trigDecTool) {
  cout << "Dumping trigger info for event" << endl;
  auto chainGroups = trigDecTool.getChainGroup(".*");
  for(auto &trig : chainGroups->getListOfTriggers()) {
    cout << "  " << trig << endl;
  }
}

TChain *getFilesFromCommandLine(int argc, char* argv[])
{
  if (argc < 2) {
    ::Error(APP_NAME, "Usage: %s <xAOD file> [xAOD file2]...", APP_NAME);
    return nullptr;
  }
  
  auto chain = new TChain("CollectionTree");
  for (int i = 1; i < argc; i++) {
    chain->Add(argv[i]);
  }

  return chain;
}
