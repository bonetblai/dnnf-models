#include <nnf.h>
#include <satlib.h>
#include <utils.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <iostream>
#include <fstream>
#include <functional>
#include <set>

using namespace std;

int verbosity_level = 0;
vector<int> fvars;

void banner(ostream &os) {
    os << "Compute best model from NNF." << endl
       << "Universidad Simon Bolivar, 2006." << endl;
}

void usage(ostream &os) {
    os << "usage: bestmodel [-f <file>] [-c <file>] <nnf>" << endl;
}

void
read_simple_file(istream &is, vector<int> &contents) {
    size_t n;
    is >> n;
    contents.clear();
    contents.reserve(n);
    for( size_t i = 0; i < n; ++i ) {
        int entry;
        is >> entry;
        contents.push_back(entry);
    }
}

void read_pairs_file(istream &is, vector<pair<int, int> > &contents) {
    size_t n;
    is >> n;
    contents.clear();
    contents.reserve(n);
    for( size_t i = 0; i < n; ++i ) {
        int p, q;
        is >> p >> q;
        contents.push_back(make_pair(p, q));
    }
}

int main(int argc, const char **argv) {
    float i_seconds, l_seconds, seconds;
    const char *forget_file = 0, *cost_file = 0;
    banner(cout);

    ++argv;
    --argc;
    if( argc == 0 ) {
      print_usage:
        usage(cout);
        exit(-1);
    }

    for( ; argc && ((*argv)[0] == '-'); --argc, ++argv ) {
        switch( (*argv)[1] ) {
          case 'f':
              forget_file = *++argv;
              --argc;
              break;
          case 'c':
              cost_file = *++argv;
              --argc;
              break;
          case '?':
            default:
              goto print_usage;
        }
    }

    if( argc != 1 ) goto print_usage;
    string nnf_file = argv[0];

    // read files
    vector<int> forgets;
    if( forget_file != 0 ) {
        ifstream is(forget_file);
        read_simple_file(is, forgets);
    }

    vector<pair<int,int> > costs;
    if( cost_file != 0 ) {
        ifstream is(cost_file);
        read_pairs_file(is, costs);
    }

    // create managers and set start time
    nnf::Manager *nnf_theory = new nnf::Manager(); //0,true);
    i_seconds = l_seconds = utils::read_time_in_seconds();

    // read nnfs from files
    ifstream nnf_is(nnf_file.c_str());
    if( !nnf_is.is_open() ) {
        cout << "main: error opening file '" << nnf_file << "'" << endl;
        exit(-1);
    }

    try {
        cout << "main: reading file '" << nnf_file << "'" << flush;
        satlib::read_nnf_file(nnf_is, *nnf_theory);
    } catch( int e ) {
        cout << endl << "main: error reading nnf file '" << nnf_file << "'" << endl;
        exit(-1);
    }
    seconds = utils::read_time_in_seconds();
    cout << " : " << seconds-l_seconds << " seconds" << endl;
    l_seconds = seconds;

    cout << "main: #nodes=" << nnf_theory->count_nodes()
         << ", #edges=" << nnf_theory->count_edges();
    seconds = utils::read_time_in_seconds();
    cout << " : " << seconds-l_seconds << " seconds" << endl;

    //cout << "#vars=" << nnf_theory->num_vars() << endl;
    int *cost_vector = new int[2*nnf_theory->num_vars()+2];
    memset(cost_vector, 0, 2*(1+nnf_theory->num_vars())*sizeof(int));
    for( int i = 0, isz = costs.size(); i < isz; ++i ) {
        int lit = costs[i].first;
        lit = lit>= 0 ? 2*lit : -2*lit+1;
        cost_vector[lit] = costs[i].second;
        //cout << "cost(" << lit << ")=" << cost_vector[lit] << endl;
    }

    nnf::Model min_model;
    int cost = nnf_theory->min_cost(cost_vector);
    nnf_theory->min_model(min_model);
    for( int i = 0, isz = forgets.size(); i < isz; ++i ) {
        int var = forgets[i]<<1;
        min_model.erase(var);
        min_model.erase(var+1);
    }
    cout << cost << " " << min_model << endl;

    delete[] cost_vector;

    // cleanup: don't needed since finishing...
    //nnf_theory->unregister_use(nnf_theory->root());
    //delete nnf_theory;

    // total time
    //seconds = utils::read_time_in_seconds();
    //cout << "main: total time " << seconds-i_seconds << " seconds" << endl;

    return 0;
}

