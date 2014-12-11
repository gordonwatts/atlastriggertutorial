//
// A very simple shell program that will read a file from the command line and
// loop over each event.
//

#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/tools/ReturnCheck.h"

#include "xAODRootAccess/TEvent.h"

#include "TChain.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"

#include <iostream>

using namespace std;
using namespace xAOD;

// Config
const char* APP_NAME = "simpleshell";
const char* OUTPUT_FILE = "simpleshell.root";

// Helpers
TChain *getFilesFromCommandLine(int argc, char* argv[]);

int main( int argc, char* argv[] ) {

  // Initialize (as done for all xAOD standalone programs!)
  RETURN_CHECK (APP_NAME, xAOD::Init(APP_NAME));

  auto chain = getFilesFromCommandLine(argc, argv);
  if (chain == nullptr) {
    return 1;
  }

  // Add initialization code for tools here

  // Create the global TEvent object
  TEvent event(TEvent::kBranchAccess);
  RETURN_CHECK(APP_NAME, event.readFrom(chain));

  // Get an output file open, book events.
  auto outputFile = TFile::Open(OUTPUT_FILE, "RECREATE");
  if (outputFile == nullptr || !outputFile->IsOpen()) {
    return 1;
  }

  auto hEventCounter = new TH1F("eventCounter", "Number of Events Processed", 1, 0.0, 1.0);

  // Run the files
  size_t nEntries = chain->GetEntries();
  for (size_t i = 0; i < nEntries; i++) {
    event.getEntry(i);

    // Count
    hEventCounter->Fill(0.5);
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
