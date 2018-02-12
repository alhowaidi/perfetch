#include "ndncatchunks.hpp"
#include <stdio.h>
#include <string>
#include "aimd-statistics-collector.hpp"
#include "aimd-rtt-estimator.hpp"
#include "consumer.hpp"
#include "discover-version-fixed.hpp"
#include "discover-version-iterative.hpp"
#include "pipeline-interests-aimd.hpp"
#include "pipeline-interests-fixed-window.hpp"
#include "options.hpp"
#include "core/version.hpp"

#include <fstream>
#include <ndn-cxx/security/validator-null.hpp>

namespace ndn {
namespace chunks {

int  ndnChunks::startChunk(std::string ndnName)//, std::string pathName)
//main(int argc, char** argv)
{
//  std::string programName(argv[0]);
  Options options;
  std::string discoverType("fixed");
  std::string pipelineType("fixed");
  size_t maxPipelineSize(1);
  int maxRetriesAfterVersionFound(0);
  int64_t discoveryTimeoutMs(300);
  std::string uri;

  // congestion control parameters, CWA refers to conservative window adaptation,
  // i.e. only reduce window size at most once per RTT
  bool disableCwa(false), resetCwndToInit(false), ignoreCongMarks(false);
  double aiStep(1.0), mdCoef(0.5), alpha(0.125), beta(0.25),
         minRto(200.0), maxRto(4000.0);
  int initCwnd(1), initSsthresh(std::numeric_limits<int>::max()), k(4);
  std::string cwndPath, rttPath;

 std::string fileNameCOM = ndnName; //pathName + ndnName + "COM";
        std::string fileName =ndnName; //  pathName + ndnName;
        uri = fileName;
std::cerr << "uri: " << uri << ".." << std::endl;
        Name prefix(uri);



  if (maxPipelineSize < 1 || maxPipelineSize > 1024) {
    std::cerr << "ERROR: pipeline size must be between 1 and 1024" << std::endl;
    return 2;
  }


  try {
    Face face;

    unique_ptr<DiscoverVersion> discover;
    
    discover = make_unique<DiscoverVersionFixed>(prefix, face, options);

    unique_ptr<PipelineInterests> pipeline;
    unique_ptr<aimd::StatisticsCollector> statsCollector;
    unique_ptr<aimd::RttEstimator> rttEstimator;
    std::ofstream statsFileCwnd;
    std::ofstream statsFileRtt;

    if (pipelineType == "fixed") {
      PipelineInterestsFixedWindow::Options optionsPipeline(options);
      optionsPipeline.maxPipelineSize = maxPipelineSize;
      pipeline = make_unique<PipelineInterestsFixedWindow>(face, optionsPipeline);
    }
    else if (pipelineType == "aimd") {
      aimd::RttEstimator::Options optionsRttEst;
      optionsRttEst.isVerbose = options.isVerbose;
      optionsRttEst.alpha = alpha;
      optionsRttEst.beta = beta;
      optionsRttEst.k = k;
      optionsRttEst.minRto = aimd::Milliseconds(minRto);
      optionsRttEst.maxRto = aimd::Milliseconds(maxRto);

      rttEstimator = make_unique<aimd::RttEstimator>(optionsRttEst);

      PipelineInterestsAimd::Options optionsPipeline(options);
      optionsPipeline.disableCwa = disableCwa;
      optionsPipeline.resetCwndToInit = resetCwndToInit;
      optionsPipeline.initCwnd = static_cast<double>(initCwnd);
      optionsPipeline.initSsthresh = static_cast<double>(initSsthresh);
      optionsPipeline.aiStep = aiStep;
      optionsPipeline.mdCoef = mdCoef;
      optionsPipeline.ignoreCongMarks = ignoreCongMarks;

      auto aimdPipeline = make_unique<PipelineInterestsAimd>(face, *rttEstimator, optionsPipeline);

      if (!cwndPath.empty() || !rttPath.empty()) {
        if (!cwndPath.empty()) {
          statsFileCwnd.open(cwndPath);
          if (statsFileCwnd.fail()) {
            std::cerr << "ERROR: failed to open " << cwndPath << std::endl;
            return 4;
          }
        }
        if (!rttPath.empty()) {
          statsFileRtt.open(rttPath);
          if (statsFileRtt.fail()) {
            std::cerr << "ERROR: failed to open " << rttPath << std::endl;
            return 4;
          }
        }
        statsCollector = make_unique<aimd::StatisticsCollector>(*aimdPipeline, *rttEstimator,
                                                                statsFileCwnd, statsFileRtt);
      }

      pipeline = std::move(aimdPipeline);
    }
    else {
      std::cerr << "ERROR: Interest pipeline type not valid" << std::endl;
      return 2;
    }


std::ofstream m_outputStream;
    Consumer consumer(security::v2::getAcceptAllValidator(), options.isVerbose, fileNameCOM, m_outputStream);

    BOOST_ASSERT(discover != nullptr);
    BOOST_ASSERT(pipeline != nullptr);
    consumer.run(std::move(discover), std::move(pipeline));
    face.processEvents();
  }
  catch (const Consumer::ApplicationNackError& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 3;
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

} // namespace chunks
} // namespace ndn

