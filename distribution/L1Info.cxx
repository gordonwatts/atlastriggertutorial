//
// The L1 features aren't really features. They aren't really
// associated with the TDT - in fact, the L1 info doesn't contain
// any navagation information.
//
// This demonstrates grabbing some L1 JetRoI features and making
// a simple plot. It is mostly just finding the container, and loading
// it directly from the event.
//

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoI.h"

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
const char* APP_NAME = "L1Info";
const char* OUTPUT_FILE = "L1Info.root";

// Helpers
TChain *getFilesFromCommandLine(int argc, char* argv[]);

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getFilesFromCommandLine(argc, argv);
  if (chain == nullptr) {
    return 1;
  }

  // Initialize Everything

  // The TEvent object. We need this to access anything that is in
  // the event.
  // We do not need the trigger tool for this!

  TEvent event(TEvent::kBranchAccess);
  RETURN_CHECK( APP_NAME, event.readFrom(chain));

  //// Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }

  // A few histograms we want to fill.
  auto hNRoI = new TH1F("nJetRoIs",
			"Number of jet RoIs from the Jet Trigger; N",
			20, 0.0, 20.0);
  auto het = new TH1F("et8x8",
		      "E_T in a 8x8 cell range around the RoI; [GeV]",
		      100, 0.0, 100.0);
  auto heta = new TH1F("eta",
		       "Eta of the RoI; eta",
		       20, -4.0, 4.0);

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    // Get the l1 jet info
    const xAOD::JetRoIContainer* jetRoIs = 0;
    event.retrieve(jetRoIs, "LVL1JetRoIs" ).ignore();
    if (jetRoIs != 0) {
      hNRoI->Fill(jetRoIs->size());
      for (auto roi : *jetRoIs) {
	het->Fill(roi->et8x8()/1000.);
	heta->Fill(roi->eta());
      }
    }
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
