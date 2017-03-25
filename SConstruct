#!python
import subprocess
import os
import platform

SConscript('color_SConscript')
Import( 'env' )



ROOTCFLAGS    	= subprocess.check_output( ['root-config',  '--cflags'] ).rstrip().split( " " )
ROOTLDFLAGS    	= subprocess.check_output( ["root-config",  "--ldflags"] )
ROOTLIBS      	= subprocess.check_output( ["root-config",  "--libs"] )
ROOTGLIBS     	= subprocess.check_output( ["root-config",  "--glibs"] )
ROOTLIBPATH 	= subprocess.check_output( ["root-config", "--libdir" ] )
ROOT_SYS 		= os.environ[ "ROOTSYS" ]
JDB_LIB			= os.environ[ "JDB_LIB" ]
JDB_LIB_NAME 	= 'libRooBarb.a'

cppDefines 		= {}
cppFlags 		= ['-Wall' ]#, '-Werror']
cxxFlags 		= ['-std=c++11' ]
cxxFlags.extend( ROOTCFLAGS )

paths 			= [ '.', 			# dont really like this but ended up needing for root dict to work ok
					'include', 
					JDB_LIB + "/include", 
					ROOT_SYS + "/include",
					ROOT_SYS + "/misc/table/inc/"	# Needed for TCernLib
					]

for root, dirs, files in os.walk( "include" ) :
	paths.extend( [ os.path.join(root, d) for d in dirs ] )



########################### ROOT dictionary creation ##########################
LD_LIBRARY_PATH = os.environ.get( "LD_LIBRARY_PATH", "" )

rootcint_env = Environment(ENV = {'PATH' : os.environ['PATH'], 'ROOTSYS' : os.environ[ "ROOTSYS" ], 'LD_LIBRARY_PATH' : LD_LIBRARY_PATH })
rootcint = Builder( action='rootcint -f $TARGET -c $_CPPINCFLAGS $SOURCES.file' )  
rootcint_env.Append( BUILDERS 		= { 'RootCint' : rootcint } )
# hack to make the rootcint use abs path to headers
rootcint_env[ "_CPPINCFLAGS" ] = "-I" + Dir(".").abspath + "/" + str( " -I" + Dir(".").abspath + "/").join( map( str, Glob( "#include/*" ) ) ) 


root_dict = rootcint_env.RootCint( "src/PythiaTrack.cpp", [Glob( "include/Pythia6TreeMaker/PythiaTrack.h" )] )
rootcint_env.Alias( 'dict', root_dict )
Clean( root_dict, "src/PythiaTrack_rdict.pcm" )


########################## Project Target #####################################
# common_env = env.Clone()
common_env =  Environment()
common_env.Append( ENV  = {'LD_LIBRARY_PATH' : LD_LIBRARY_PATH} )
common_env['ENV']['TERM'] = os.environ['TERM']
common_env.Append(CPPDEFINES 	= cppDefines)
common_env.Append(CPPFLAGS 		= cppFlags)
common_env.Append(CXXFLAGS 		= cxxFlags)
common_env.Append(LINKFLAGS 	= cxxFlags ) #ROOTLIBS + " " + JDB_LIB + "/lib/libJDB.a"
common_env.Append(CPPPATH		= paths)
common_env.Append(LIBS 			= [ "libRooBarbCore.a", "libRooBarbConfig.a", "libRooBarbTasks.a", "libRooBarbRootAna.a", "libRooBarbUnitTest.a", "libRooBarbExtra.a" ] )
common_env.Append(LIBPATH 		= [ JDB_LIB + "/lib/" ] )

common_env[ "_LIBFLAGS" ] = common_env[ "_LIBFLAGS" ] + " " + ROOTLIBS + " "




# REMOVE "-pthread" from clang link options in OS X
# assuming darwin=clang but must be a better way...
if "Darwin" in platform.platform() :
	common_env[ "LINKFLAGS" ].remove( "-pthread" )

jdb_log_level = ARGUMENTS.get( "ll", 60 )
common_env.Append(CXXFLAGS 		= "-DJDB_LOG_LEVEL=" + str(jdb_log_level) )
common_env.Append(CXXFLAGS 		= "-O3" )

target = common_env.Program( target='bin/pta.app', source=[Glob( "src/*.cpp" ), Glob( "src/*/*.cpp" )] )

Depends( target, root_dict )
# Depends( target, pico_dict )
# Depends( target, JDB_LIB + "/lib/" + JDB_LIB_NAME )
# Depends( target, Glob( JDB_LIB + "/include/jdb/*" ) )

# set as the default target
Default( target )


