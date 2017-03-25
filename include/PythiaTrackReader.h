#ifndef PYTHIA_TRACK_READER_H
#define PYTHIA_TRACK_READER_H

#include "PythiaTrack.h"
#include "TClonesArray.h"

class PythiaTrackReader : public IObject
{
public:
	virtual const char* classname() const { return "PythiaTrackReader"; }
	PythiaTrackReader() {}
	PythiaTrackReader( TChain * _tree, string _bn = "Tracks" ) {
		create( _tree, _bn );
	}
	~PythiaTrackReader() {}

	PythiaTrack* const getTrack( int iTrack = 0 ){ return (PythiaTrack*)tracks->At( iTrack ); }
	PythiaTrack* const getTrackParent( PythiaTrack* pt ){ 
		int parentId = pt->mParent - 1;
		if ( parentId >= 0 )
			return getTrack( parentId );
		return nullptr;
	}

	int getNTracks() const { return tracks->GetEntries(); }

	void create( TChain * _tree, string _bn = "Tracks"){
		if ( nullptr == _tree ){
			ERROR( classname(), "Invalid TChain" );
			return;
		}
		b_tracks = _tree->GetBranch( _bn.c_str() );
		if ( !b_tracks ){
			ERROR( classname(), _bn << " DNE" );
			return;
		}

		tracks = new TClonesArray( "PythiaTrack" );
		_tree->SetBranchAddress( _bn.c_str(), &tracks );
		b_tracks->SetAutoDelete( kFALSE );

		INFO( classname(), "Set Branch Address for " << _bn );
	}
protected:


	TBranch * b_tracks;
	TClonesArray * tracks = nullptr;

};

#endif