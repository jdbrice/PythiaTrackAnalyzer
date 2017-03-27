#ifndef DIMUON_ANALYZER_H
#define DIMUON_ANALYZER_H

#include "TreeAnalyzer.h"
#include "PythiaTrackReader.h"
#include "TLorentzVector.h"
#include "TwoBodyDecayer.h"
#include "TRandom3.h"

#include "CutCollection.h"

#define LOGURU_IMPLEMENTATION 1
#include "Pythia6TreeMaker/loguru.h"

class DimuonAnalyzer : public TreeAnalyzer
{
protected:
	PythiaTrackReader ptr;
	vector<PythiaTrack*> mup;
	vector<PythiaTrack*> mum;
	vector<PythiaTrack*> decayTwoBody;


	CutCollection rcol;

	map<string, int> kfcodes;

	float C_PROB_PION_DECAY = 0.1f;

	class TrackInfo{
	public:
		TrackInfo(){
			tr = nullptr;
		}
		TrackInfo( PythiaTrack *_tr ){
			tr = _tr;
			lv.SetXYZM( tr->mPx, tr->mPy, tr->mPz, tr->mMass );
		}
		TrackInfo( TLorentzVector _lv ){
			tr = new PythiaTrack();
			tr->mPx = _lv.Px();
			tr->mPy = _lv.Py();
			tr->mPz = _lv.Pz();

			lv = _lv;
		}
		TLorentzVector lv;
		PythiaTrack * tr;
	};
	class TrackPair{
	public:
		TrackPair(){}
		TrackPair( TrackInfo _tr1, TrackInfo _tr2, TrackInfo _p ){
			tr1    = _tr1;
			tr2    = _tr2;
			parent = _p;
		}
		// TrackPair( TrackInfo _ti1, TrackInfo _ti2 ){
		// 	ti1 = _ti1;
		// 	ti2 = _ti2;
		// 	parent = TrackInfo( ptr.getTrackParent( _ti1.tr ) );
		// 	lv = ti1.lv + ti2.lv;
		// }
		TLorentzVector lv;
		TrackInfo tr1;
		TrackInfo tr2;
		TrackInfo parent;
	};

	vector<TrackPair> pairs;
	vector<TrackInfo> pos;
	vector<TrackInfo> neg;


public:
	virtual const char* classname() const { return "DimuonAnalyzer"; }
	DimuonAnalyzer() {}
	~DimuonAnalyzer() {}

	virtual void initialize(){
		LOG_SCOPE_FUNCTION(INFO);
		TreeAnalyzer::initialize();

		int seed = config.getInt( "seed", 0 );
		LOG_F( INFO, "gRandom SEED = %d", seed );
		gRandom = new TRandom3();
		gRandom->SetSeed( seed );

		C_PROB_PION_DECAY = config.getFloat( "ProbPionDecay", 0.05 );

		ptr.create( chain );

		kfcodes[ "mu+" ]      = -13;
		kfcodes[ "mu-" ]      = 13;
		kfcodes[ "rho0" ]     = 113;
		kfcodes[ "pi-" ]      = 211;
		kfcodes[ "pi+" ]      = -211;
		kfcodes[ "k+" ]       = 321;
		kfcodes[ "omega" ]    = 223;
		kfcodes[ "phi" ]      = 333;
		kfcodes[ "jpsi" ]     = 443;
		kfcodes[ "eta" ]      = 221;
		kfcodes[ "etaprime" ] = 331;

		rcol.init( config, "Ranges" );
		rcol.report();
	}
protected:

	

	virtual void printTrack( PythiaTrack * _trk ){
		std::cout << ">>>==================================================" << std::endl;
		std::cout << "mId       = "  << _trk-> mId       << std::endl;
		std::cout << "mEnergy   = "  << _trk-> mEnergy   << std::endl;
		std::cout << "mKF       = "  << _trk-> mKF       << std::endl;
		std::cout << "mKS       = "  << _trk-> mKS       << std::endl;
		std::cout << "mLifetime = "  << _trk-> mLifetime << std::endl;
		std::cout << "mMass     = "  << _trk-> mMass     << std::endl;
		std::cout << "mParent   = "  << _trk-> mParent   << std::endl;
		std::cout << "mPx       = "  << _trk-> mPx       << std::endl;
		std::cout << "mPy       = "  << _trk-> mPy       << std::endl;
		std::cout << "mPz       = "  << _trk-> mPz       << std::endl;
		std::cout << "mTime     = "  << _trk-> mTime     << std::endl;
		std::cout << "mVx       = "  << _trk-> mVx       << std::endl;
		std::cout << "mVy       = "  << _trk-> mVy       << std::endl;
		std::cout << "mVz       = "  << _trk-> mVz       << std::endl;
		std::cout << "<<<==================================================" << std::endl;
	}

	bool commonParent( TrackInfo ti1, TrackInfo ti2 ){
		if ( nullptr == ti1.tr || nullptr == ti2.tr ) return false;
		if ( ti1.tr->mParent == ti2.tr->mParent ) return true;
		return false;
	}

