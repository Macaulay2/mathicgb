// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#ifndef MATHICGB_VERSION_ACTION_GUARD
#define MATHICGB_VERSION_ACTION_GUARD

#include <mathic.h>

MATHICGB_NAMESPACE_BEGIN

class VersionAction : public mathic::Action {
public:
  virtual void performAction();
  static const char* staticName();
  virtual const char* name() const;
  virtual const char* description() const;
  virtual const char* shortDescription() const;
  virtual void pushBackParameters(std::vector<mathic::CliParameter*>& parameters);
};

MATHICGB_NAMESPACE_END
#endif
