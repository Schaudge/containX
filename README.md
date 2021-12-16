# containX
```
Usage: containX [options] <input-reads.fq> <in.paf>
Options:
  -l NUM      min overlap length, default 0
  -i NUM      min overlap percentage identity [0.0-100.0], default 0
  -m NUM      min fraction of minimizer matches for redundant contained reads, default 0.8
  -H          use homopolymer-compressed k-mer
  -c          mark all contained reads as redundant and remove
  -t NUM      enable transitive reduction using given fuzz, disabled by default
  -d FILE     dump graph in gfa format
  -D FILE     dump graph in gfa format with sequences
```