	virtual void makeTwoBodyDecay( PythiaTrack * _parent ){
		if ( nullptr == _parent ) return;

		TwoBodyDecayer tbd;
		TrackInfo ti( _parent );
		tbd.decay( ti.lv, 0.105 );

		TrackInfo ti1( tbd.lvl1 );
		TrackInfo ti2( tbd.lvl2 );
		
		ti1.tr = new PythiaTrack();
		ti1.tr->mParent = _parent->mId + 1;

		ti2.tr = new PythiaTrack();
		ti2.tr->mParent = _parent->mId + 1;

		pos.push_back( ti1 );
		neg.push_back( ti2 );
	}

	virtual void makePionDecay( PythiaTrack * _parent, vector<TrackInfo> &col ){
		if ( nullptr == _parent ) return;

		TwoBodyDecayer tbd;
		TrackInfo ti( _parent );
		tbd.decay( ti.lv, 0.105, 0.0 );

		TrackInfo ti1( tbd.lvl1 );
		
		ti1.tr = new PythiaTrack();
		ti1.tr->mParent = _parent->mId + 1;

		col.push_back( ti1 );

		book->fill( "PionMuonDeltaPhi", ti.lv.DeltaPhi( ti1.lv ) );
		book->fill( "PionMuonAbsDeltaPhi", abs(ti.lv.DeltaPhi( ti1.lv )) );
	}

	virtual void findPlcs(){

		pos.clear();
		neg.clear();

		int nPip=0, nPin=0;
		int nTracks = ptr.getNTracks();
		for ( int iTrack = 0; iTrack < nTracks; iTrack++  ){
			PythiaTrack * pyTr    = ptr.getTrack( iTrack );
			PythiaTrack * pyTrPar = ptr.getTrackParent( pyTr );

			if ( kfcodes[ "mu-" ] == pyTr->mKF ){
				neg.push_back( TrackInfo( pyTr ) );
			}
			if ( kfcodes[ "mu+" ] == pyTr->mKF ){
				pos.push_back( TrackInfo( pyTr ) );
			}

			// DECAY
			// if ( kfcodes[ "omega" ] == pyTr->mKF ){
			// 	makeTwoBodyDecay( pyTr );
			// }
			// if ( kfcodes[ "phi" ] == pyTr->mKF ){
			// 	makeTwoBodyDecay( pyTr );
			// }
			// if ( kfcodes[ "jpsi" ] == pyTr->mKF ){
			// 	makeTwoBodyDecay( pyTr );
			// }

			if ( kfcodes[ "pi+" ] == pyTr->mKF ){
				if ( gRandom->Uniform(0,1.) < C_PROB_PION_DECAY )
					makePionDecay( pyTr, pos );
				nPip++;
			}
			if ( kfcodes[ "pi-" ] == pyTr->mKF ){
				if ( gRandom->Uniform(0,1.) < C_PROB_PION_DECAY )
					makePionDecay( pyTr, neg );
				nPin++;
			}
		}

		book->fill( "muons", pos.size(), neg.size() );
		book->fill( "pions", nPip, nPin );
	}

	virtual void ulsPairs(){
		for ( TrackInfo &p : pos ){
			for ( TrackInfo &n : neg ){
				TLorentzVector lv;
				lv = p.lv + n.lv;

				if ( commonParent( p, n ) ){
					PythiaTrack * parent = ptr.getTrackParent( p.tr );
					
					int pKF = parent->mKF;

					if ( kfcodes[ "rho0" ] == pKF )
						book->fill( "true_rho0", lv.M() );
					if ( kfcodes[ "omega" ] == pKF )
						book->fill( "true_omega", lv.M() );
					if ( kfcodes[ "phi" ] == pKF )
						book->fill( "true_phi", lv.M() );
					if ( kfcodes[ "jpsi" ] == pKF )
						book->fill( "true_jpsi", lv.M() );
					if ( kfcodes[ "eta" ] == pKF )
						book->fill( "true_eta", lv.M() );
					if ( kfcodes[ "etaprime" ] == pKF )
						book->fill( "true_etaprime", lv.M() );

					book->fill( "true_all", lv.M() );
				}	// commonParent
				else { // combinatorial only
					book->fill( "bg_uls_all", lv.M() );
				}
				book->fill( "fg_uls_all", lv.M() );

			} // neg
		} // pos
	}

	virtual void lsPairs(){
		for ( TrackInfo &p1 : pos ){
			for ( TrackInfo &p2 : pos ){
				if ( p1.tr == p2.tr ) continue;

				TLorentzVector lv;
				lv = p1.lv + p2.lv;

				book->fill( "ls", lv.M() );
				book->fill( "lsp", lv.M() );

			} // p2 : pos
		}// p1 : pos

		for ( TrackInfo &n1 : neg ){
			for ( TrackInfo &n2 : neg ){
				if ( n1.tr == n2.tr ) continue;

				TLorentzVector lv;
				lv = n1.lv + n2.lv;

				book->fill( "ls", lv.M() );
				book->fill( "lsn", lv.M() );

			} // n2 : neg
		}// n1 : neg
	} // lsPairs

	virtual bool keepEvent(){

		findPlcs();

		if ( pos.size() <= 0 && neg.size() <= 0 ) return false;

		return true;
	}


	virtual void analyzeEvent(){
		
		ulsPairs();
		lsPairs();

		return;
	} // analyze event

};


#endif