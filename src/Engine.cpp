

// RooBarb
#include "TaskEngine.h"
using namespace jdb;

// STL
#include <iostream>
#include <exception>

// Project
#include "DimuonAnalyzer.h"

#define LOGURU_IMPLEMENTATION 1
#include "loguru.h"

int main( int argc, char* argv[] ) {

	loguru::add_file("debug.log", loguru::Truncate, loguru::Verbosity_MAX);
	
	TaskFactory::registerTaskRunner<DimuonAnalyzer>( "DimuonAnalyzer" );

	TaskEngine engine( argc, argv );

	return 0;
}
