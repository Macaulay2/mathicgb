// MathicGB copyright 2012 all rights reserved. MathicGB comes with ABSOLUTELY
// NO WARRANTY and is licensed as GPL v2.0 or later - see LICENSE.txt.
#include "mathicgb/stdinc.h"
#include "CommonParams.hpp"

#include "mathicgb/LogDomain.hpp"
#include "mathicgb/LogDomainSet.hpp"

MATHICGB_DEFINE_LOG_ALIAS("default", "F4Detail,SPairs");

MATHICGB_NAMESPACE_BEGIN

CommonParams::CommonParams(size_t minDirectParams, size_t maxDirectParams):
  mTracingLevel("tracingLevel",
    "How much information to print out about what the program does. No "
    "information is shown if the value is zero. Higher values "
    "result in more information.",
    0),

  mThreadCount("threadCount",
    "Specifies how many threads to use at a time. A value of 0 indicates that "
    "the program should choose the optimal number of threads.",
    0),

  mLogs("log",
    "Enable the specified log. Do \"help logs\" to see all available logs and "
    "the details of how to configure what kind of logging you want. "
    "To enable logs X, Y and Z, do \"-log X,Y,Z\". "
    "To enabled all logs, do \"-log\" or \"-log all\".",
    "none"),

  mMinDirectParams(minDirectParams),
  mMaxDirectParams(maxDirectParams)
{
}

void CommonParams::directOptions(
  std::vector<std::string> tokens,
  mathic::CliParser& parser
) {
  if (tokens.size() < mMinDirectParams)
    mathic::reportError("Too few direct options");
  if (tokens.size() > mMaxDirectParams)
    mathic::reportError("Too many direct options");
  mDirectParameters = std::move(tokens);
}

void CommonParams::pushBackParameters(
  std::vector<mathic::CliParameter*>& parameters
) {
  parameters.push_back(&mLogs);
  parameters.push_back(&mTracingLevel);
  parameters.push_back(&mThreadCount);
}

void CommonParams::perform() {
  const std::string logs = mLogs.value().empty() ? "default" : mLogs.value();
  LogDomainSet::singleton().performLogCommands(logs);
  tracingLevel = mTracingLevel.value();

  // delete the old init object first to make the new one take control.
  mTaskArena.reset();
  mTaskArena = make_unique<mtbb::task_arena>(mtbb::numThreads(mThreadCount.value()));
}

void CommonParams::registerFileNameExtension(std::string extension) {
  MATHICGB_ASSERT(!extension.empty());
  mExtensions.push_back(std::move(extension));
}

size_t CommonParams::inputFileCount() const {
  return mDirectParameters.size();
}

std::string CommonParams::inputFileName(size_t i) {
  MATHICGB_ASSERT(i < inputFileCount());
  return mDirectParameters[i];
}

std::string CommonParams::inputFileNameStem(size_t i) {
  MATHICGB_ASSERT(i < inputFileCount());
  const auto& str = mDirectParameters[i];
  const auto toStrip = inputFileNameExtension(i);
  MATHICGB_ASSERT
    (toStrip.size() < str.size() || (toStrip.empty() && str.empty()));
  return str.substr(0, str.size() - toStrip.size());
}

std::string CommonParams::inputFileNameExtension(size_t i) {
  MATHICGB_ASSERT(i < inputFileCount());
  const auto& str = mDirectParameters[i];
  const auto end = mExtensions.end();
  for (auto it = mExtensions.begin(); it != end; ++it) {
    if (
      str.size() >= it->size() &&
      str.substr(str.size() - it->size(), it->size()) == *it
    )
      return *it;
  }
  return std::string();
}

MATHICGB_NAMESPACE_END
