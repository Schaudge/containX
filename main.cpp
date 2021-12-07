#include <iostream>
#include <cassert>
#include <zlib.h>

#include "graph.hpp"
#include "ketopt.h"

int main(int argc, char *argv[])
{
  ketopt_t o = KETOPT_INIT;
  float min_ovlp_identity = 0.0; //[0-100]
  int min_ovlp_len = 0; 
  int c;
  std::string gfadumpfilename;
  int fuzz = INT32_MAX;
  bool removeContainedReads = false;

  while ((c = ketopt(&o, argc, argv, 1, "l:i:d:t:c", 0)) >= 0)
  {
    if (c == 'i') min_ovlp_identity = atof(o.arg);
    else if (c == 'l') min_ovlp_len = atoi(o.arg);
    else if (c == 'd') gfadumpfilename = o.arg;
    else if (c == 't') fuzz = atoi(o.arg);
    else if (c == 'c') removeContainedReads = true;
  }

  //print usage
  if (argc <= o.ind + 1) {
    std::cerr << "Usage: containX [options] <input-reads.fq> <in.paf>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -l NUM      min overlap length, default " << min_ovlp_len << "\n";
    std::cerr << "  -i NUM      min overlap percentage identity [0.0-100.0], default " << min_ovlp_identity << "\n";
    std::cerr << "  -c          mark all contained reads as redundant and remove\n";
    std::cerr << "  -t NUM      enable transitive reduction using given fuzz, disabled by default\n";
    std::cerr << "  -d FILE     dump graph in gfa format\n";
    return 1;
  }

  assert (min_ovlp_identity >= 0.0);
  assert (min_ovlp_identity <= 100.0);
  assert (fuzz >= 0);

  graphcontainer g; 
  ovlgraph_gen (argv[o.ind], argv[o.ind+1], min_ovlp_identity, min_ovlp_len, fuzz, removeContainedReads, g);

  if (!gfadumpfilename.empty())
    g.outputGFA (gfadumpfilename);

  //log complete command given by user
  fprintf(stderr, "INFO, %s(), CMD:", __func__);
  for (int i = 0; i < argc; ++i)
    fprintf(stderr, " %s", argv[i]);
  std::cerr << "\n";

  return 0;
}
