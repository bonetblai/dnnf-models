/*
 *  Copyright (C) 2006 Universidad Simon Bolivar
 * 
 *  Permission is hereby granted to distribute this software for
 *  non-commercial research purposes, provided that this copyright
 *  notice is included with any such distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 *  SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 *  ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *  
 *  Blai Bonet, bonet@ldc.usb.ve
 *
 */

#include <nnf.h>
#include <satlib.h>
#include <utils.h>

#include <iostream>
#include <fstream>

using namespace std;

void banner(ostream &os) {
    os << "Model enumerator for d-DNNF theories." << endl
       << "Universidad Simon Bolivar, 2006." << endl;
}

void usage(ostream &os) {
    os << "usage: models [--mp] [--literal-counts] [--num <n>] [--verbose] [--write-models] [--write-all-literals] <nnf>" << endl;
}

int main(int argc, char **argv) {
    float i_seconds, l_seconds, seconds;
    bool use_mp = false;
    bool literal_counts = false;
    bool all = false;
    bool output = false;
    bool verbose = false;
    size_t count = 0;
    banner(cout);

    ++argv;
    --argc;
    if( argc == 0 ) {
      print_usage:
        usage(cout);
        exit(-1);
    }

    for( ; argc && ((*argv)[0] == '-'); --argc, ++argv ) {
        if( !strcmp(*argv, "--mp") ) {
            use_mp = true;
        } else if( !strcmp(*argv, "--literal-counts") ) {
            literal_counts = true;
        } else if( !strcmp(*argv, "--num") ) {
            count = atoi(argv[1]);
            ++argv;
            --argc;
        } else if( !strcmp(*argv, "--verbose") ) {
            verbose = true;
        } else if( !strcmp(*argv, "--write-models") ) {
            output = true;
        } else if( !strcmp(*argv, "--write-all-literals") ) {
            all = true;
        } else {
            usage(cout);
            exit(-1);
        }
    }

    if( argc != 1 ) goto print_usage;
    string nnf_file = argv[0];

    // create managers, set start timer and read file
    nnf::Manager *nnf_theory = new nnf::Manager(0, verbose);
    i_seconds = l_seconds = utils::read_time_in_seconds();

    ifstream nnf_is(nnf_file.c_str());
    if( !nnf_is.is_open() ) {
        cout << "main: error opening file '" << nnf_file << "'" << endl;
        exit(-1);
    }

    try {
        cout << "main: reading file '" << nnf_file << "'" << flush;
        satlib::read_nnf_file(nnf_is, *nnf_theory);
    } catch( int e ) {
        cout << endl << "main: error reading file '" << nnf_file << "'" << endl;
        exit(-1);
    }
    seconds = utils::read_time_in_seconds();
    cout << " : " << seconds-l_seconds << " seconds" << endl;
    l_seconds = seconds;

    // print simple stats
    void *lc = 0;
    if( literal_counts ) {
        if( use_mp ) {
            lc = new mp::Int*[1+nnf_theory->num_vars()];
        } else {
            lc = new float[1+nnf_theory->num_vars()];
        }
    }
    cout << "main: #nodes=" << nnf_theory->count_nodes()
         << ", #edges=" << nnf_theory->count_edges();

    if( use_mp )
        cout << ", #models=" << *(nnf_theory->mp_count_models((mp::Int**)lc));
    else
        cout << ", #models=" << nnf_theory->count_models((float*)lc);

    seconds = utils::read_time_in_seconds();
    cout << " : " << seconds-l_seconds << " seconds" << endl;

    if( literal_counts ) {
        cout << "--- count table ---" << endl;
        for( size_t i = 0 ; i < nnf_theory->num_vars(); ++i ) {
            if( use_mp ) {
                if( ((mp::Int**)lc)[i+1] == 0 )
                    cout << "literal " << i+1 << " : n/a" << endl;
                else
                    cout << "literal " << i+1 << " : " << *((mp::Int**)lc)[i+1] << endl;
            } else {
                if( ((float*)lc)[i+1] == -1 )
                    cout << "literal " << i+1 << " : n/a" << endl;
                else
                    cout << "literal " << i+1 << " : " << ((float*)lc)[i+1] << endl;
            }
        }
        cout << "--- count table ---" << endl;
    }

    // enumerate models
    if( output ) {
        cout << "--- models begin ---" << endl;
        nnf_theory->enumerate_models(cout, count, all);
        cout << "---- models end ----" << endl;
    }

    // cleanup: don't needed since finishing...
    //nnf_theory->unregister_use(nnf_theory->root());
    //delete nnf_theory;

    seconds = utils::read_time_in_seconds();
    cout << "main: total time " << seconds-i_seconds << " seconds" << endl;
    return 0;
}

