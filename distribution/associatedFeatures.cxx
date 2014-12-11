//
// Grab the features associated with a particular trigger.
//

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "xAODTrigEgamma/TrigElectronContainer.h"

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
const char* APP_NAME = "associatedFeatures";
const char* OUTPUT_FILE = "associatedFeatures.root";

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
  trigDecTool.setProperty("TrigDecisionKey","xTrigDecision");
  trigDecTool.initialize();

  //// Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }

  auto hElePt = new TH1F("elept", "Electron p_{T}; p_{T} [GeV]", 50, 0.0, 300.0);
  auto hNContainers = new TH1F("eleNFeature", "Number of electron feature containers in each event; N", 10, 0.0, 10.0);

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t entry = 0; entry < nEntries; entry++) {
    event.getEntry(entry);

    // Get a chain group that represents the HLT trigger.
    auto cg = trigDecTool.getChainGroup("HLT_e25_etcut");
    auto fc = cg->features();
    auto eleFeatureContainers = fc.containerFeature<TrigElectronContainer>();
    hNContainers->Fill(eleFeatureContainers.size());
    cout << "Looking at features for the event: " << endl;
    for(auto econt : eleFeatureContainers) {
      cout << "  -> TrigElectronContainer: " << econt.label() << endl;
      for (auto e : *econt.cptr()) {
	hElePt->Fill(e->pt()/1000.0);
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
