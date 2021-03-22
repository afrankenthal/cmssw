import FWCore.ParameterSet.Config as cms

generator = cms.EDFilter("Pythia8PtDisplacedGun",

    maxEventsToPrint = cms.untracked.int32(1),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    pythiaHepMCVerbosity = cms.untracked.bool(True),

    PGunParameters = cms.PSet(
        ParticleID = cms.vint32(13),
        AddAntiParticle = cms.bool(True),
        MakeDisplaced = cms.bool(True),
        CustomCollimation = cms.bool(True),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
        MinPt = cms.double(10.0),
        MaxPt = cms.double(100.0),
        MinEta = cms.double(0.0),
        MaxEta = cms.double(2.0),
        MinProdRadius = cms.double(200.0),
        MaxProdRadius = cms.double(400.0),
        PairDeltaPhi = cms.double(0),
        TrueMuonium = cms.bool(True)
        ),

     PythiaParameters = cms.PSet(
         parameterSets = cms.vstring(
             #'py8ZDecaySettings'
             )
         )

    )
