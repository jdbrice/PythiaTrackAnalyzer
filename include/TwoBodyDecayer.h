#ifndef TWO_BODY_DECAYER_H
#define TWO_BODY_DECAYER_H

#include "Decayer.h"
#include "TRandom3.h"

class TwoBodyDecayer : public Decayer
{
public:
	TwoBodyDecayer() {}
	~TwoBodyDecayer() {}
	

	virtual void applyBoost( TLorentzVector &_parent_lv, TLorentzVector &_d_lv ){

		float betaX = _parent_lv.Px() / _parent_lv.E();
		float betaY = _parent_lv.Py() / _parent_lv.E();
		float betaZ = _parent_lv.Pz() / _parent_lv.E();

		_d_lv.Boost(betaX,betaY,betaZ);
	}

	void rotate(double pIn[3], double pOut[3], double _costheta, double _sintheta, double _cosphi, double _sinphi){
		pOut[0] = pIn[0] * _costheta * _cosphi-pIn[1] * _sinphi+pIn[2] * _sintheta * _cosphi;
		pOut[1] = pIn[0] * _costheta * _sinphi+pIn[1] * _cosphi+pIn[2] * _sintheta * _sinphi;
		pOut[2] = -1.0  * pIn[0] * _sintheta + pIn[2] * _costheta;
		return;
	}

	/* Two Body Decay in the rest frame of the parent
	 *
	 * ONLY Valid for m1 == m2! The daughter mass must be equal for this simplified form.
	 * The decay is computed and then the daughters are boosted into the frame of the parent
	 */
	virtual void decay( TLorentzVector _parent_lv, double lMass ){
		// DEBUG( classname(), "lv( P=" << dts(_parent_lv.Px()) << "," << dts(_parent_lv.Py()) << "," << dts(_parent_lv.Pz()) << ", M=" << dts( _parent_lv.M() ) << ")" );

		// MUST BE EQUAL
		double M_d1 = lMass;
		double M_d2 = lMass;


		double E_d = _parent_lv.M()/2.;
		double p = sqrt(E_d*E_d - M_d1*M_d1);
		double costheta = gRandom->Uniform(-1.,1.);
		double phi = gRandom->Uniform(0,TMath::Pi()*2);

		// make sure that the magnitude of the mom vector is correct!
		// May allow these distributions to be input?
		double pz = p * costheta;
		double px = p * sqrt(1.0 - costheta * costheta) * cos(phi);
		double py = p * sqrt(1.0 - costheta * costheta) * sin(phi);

		TLorentzVector daughter1( px, py, pz, E_d);
		TLorentzVector daughter2( -px, -py, -pz, E_d );

		applyBoost(_parent_lv,daughter1);
		applyBoost(_parent_lv,daughter2);

		// Note this is slightly different than Bingchu/Shuai's code
		// That code gets d2's P correct but mass wrong
		// this method gets the entire 4-vector correct

		lvl1 = daughter1;
		lvl2 = daughter2;
	}

	/* Two Body Decay in the rest frame of the parent
	 *
	 * More general form for m1 != m2. Should give identical result as above if m1 == m2!
	 * The decay is computed and then the daughters are boosted into the frame of the parent
	 */
	virtual void decay( TLorentzVector _parent_lv, double m1, double m2 ){
		// DEBUG( classname(), "lv( P=" << dts(_parent_lv.Px()) << "," << dts(_parent_lv.Py()) << "," << dts(_parent_lv.Pz()) << ", M=" << dts( _parent_lv.M() ) << ")" );

		// MUST BE EQUAL
		double M_d1 = m1;
		double M_d2 = m2;
		double M_p  = _parent_lv.M();

		double E_d1 = ( M_p*M_p + M_d1*M_d1 - M_d2*M_d2) / ( 2 * M_p );
		double E_d2 = ( M_p*M_p + M_d2*M_d2 - M_d1*M_d1) / ( 2 * M_p );

		double p    = sqrt( pow( M_p*M_p - M_d1*M_d1 - M_d2*M_d2, 2 ) - 4 * M_d1*M_d1 * M_d2*M_d2 ) / ( 2 * M_p );

		// double p = sqrt(E_d*E_d - M_d1*M_d1);
		double costheta = gRandom->Uniform(-1.,1.);
		double phi = gRandom->Uniform(0,TMath::Pi()*2);

		// make sure that the magnitude of the mom vector is correct!
		// May allow these distributions to be input?
		double pz = p * costheta;
		double px = p * sqrt(1.0 - costheta * costheta) * cos(phi);
		double py = p * sqrt(1.0 - costheta * costheta) * sin(phi);

		TLorentzVector daughter1( px, py, pz, E_d1);
		TLorentzVector daughter2( -px, -py, -pz, E_d2 );

		applyBoost(_parent_lv,daughter1);
		applyBoost(_parent_lv,daughter2);

		// Note this is slightly different than Bingchu/Shuai's code
		// That code gets d2's P correct but mass wrong
		// this method gets the entire 4-vector correct

		lvl1 = daughter1;
		lvl2 = daughter2;
	}




};


#endif