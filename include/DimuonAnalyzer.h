#ifndef DIMUON_ANALYZER_H
#define DIMUON_ANALYZER_H

#include "TreeAnalyzer.h"
#include "PythiaTrackReader.h"
#include "TLorentzVector.h"
#include "TwoBodyDecayer.h"


class DimuonAnalyzer : public TreeAnalyzer
{
protected:
	PythiaTrackReader ptr;
	vector<PythiaTrack*> mup;
	vector<PythiaTrack*> mum;
	vector<PythiaTrack*> decayTwoBody;


	

	map<string, int> kfcodes;

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
		TreeAnalyzer::initialize();

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
	}
protected:

	

	virtual void printTrack( PythiaTrack * _trk ){
		std::cout << "==================================================" << std::endl;
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

		// cout << "tbd.lvl1 ( M=" << tbd.lvl1.M() << ", P=(" << tbd.lvl1.Px() << ", " << tbd.lvl1.Py() << ", " << tbd.lvl1.Pz() << " ) )" << endl;
		// cout << "tbd.lvl2 ( M=" << tbd.lvl2.M() << ", P=(" << tbd.lvl2.Px() << ", " << tbd.lvl2.Py() << ", " << tbd.lvl2.Pz() << " ) )" << endl;

		pos.push_back( ti1 );
		neg.push_back( ti2 );
	}

	virtual void makePionDecay( PythiaTrack * _parent, vector<TrackInfo> &col ){
		if ( nullptr == _parent ) return;

		// TwoBodyDecayer tbd1, tbd2;
		// TrackInfo ti( _parent );
		// tbd1.decay( ti.lv, 0.05 );
		// tbd2.decay( ti.lv, 0.05, 0.05 );

		// cout << "parent.lv ( M=" << ti.lv.M() << ", P=(" << ti.lv.Px() << ", " << ti.lv.Py() << ", " << ti.lv.Pz() << " ) )" << endl;
		
		// cout << "tbd1.lvl1 ( M=" << tbd1.lvl1.M() << ", P=" << tbd1.lvl1.P() << "(" << tbd1.lvl1.Px() << ", " << tbd1.lvl1.Py() << ", " << tbd1.lvl1.Pz() << " ) )" << endl;
		// cout << "tbd1.lvl2 ( M=" << tbd1.lvl2.M() << ", P=" << tbd1.lvl2.P() << "(" << tbd1.lvl2.Px() << ", " << tbd1.lvl2.Py() << ", " << tbd1.lvl2.Pz() << " ) )" << endl;

		// cout << "tbd2.lvl1 ( M=" << tbd2.lvl1.M() << ", P=" << tbd2.lvl1.P() << "(" << tbd2.lvl1.Px() << ", " << tbd2.lvl1.Py() << ", " << tbd2.lvl1.Pz() << " ) )" << endl;
		// cout << "tbd2.lvl2 ( M=" << tbd2.lvl2.M() << ", P=" << tbd2.lvl2.P() << "(" << tbd2.lvl2.Px() << ", " << tbd2.lvl2.Py() << ", " << tbd2.lvl2.Pz() << " ) )" << endl;

		TwoBodyDecayer tbd;
		TrackInfo ti( _parent );
		tbd.decay( ti.lv, 0.105, 0.0 );

		TrackInfo ti1( tbd.lvl1 );
		// TrackInfo ti2( tbd.lvl2 );
		
		ti1.tr = new PythiaTrack();
		ti1.tr->mParent = _parent->mId + 1;

		// ti2.tr = new PythiaTrack();
		// ti2.tr->mParent = _parent->mId + 1;

		col.push_back( ti1 );

		// cout << "deltaPhi( pi, muon) = " << ti.lv.DeltaPhi( ti1.lv ) * (180 / 3.1415926) << endl;
		book->fill( "PionMuonDeltaPhi", ti.lv.DeltaPhi( ti1.lv ) );
		book->fill( "PionMuonAbsDeltaPhi", abs(ti.lv.DeltaPhi( ti1.lv )) );
		
	}

	virtual void findPlcs(){

		pos.clear();
		neg.clear();

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
			if ( kfcodes[ "omega" ] == pyTr->mKF ){
				makeTwoBodyDecay( pyTr );
			}
			// if ( kfcodes[ "phi" ] == pyTr->mKF ){
			// 	makeTwoBodyDecay( pyTr );
			// }
			// if ( kfcodes[ "jpsi" ] == pyTr->mKF ){
			// 	makeTwoBodyDecay( pyTr );
			// }

			if ( kfcodes[ "pi+" ] == pyTr->mKF ){
				makePionDecay( pyTr, pos );
			}
			if ( kfcodes[ "pi-" ] == pyTr->mKF ){
				makePionDecay( pyTr, neg );
			}
		}

		book->fill( "muons", pos.size(), neg.size() );
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

		// mup.clear();
		// mum.clear();
		// decayTwoBody.clear();

		// pairs.clear();

		// int nTracks = ptr.getNTracks();
		// for ( int iTrack = 0; iTrack < nTracks; iTrack++  ){
		// 	PythiaTrack * pyTr    = ptr.getTrack( iTrack );
		// 	PythiaTrack * pyTrPar = ptr.getTrackParent( pyTr );

		// 	book->fill( "KF", pyTr->mKF );
		// 	book->fill( "KS", pyTr->mKS );

			

		// 	int parentKF = 0;
		// 	if ( pyTrPar )
		// 		parentKF = pyTrPar->mKF;
		// 	if ( kfcodes[ "mu-" ] == pyTr->mKF ){
		// 		book->fill( "mumParents", parentKF );
		// 		mum.push_back( pyTr );
		// 		// printTrack( pyTr );
		// 	}
		// 	if ( kfcodes["mu+"] == pyTr->mKF ){
		// 		book->fill( "mupParents", parentKF );
		// 		mup.push_back( pyTr );
		// 		// printTrack( pyTr );
		// 	}


		// 	// TODO add BR
		// 	if ( kfcodes[ "omega" ] == pyTr->mKF ){
		// 		printTrack( pyTr );
		// 		decayTwoBody.push_back( pyTr );
		// 	}

		// 	if ( kfcodes[ "jpsi" ] == pyTr->mKF ){
		// 		printTrack( pyTr );
		// 		decayTwoBody.push_back( pyTr );
		// 	}

		// } // iTrack

		// if ( mup.size() <= 0 || mum.size() <= 0 ) return false;

		// return true;
	}


	/* makeTruePairs
	 *
	 * 1. Make TrackPairs from each true pair
	 * 2. Make decays for each undecayed parent
	 * 	a. Add the new plcs to the list of pos/neg for future use
	 * 3. loop over true pairs to make plots
	 *
	 * @return void
	 */
	// void makeTruePairs(){

	// 	vector<TLorentzVector> lvp, lvn;
	// 	vector<int> parentKF;

	// 	int nPos = mup.size();
	// 	int nNeg = mum.size();

	// 	for ( int iPos = 0; iPos < nPos; iPos++ ){
	// 		PythiaTrack* posplc = mup[ iPos ];
	// 		for ( int iNeg = 0; iNeg < nNeg; iNeg++ ){
	// 			PythiaTrack* negplc = mum[ iNeg ];
	// 			if ( posplc->mParent != negplc->mParent ) continue;

	// 			PythiaTrack * parent = ptr.getTrackParent( posplc );
	// 			if ( parent ){
	// 				TLorentzVector lv1, lv2;
	// 				lv1.SetXYZM( negplc->mPx, negplc->mPy, negplc->mPz, negplc->mMass );
	// 				lv2.SetXYZM( posplc->mPx, posplc->mPy, posplc->mPz, posplc->mMass );
					
	// 				lvn.push_back( lv1 );
	// 				lvp.push_back( lv2 );
	// 				parentKF.push_back( parent->mKF );

	// 				TrackPair tp;
	// 				tp.lv1 = lv1;
	// 				tp.lv2 = lv2;
	// 				tp.tr1 = posplc;
	// 				tp.tr2 = negplc;
	// 				tp.parent = parent;

	// 				pairs.push_back( tp );

	// 			}
	// 		}
	// 	}

	// 	// DECAY the undecayed parents
	// 	for ( PythiaTrack * pyTr : decayTwoBody ){
	// 		TwoBodyDecayer tbd;
			
	// 		TLorentzVector lv;
	// 		lv.SetXYZM( pyTr->mPx, pyTr->mPy, pyTr->mPz, pyTr->mMass );

	// 		tbd.decay( lv, 0.105 );
	// 		lvp.push_back( tbd.lvl1 ) ;
	// 		lvn.push_back( tbd.lvl2 ) ;
	// 		parentKF.push_back( pyTr->mKF );

	// 		TrackPair tp;
	// 		tp.lv1 = tbd.lvl1;
	// 		tp.lv2 = tbd.lvl2;
	// 		tp.tr1 = nullptr;
	// 		tp.tr2 = nullptr;
	// 		tp.parent = pyTr;
	// 		pairs.push_back( tp );

	// 		PythiaTrack * trp = new PythiaTrack();


	// 	}


	// 	for ( int iPair = 0; iPair < lvp.size(); iPair++ ){
	// 		int pKF            = parentKF[ iPair ];

	// 		TLorentzVector lv;
	// 		lv = lvp[ iPair ] + lvn[ iPair ];

	// 		if ( kfcodes[ "rho0" ] == pKF )
	// 			book->fill( "true_rho0", lv.M() );
	// 		if ( kfcodes[ "omega" ] == pKF )
	// 			book->fill( "true_omega", lv.M() );
	// 		if ( kfcodes[ "phi" ] == pKF )
	// 			book->fill( "true_phi", lv.M() );
	// 		if ( kfcodes[ "jpsi" ] == pKF )
	// 			book->fill( "true_jpsi", lv.M() );
	// 		if ( kfcodes[ "eta" ] == pKF )
	// 			book->fill( "true_eta", lv.M() );
	// 		if ( kfcodes[ "etaprime" ] == pKF )
	// 			book->fill( "true_etaprime", lv.M() );

	// 		book->fill( "true_all", lv.M() );

	// 	}

	// }


	virtual void analyzeEvent(){
		
		ulsPairs();
		lsPairs();

		return;
		int nPos = mup.size();
		int nNeg = mum.size();


		// makeTruePairs();

		// // true ls pairs
		// for ( int iPos = 0; iPos < nPos; iPos++ ){
		// 	PythiaTrack* posplc1 = mup[ iPos ];
		// 	for ( int iPos2 = iPos; iPos2 < nPos; iPos2++ ){
		// 		if ( iPos == iPos2 ) continue;
		// 		PythiaTrack* posplc2 = mup[ iPos2 ];

		// 		if ( posplc1->mParent != posplc2->mParent ) continue;

		// 		PythiaTrack * parent = ptr.getTrackParent( posplc1 );
		// 		if ( parent ){
		// 			TLorentzVector lv;
		// 			lv.SetPxPyPzE( parent->mPx, parent->mPy, parent->mPz, parent->mEnergy );
		// 			book->fill( "true_ls", lv.M() );
		// 		}
		// 	} // loop iPos2
		// } // iPos


		// foreground uls pairs
		// for ( int iPos = 0; iPos < nPos; iPos++ ){
		// 	PythiaTrack* posplc = mup[ iPos ];
		// 	for ( int iNeg = 0; iNeg < nNeg; iNeg++ ){
		// 		PythiaTrack* negplc = mum[ iNeg ];

		// 		TLorentzVector lv1, lv2, lv;
		// 		lv1.SetPxPyPzE( negplc->mPx, negplc->mPy, negplc->mPz, negplc->mEnergy );
		// 		lv2.SetPxPyPzE( posplc->mPx, posplc->mPy, posplc->mPz, posplc->mEnergy );
		// 		lv = lv1 + lv2;
		// 		book->fill( "fg_uls_all", lv.M() );
		// 		book->fill( "fg_uls_all_2d", lv.M(), lv.Pt() );

		// 		// if ( starAccepts( lv1, lv2 ) ){
		// 		// 	book->fill( "star_fg_uls_all", lv.M() );					
		// 		// }

		// 	} // iNeg
		// } // iPos


		for ( TrackPair &tp : pairs ){

		}



	} // analyze event

};


#endif