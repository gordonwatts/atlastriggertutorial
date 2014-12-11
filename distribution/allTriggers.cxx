//
// Look at all the triggers that have fired in a app.
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
const char* APP_NAME = "allTriggers";
const char* OUTPUT_FILE = "allTriggers.root";

// Helpers
TChain *getFilesFromCommandLine(int argc, char* argv[]);

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

  // Run the files
  map<string,int> triggerCounts;
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    // Count
    hEventCounter->Fill(0.5);

    // Get a list of all the chains in the event. Save them in a map for later use.
    // We add a zero to make sure we have a record of all the triggers
    auto chainGroup = trigDecTool.getChainGroup(".*");
    for(auto &trig : chainGroup->getListOfTriggers()) {
      auto cg = trigDecTool.getChainGroup(trig);
      if (cg->isPassed()) {
	triggerCounts[trig]++;
      } else {
	triggerCounts[trig] += 0;
      }
    }
  }

  // Dump it out to std out, and record in a histogram.
  cout << "Final trigger tally" << endl;
  auto htrig = new TH1F("firedTriggers", "Tally of all triggers in the file", triggerCounts.size(), 0.0, triggerCounts.size());
  int index = 1;
  for (auto &i : triggerCounts) {
    cout << "  " << i.first << ": " << i.second << " times" << endl;
    htrig->GetXaxis()->SetBinLabel(index, i.first.c_str());
    htrig->SetBinContent(index, i.second);
    index++;
  }

  outputFile->Write();
  outputFile->Close();
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
