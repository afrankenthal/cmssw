import FWCore.ParameterSet.Config as cms

generator = cms.EDFilter("Pythia8PtDisplacedGun",

    maxEventsToPrint = cms.untracked.int32(1),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    pythiaHepMCVerbosity = cms.untracked.bool(True),

    PGunParameters = cms.PSet(
        ParticleID = cms.vint32(15),
        AddAntiParticle = cms.bool(True),
        MakeDisplaced = cms.bool(True),
        CustomCollimation = cms.bool(True),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
        MinPt = cms.double(10.0),
        MaxPt = cms.double(100.0),
        MinEta = cms.double(0.0),
        MaxEta = cms.double(2.4),
        MinProdRadius = cms.double(0.0),
        MaxProdRadius = cms.double(10.0),
        PairDeltaPhi = cms.double(1.570796),
        TrueMuonium = cms.bool(False)
        ),

    ExternalDecays = cms.PSet(
        Tauola = cms.untracked.PSet(
            UseTauolaPolarization = cms.bool(True),
            InputCards = cms.PSet
            ( 
                pjak1 = cms.int32(0), # 1 = electron mode
                pjak2 = cms.int32(0), # 2 = muon mode
                #mdtau = cms.int32(240)  # (any) tau -> nu pi+- 
                mdtau = cms.int32(214)  # one tau -> L+-, other taus -> hadrons
                )
            ),
        parameterSets = cms.vstring('Tauola')
        ),

    PythiaParameters = cms.PSet(
        py8ZDecaySettings = cms.vstring(  '23:onMode = off', # turn OFF all Z decays
            '23:onIfAny = 15'  # turn ON Z->tautau
            ),
        parameterSets = cms.vstring(  
            #'py8ZDecaySettings' 
            )
        )
    )
