#include <sys/time.h>

#include "EventFilter/FEDInterface/interface/fed_header.h"
#include "EventFilter/Utilities/interface/AuxiliaryMakers.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace evf{
  namespace evtn{

	edm::EventAuxiliary makeEventAuxiliary(const tcds::Raw_v1* tcds,
					     unsigned int runNumber,
                                             unsigned int lumiSection,
					     std::string const &processGUID,
                                             bool verifyLumiSection){
	edm::EventID eventId(runNumber, // check that runnumber from record is consistent
                             lumiSection,
			     tcds->header.eventNumber);

        edm::TimeValue_t time = static_cast<edm::TimeValue_t>(((uint64_t)tcds->bst.gpstimehigh << 32) | tcds->bst.gpstimelow);
        if (time == 0) {
          timeval stv;
          gettimeofday(&stv,0);
          time = stv.tv_sec;
          time = (time << 32) + stv.tv_usec;
        }

        const uint64_t orbitnr = ((uint64_t)tcds->header.orbitHigh << 16) | tcds->header.orbitLow;
        const uint32_t recordLumiSection = tcds->header.lumiSection;

        if (verifyLumiSection && recordLumiSection != lumiSection)
          edm::LogWarning("AuxiliaryMakers") << "Lumisection mismatch, external : " << lumiSection << ", record : " << recordLumiSection;
        if ((orbitnr >> 18) + 1 != recordLumiSection)
          edm::LogWarning("AuxiliaryMakers") << "Lumisection and orbit number mismatch, LS : " << lumiSection << ", LS from orbit: " << ((orbitnr >> 18) + 1) << ", orbit:" << orbitnr;

	return edm::EventAuxiliary(eventId,
				   processGUID,
				   edm::Timestamp(time),
				   true,
                                   (edm::EventAuxiliary::ExperimentType)FED_EVTY_EXTRACT(tcds->fedHeader.eventid),
				   (int)tcds->header.bxid,
				   ((uint32_t)(tcds->bst.lhcFillHigh)<<16)|tcds->bst.lhcFillLow,
				   (int)(orbitnr&0x7fffffffU));//framework supports only 32-bit signed
      }
  }
}
