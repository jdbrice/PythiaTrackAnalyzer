

// RooBarb
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>

// Project
#include "DimuonAnalyzer.h"


int main( int argc, char* argv[] ) {

	TaskFactory::registerTaskRunner<DimuonAnalyzer>( "DimuonAnalyzer" );

	TaskEngine engine( argc, argv );

	return 0;
}
