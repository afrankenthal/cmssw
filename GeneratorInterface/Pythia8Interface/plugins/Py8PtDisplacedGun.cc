#include "GeneratorInterface/Core/interface/GeneratorFilter.h"
#include "GeneratorInterface/ExternalDecays/interface/ExternalDecayDriver.h"

#include "GeneratorInterface/Pythia8Interface/interface/Py8GunBase.h"

namespace gen {

class Py8PtDisplacedGun : public Py8GunBase {
   
   public:
      
      Py8PtDisplacedGun( edm::ParameterSet const& );
      ~Py8PtDisplacedGun() override {}
	 
      bool generatePartonsAndHadronize() override;
      const char* classname() const override;
	 
   private:
      
      // PtGun particle(s) characteristics
      double  fMinEta;
      double  fMaxEta;
      double  fMinPt ;
      double  fMaxPt ;
      double  fMinProdRadius;
      double  fMaxProdRadius;
      double  fPairDeltaPhi;
      bool    fTrueMuonium;
      bool    fAddAntiParticle;
      bool    fMakeDisplaced;
      bool    fCustomCollimation;

};

// implementation 
//
Py8PtDisplacedGun::Py8PtDisplacedGun( edm::ParameterSet const& ps )
   : Py8GunBase(ps) {

   // ParameterSet defpset ;
   edm::ParameterSet pgun_params = 
      ps.getParameter<edm::ParameterSet>("PGunParameters"); // , defpset ) ;
   fMinEta     = pgun_params.getParameter<double>("MinEta"); // ,-2.2);
   fMaxEta     = pgun_params.getParameter<double>("MaxEta"); // , 2.2);
   fMinPt      = pgun_params.getParameter<double>("MinPt"); // ,  0.);
   fMaxPt      = pgun_params.getParameter<double>("MaxPt"); // ,  0.);
   fMinProdRadius = pgun_params.getParameter<double>("MinProdRadius"); // , 0.);
   fMaxProdRadius = pgun_params.getParameter<double>("MaxProdRadius"); // , 0.);
   fPairDeltaPhi = pgun_params.getParameter<double>("PairDeltaPhi"); // , 0.);
   fTrueMuonium = pgun_params.getParameter<bool>("TrueMuonium"); // , false);
   fAddAntiParticle = pgun_params.getParameter<bool>("AddAntiParticle"); //, false) ;  
   fMakeDisplaced = pgun_params.getParameter<bool>("MakeDisplaced"); //, true);
   fCustomCollimation = pgun_params.getParameter<bool>("CustomCollimation"); //, false);

}

bool Py8PtDisplacedGun::generatePartonsAndHadronize()
{

   fMasterGen->event.reset();
   
   for ( size_t i=0; i<fPartIDs.size(); i++ ){

      int particleID = fPartIDs[i]; // this is PDG - need to convert to Py8 ???

      double phi = (fMaxPhi-fMinPhi) * randomEngine().flat() + fMinPhi;
      double eta  = (fMaxEta-fMinEta) * randomEngine().flat() + fMinEta;
      double the  = 2.*atan(exp(-eta));

      double pt   = (fMaxPt-fMinPt) * randomEngine().flat() + fMinPt;
      
      double mass = (fMasterGen->particleData).m0( particleID );

      double pp = pt / sin(the); // sqrt( ee*ee - mass*mass );
      double ee = sqrt( pp*pp + mass*mass );
      
      double px = pt * cos(phi);
      double py = pt * sin(phi);
      double pz = pp * cos(the);

      double radius = (fMaxProdRadius-fMinProdRadius) * randomEngine().flat() + fMinProdRadius;
      double vx = radius * cos(phi);
      double vy = radius * sin(phi);
      double vz = (70 - (-70)) * randomEngine().flat() + (-70); // luminous region in Z: (-70, 70) mm

      if ( !((fMasterGen->particleData).isParticle( particleID )) ){
         particleID = std::abs(particleID) ;
      }
      if( 1<= std::abs(particleID) && std::abs(particleID) <= 6) // quarks
	(fMasterGen->event).append( particleID, 23, 101, 0, px, py, pz, ee, mass ); 
      else if (std::abs(particleID) == 21)                   // gluons
	(fMasterGen->event).append( 21, 23, 101, 102, px, py, pz, ee, mass );
                                                         // other
      else {
        (fMasterGen->event).append( particleID, 1, 0, 0, px, py, pz, ee, mass ); 
        // -log(flat) = exponential distribution
        double tauTmp = -(fMasterGen->event).back().tau0() * log(randomEngine().flat());
        (fMasterGen->event).back().tau( tauTmp );
      }
      if (fMakeDisplaced) { // Make production vertex displaced from IP
          fMasterGen->event.back().xProd(vx);
          fMasterGen->event.back().yProd(vy);
          fMasterGen->event.back().zProd(vz);
      }

      
// Here also need to add anti-particle (if any)
// otherwise just add a 2nd particle of the same type 
// (for example, gamma)
//
      if ( fAddAntiParticle ) {
          if (!fTrueMuonium && fCustomCollimation) { // Make "back-to-back" a customizable choice
             phi += fPairDeltaPhi;
             px = pt * cos(phi);
             py = pt * sin(phi);
             vx = radius * cos(phi);
             vy = radius * sin(phi);
          }
          float sign = -1.0;
          if (fTrueMuonium) // in TM muons have identical momentum vector
              sign = +1.0;
         if( 1 <= std::abs(particleID) && std::abs(particleID) <= 6){ // quarks
	  (fMasterGen->event).append( -particleID, 23, 0, 101, -px, -py, -pz, ee, mass );
         } else if (std::abs(particleID) == 21){                   // gluons
	  (fMasterGen->event).append( 21, 23, 102, 101, -px, -py, -pz, ee, mass );
         } else {
	   if ( (fMasterGen->particleData).isParticle( -particleID ) ) {
	     (fMasterGen->event).append( -particleID, 1, 0, 0, sign*px, sign*py, sign*pz, ee, mass );
	   } else {
	     (fMasterGen->event).append( particleID, 1, 0, 0, sign*px, sign*py, sign*pz, ee, mass );
	   }
	   // -log(flat) = exponential distribution
	   double tauTmp = -(fMasterGen->event).back().tau0() * log(randomEngine().flat());
	   (fMasterGen->event).back().tau( tauTmp );
         }
         if (fMakeDisplaced) { // Make production vertex displaced from IP
             fMasterGen->event.back().xProd(sign*vx);
             fMasterGen->event.back().yProd(sign*vy);
             fMasterGen->event.back().zProd(sign*vz);
         }
      }
   }
   
   if ( !fMasterGen->next() ) return false;
   evtGenDecay();
   
   event().reset(new HepMC::GenEvent);
   return toHepMC.fill_next_event( fMasterGen->event, event().get() );
  
}

const char* Py8PtDisplacedGun::classname() const
{
   return "Py8PtDisplacedGun"; 
}

typedef edm::GeneratorFilter<gen::Py8PtDisplacedGun, gen::ExternalDecayDriver> Pythia8PtDisplacedGun;

} // end namespace

using gen::Pythia8PtDisplacedGun;
DEFINE_FWK_MODULE(Pythia8PtDisplacedGun);
