/*
Package:    GeneralInterface/GenFilters/pMSSMFilter
Class:      pMSSMFilter

class pMSSMFilter pMSSMFilter.cc GeneratorInterface/GenFilters/src/pMSSMFilter.cc

Description: EDFilter which checks the event passes a baseline selection for the run-II pMSSM effort.

Implementation: 

The following input parameters are used 
  gpssrc = cms.InputTag("X") : gen particle collection label as input
  jetsrc  = cms.InputTag("X") : genjet collection collection label as input
  jetPtCut = cms.double(#) : GenJet pT cut for HT
  jetEtaCut = cms.double(#) : GenJet eta cut for HT
  genHTcut = cms.double(#) : GenHT cut
  muPtCut = cms.double(#) : muon pT cut
  muEtaCut = cms.double(#) : muon eta cut
  elPtCut = cms.double(#) : electron pT cut
  elEtaCut = cms.double(#) : electron eta cut
  gammaPtCut = cms.double(#) : photon pT cut
  gammaEtaCut = cms.double(#) : photon eta cut
  loosemuPtCut = cms.double(#) : loose muon pT cut
  looseelPtCut = cms.double(#) : loose electron pT cut
  loosegammaPtCut = cms.double(#) : loose photon pT cut
  veryloosegammaPtCut = cms.double(#) : even looser photon pT cut
Original Author:  Malte Mrowietz
         Created:  Jun 2019
*/

//System include files
#include <memory>
#include <vector>
#include <math.h>
//User include files
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

//Class declaration
class pMSSMFilter : public edm::EDFilter {
public:
  explicit pMSSMFilter(const edm::ParameterSet&);
  ~pMSSMFilter() override;

private:
  bool filter(edm::Event&, const edm::EventSetup&) override;

  //Member data
  edm::EDGetTokenT<reco::GenParticleCollection> token_;
  edm::EDGetTokenT<reco::GenJetCollection> token2_;
  double muPtCut_, muEtaCut_,elPtCut_,elEtaCut_,gammaPtCut_,gammaEtaCut_,loosemuPtCut_,looseelPtCut_,loosegammaPtCut_,veryloosegammaPtCut_,jetPtCut_, jetEtaCut_, genHTcut_;
};

//Constructor
pMSSMFilter::pMSSMFilter(const edm::ParameterSet& params)
  :  token_(consumes<reco::GenParticleCollection>(params.getParameter<edm::InputTag>("gpssrc"))),
     token2_(consumes<reco::GenJetCollection>(params.getParameter<edm::InputTag>("jetsrc"))),
     muPtCut_(params.getParameter<double>("muPtCut")),
     muEtaCut_(params.getParameter<double>("muEtaCut")),
     elPtCut_(params.getParameter<double>("elPtCut")),
     elEtaCut_(params.getParameter<double>("elEtaCut")),
     gammaPtCut_(params.getParameter<double>("gammaPtCut")),
     gammaEtaCut_(params.getParameter<double>("gammaEtaCut")),
     loosemuPtCut_(params.getParameter<double>("loosemuPtCut")),
     looseelPtCut_(params.getParameter<double>("looseelPtCut")),
     loosegammaPtCut_(params.getParameter<double>("loosegammaPtCut")),
     veryloosegammaPtCut_(params.getParameter<double>("veryloosegammaPtCut")),
     jetPtCut_(params.getParameter<double>("jetPtCut")),
     jetEtaCut_(params.getParameter<double>("jetEtaCut")),
     genHTcut_(params.getParameter<double>("genHTcut")){}


//Destructor
pMSSMFilter::~pMSSMFilter() {}

bool pMSSMFilter::filter(edm::Event& evt, const edm::EventSetup& params) {
    using namespace std;
    using namespace edm;
    using namespace reco;
    edm::Handle<reco::GenParticleCollection> gps; 
    evt.getByToken(token_, gps);
    edm::Handle<reco::GenJetCollection> generatedJets;
    evt.getByToken(token2_, generatedJets);
    int looseel_ = 0;
    int loosemu_ = 0;
    int loosegamma_ = 0;
    int veryloosegamma_ = 0;
    float decaylength;
    for (std::vector<reco::GenParticle>::const_iterator it = gps->begin(); it != gps->end(); ++it) {
        const reco::GenParticle& gp = *it;
        if (gp.status() != 1){
            continue;
        }
        if (gp.isLastCopy()){
            if (abs(gp.pdgId()) == 13){
                if (gp.pt() > muPtCut_ && abs(gp.eta()) < muEtaCut_) {
                    return true;
                }
                if (gp.pt() > loosemuPtCut_ && abs(gp.eta()) < muEtaCut_) {
                    loosemu_ +=1;
                }
            }
            if (abs(gp.pdgId()) == 11){
                if (gp.pt() > elPtCut_ && abs(gp.eta()) < elEtaCut_) {
                    return true;
                }
                if (gp.pt() > looseelPtCut_ && abs(gp.eta()) < elEtaCut_) {
                    looseel_ += 1;
                }
            }
            if (abs(gp.pdgId()) == 22){
                if (gp.pt() > gammaPtCut_ && abs(gp.eta()) < gammaEtaCut_){
                    return true;
                }
                if (gp.pt() > loosegammaPtCut_ && abs(gp.eta()) < gammaEtaCut_){
                    loosegamma_ += 1;
                }
                else{
                    if (gp.pt() > veryloosegammaPtCut_ && abs(gp.eta()) < gammaEtaCut_){
                        veryloosegamma_ += 1;
                    }
                }
            }
            if (abs(gp.pdgId())==1000024) {
                if (gp.numberOfDaughters()>0){
                    decaylength = sqrt(pow(gp.vx() - gp.daughter(0)->vx(),2) + pow(gp.vy() - gp.daughter(0)->vy(),2));
                    if (decaylength > 300){
                        return true;
                    }
                }
                else {
                    return true;
                }
            }
        }
    
    }
    if (looseel_ + loosemu_ + loosegamma_ > 1){
        return true;
    }
    if (loosegamma_ >0 && veryloosegamma_ > 0) {
        return true;
    }
    double genHT = 0.0;
    for(std::vector<reco::GenJet>::const_iterator it = generatedJets->begin(); it != generatedJets->end(); ++it)
    {
        const reco::GenJet& gjet = *it;
        //Add GenJet pt to genHT if GenJet complies with given HT definition
        if(gjet.pt() > jetPtCut_ && abs(gjet.eta()) < jetEtaCut_) 
        {
            genHT += gjet.pt();
        }
    }
    return (genHT > genHTcut_);
}

// Define module as a plug-in
DEFINE_FWK_MODULE(pMSSMFilter);
