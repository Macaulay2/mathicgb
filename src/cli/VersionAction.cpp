// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.

#include <iostream>

#include "mathicgb.h"
#include "mathicgb/stdinc.h"
#include "VersionAction.hpp"


MATHICGB_NAMESPACE_BEGIN

void VersionAction::performAction() {
  std::cout << "Mathicgb " << MATHICGB_VERSION_STRING << std::endl;

  exit(0);
}

const char* VersionAction::staticName() {
  return "version";
}

const char* VersionAction::name() const {
  return staticName();
}

const char* VersionAction::description() const {
  return "Prints the version number of MathicGB and exits.  This is useful for "
    "verifying which version is installed or for reporting in bug reports.";
}

const char* VersionAction::shortDescription() const {
  return "Print Mathicgb version";
}

void VersionAction::pushBackParameters(std::vector<mathic::CliParameter*>& parameters) {}

MATHICGB_NAMESPACE_END
