#ifndef DECAYER_H
#define DECAYER_H

#include "IObject.h"

//  ROOT
#include "TLorentzVector.h"

class Decayer : public IObject
{
public:
	virtual const char* classname() const { return "Decayer"; }
	Decayer() {}
	~Decayer() {}
	
	virtual void decay( TLorentzVector _lv, double m ) = 0;
	virtual void decay( TLorentzVector _lv, double m1, double m2 ) = 0;
	TLorentzVector lvl1, lvl2;

};


#endif