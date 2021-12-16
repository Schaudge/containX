#include <iostream>
#include <cassert>
#include <zlib.h>

#include "graph.hpp"
#include "algo.hpp"
#include "ketopt.h"

int main(int argc, char *argv[])
{
  ketopt_t o = KETOPT_INIT;
  float min_ovlp_identity = 0.0; //[0-100]
  int min_ovlp_len = 0;
  int c;
  std::string gfadumpfilename;
  bool printReadStrings = true;
  bool removeAllContainedReads = false;
  algoParams param;
  param.hpc = false;
  param.fuzz = UINT32_MAX; //disable transitive reduction of edges by default
  param.cutoff = 0.8;

  while ((c = ketopt(&o, argc, argv, 1, "l:i:d:D:t:cHm:", 0)) >= 0)
  {
    if (c == 'i') min_ovlp_identity = atof(o.arg);
    else if (c == 'l') min_ovlp_len = atoi(o.arg);
    else if (c == 'd') gfadumpfilename = o.arg, printReadStrings = false;
    else if (c == 'D') gfadumpfilename = o.arg;
    else if (c == 't') param.fuzz = atoi(o.arg);
    else if (c == 'c') removeAllContainedReads = true;
    else if (c == 'H') param.hpc = true;
    else if (c == 'm') param.cutoff = atof(o.arg);
  }

  //print usage
  if (argc <= o.ind + 1) {
    std::cerr << "Usage: containX [options] <input-reads.fq> <in.paf>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -l NUM      min overlap length, default " << min_ovlp_len << "\n";
    std::cerr << "  -i NUM      min overlap percentage identity [0.0-100.0], default " << min_ovlp_identity << "\n";
    std::cerr << "  -m NUM      min fraction of minimizer matches for redundant contained reads, default " << param.cutoff << "\n";
    std::cerr << "  -H          use homopolymer-compressed k-mer\n";
    std::cerr << "  -c          mark all contained reads as redundant and remove\n";
    std::cerr << "  -t NUM      enable transitive reduction using given fuzz, disabled by default\n";
    std::cerr << "  -d FILE     dump graph in gfa format\n";
    std::cerr << "  -D FILE     dump graph in gfa format with sequences\n";
    return 1;
  }

  assert (min_ovlp_identity >= 0.0);
  assert (min_ovlp_identity <= 100.0);

  //set parameters
  param.maxContainmentDegree = 5, param.depth = 5, param.k = 16;
  param.d = 1.0/param.k;
  param.iter = 5;
  param.printParams();

  graphcontainer g;
  ovlgraph_gen (argv[o.ind], argv[o.ind+1], min_ovlp_identity, min_ovlp_len, g);

#ifdef VERBOSE
  printContainmentDegreeDistribution (g, "ContainmentDegree.beforeSimplify.txt");
  printDegreeDistribution (g, "Degree.beforeSimplify.txt");
  printEdgesDOTFormat (g, "edges.beforeSimplify.DOT");
#endif

  ovlgraph_simplify (removeAllContainedReads, g, param);


#ifdef VERBOSE
  printContainmentDegreeDistribution (g, "ContainmentDegree.afterSimplify.txt");
  printDegreeDistribution (g, "Degree.afterSimplify.txt");
  printEdgesDOTFormat (g, "edges.afterSimplify.DOT");
#endif

  if (!gfadumpfilename.empty())
    g.outputGFA (gfadumpfilename, printReadStrings);


  //log complete command given by user
  fprintf(stderr, "INFO, %s(), CMD:", __func__);
  for (int i = 0; i < argc; ++i)
    fprintf(stderr, " %s", argv[i]);
  std::cerr << "\n";

  return 0;
}
