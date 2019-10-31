# ngsLD

`ngsLD` is a program to estimate pairwise linkage disequilibrium (LD) taking the uncertainty of genotype's assignation into account. It does so by avoiding genotype calling and using genotype likelihoods or posterior probabilities.

### Citation

`ngsLD` has been accepted in [Bioinformatics](https://doi.org/10.1093/bioinformatics/btz200), so please cite it if you use it in your work:

    Fox EA, Wright AE, Fumagalli M, and Vieira FG
    ngsLD: evaluating linkage disequilibrium using genotype likelihoods
    Bioinformatics (2019) 35(19):3855 - 3856

### Installation

`ngsLD` can be easily installed but has some external dependencies:

* Mandatory:
  * `gcc`: >= 4.9.2 tested on Debian 7.8 (wheezy)
  * `zlib`: v1.2.7 tested on Debian 7.8 (wheezy)
  * `gsl` : v1.15 tested on Debian 7.8 (wheezy)
* Optional (only needed for testing or auxilliary scripts):
  * `md5sum`
  * `Perl` packages: `Getopt::Long`, `Graph::Easy`, `Math::BigFloat`, and `IO::Zlib`
  * `R` packages: `optparse`, `tools`, `ggplot2`, `reshape2`, `plyr`, `gtools`, and `LDheatmap`

To install the entire package just download the source code:

    % git clone https://github.com/fgvieira/ngsLD.git

and run:

    % cd ngsLD
    % make

To run the tests (only if installed through [ngsTools](https://github.com/mfumagalli/ngsTools)):

    % make test

Executables are built into the main directory. If you wish to clean all binaries and intermediate files:

    % make clean

### Usage

    % ./ngsLD [options] --geno glf/in/file --n_ind INT --n_sites INT --out output/file

#### Parameters
* `--geno FILE`: input file with genotypes, genotype likelihoods or genotype posterior probabilities.
* `--probs`: is the input genotype probabilities (likelihoods or posteriors)?
* `--log_scale`: is the input in log-scale?
* `--n_ind INT`: sample size (number of individuals).
* `--n_sites INT`: total number of sites.
* `--pos` FILE: input file with site coordinates (one per line), where the 1st column stands for the chromosome/contig and the 2nd for the position (bp); remaining columns will be ignored but included in output.
* `--max_kb_dist DOUBLE`: maximum distance between SNPs (in Kb) to calculate LD. If set to 0 (zero) will perform all comparisons. [100]
* `--max_snp_dist INT`: maximum distance between SNPs (in number of SNPs) to calculate LD. If set to 0 (zero) will perform all comparisons. [0]
* `--min_maf DOUBLE`: minimum SNP minor allele frequency. [0.001]
* `--ignore_miss_data`: ignore missing genotype data from analyses.
* `--call_geno`: call genotypes before running analyses.
* `--N_thresh DOUBLE`: minimum threshold to consider site; missing data if otherwise (assumes -call_geno).
* `--call_thresh DOUBLE`: minimum threshold to call genotype; left as is if otherwise (assumes -call_geno).
* `--rnd_sample DOUBLE`: proportion of comparisons to randomly sample. [1]
* `--seed INT`: random number generator seed for random sampling (--rnd_sample).
* `--extend_out`: print extended output (see below).
* `--out FILE`: output file name. [stdout]
* `--n_threads INT`: number of threads to use. [1]
* `--verbose INT`: selects verbosity level. [1]

### Input data
As input, `ngsLD` accepts both genotypes, genotype likelihoods (GP) or genotype posterior probabilities (GP). Genotypes must be input as gziped TSV with one row per site and one column per individual ![n_sites.n_ind](http://bit.ly/2Wvvt03) and genotypes coded as [-1, 0, 1, 2].
As for GL and GP, `ngsLD` accepts both gzipd TSV and binary formats, but with 3 columns per individual ![3.n_sites.n_ind](http://bit.ly/2N3jkwc) and, in the case of binary, the GL/GP coded as doubles.

It is advisable that SNPs be called first, since monomorphic sites are not informative and it will greatly speed up computation. If not, these comparisons will show up as `nan` or `inf` in the output.

### Output
`ngsLD` outputs a TSV file with LD results for all pairs of sites for which LD was calculated, where the first two columns are positions of the SNPs, the third column is the distance (in bp) between the SNPs, and the following 4 columns are the various measures of LD calculated (![r^2](http://bit.ly/32Nw9QH) from pearson correlation between expected genotypes, ![D](http://bit.ly/34mm9xV) from EM algorithm, ![D'](http://bit.ly/2oxSd2J) from EM algorithm, and ![r^2](http://bit.ly/32Nw9QH) from EM algorithm). If the option `--extend_out` is used, then an extra 8 columns are printed with number of samples, minor allele frequency (MAF) of both loci, haplotype frequencies for all four haplotypes, and a chi2 (1 d.f.) for the strength of association ([Collins et al., 1999](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC24792/)).

### Possible analyses
##### LD pruning
For some analyses, linked sites are typically pruned since their presence can bias results. You can use the script `scripts/prune_graph.pl` to prune your dataset and get a list of unlinked sites.

    % perl scripts/prune_graph.pl --in_file testLD_2.ld --max_kb_dist 5 --min_weight 0.5 --out testLD_unlinked.id

* `--in_file FILE`: File with input network [STDIN]
* `--max_kb_dist INT`: Maximum distance between nodes (input file 3rd column) to assume they are connected
* `--min_weight FLOAT`: Minimum weight (in `--weight_field`) of an edge to assume nodes are connected
* `--out FILE`: Path to output file [STDOUT]

For more advanced options, please check script help (`perl scripts/prune_graph.pl --help`).

#### LD decay
If you are interested on the rate of LD decay, you can fit a distribution to your data using the script `scripts/fit_LDdecay.R` to fit LD decay models for ![r^2](http://bit.ly/32Nw9QH) ([Hill and Weir, 1988](https://www.ncbi.nlm.nih.gov/pubmed/3376052) and [Remington et al., 2001](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC58755/)) and ![D'](http://bit.ly/2oxSd2J) ([Abecassis et al., 2001](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC1234912/)) over physical (or genetic) distance.

There are two models implemented for ![r^2](http://bit.ly/32Nw9QH) and one for ![D'](http://bit.ly/2oxSd2J) decay. Briefly, the first is derived by adjusting the expected ![r^2](http://bit.ly/32Nw9QH) under a drift-recombination equilibrium for finite samples sizes and low level of mutation ([Hill and Weir, 1988](https://www.ncbi.nlm.nih.gov/pubmed/3376052):

![E_r^2](http://bit.ly/334PA7s)

The second formulation is an extension of the to account for the range of observed ![r^2](http://bit.ly/32Nw9QH) values:

![E_r^2](http://bit.ly/2MZLrfw)

For ![D'](http://bit.ly/2oxSd2J), we fit the expectation derived by [Abecassis et al., 2001](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC1234912/), assuming a recombination rate of ![cm-Mb](http://bit.ly/2JznAB9) and fixing ![D'0_1](http://bit.ly/33dMJJo):

![E_D'](http://bit.ly/2N0RbWw)

For more information, please refer to the online published supplementary data.

    % Rscript --vanilla --slave scripts/fit_LDdecay.R --ld_files ld_files.list --out plot.pdf

* `--ld_files FILE`: file with list of LD files to fit and plot (if ommited, can be read from STDIN)
* `--out`: Name of output plot

For more advanced options, please check script help (`Rscript --vanilla --slave scripts/fit_LDdecay.R --help`). The shape of the decay curve can give valuable insight into the sample's biology (e.g.):
* `Intersect` (high): small Ne (or bottleneck), natural selection (local LD)*, non-random mating (or inbreeding), recent admixture
* `Decay Rate` (high): large Ne (or population expansion)*, high mutation rate*, high recombination rate*, random mating (or no inbreeding)*, not recently admixture
* `Asymptote` (high): population structure*, small sample size*

NOTE: Please keep in mind that these are just general trends, and that it all depends on the biologu/genetics of the sample. Since LD is influenced by many factors, it is usually less straightforward to derive exact predictions from it.

##### LD blocks
To plot LD blocks, we also provide a small script as an example for how it can be easily done in `R` using the `LDheatmap` package (by default, ![r^2](http://mathurl.com/ya2uo8sp.png) is plotted).

    % cat testLD_2.ld | bash ../scripts/LD_blocks.sh chrSIM_21 2000 5000

### Hints
* `ngsLD` performance seems to drop considerable under extremely low coverages (<1x); consider these cases only if you have large sample sizes (>100 individuals).
* For some analyses (e.g. LD decay) consider sampling your data (`--rnd_sample`), since `ngsLD` will be much faster and you probably don't need all comparisons.
* For the LD decay, as a rule-of-thumb, consider using at least 10'000 SNPs; check the confidence interval and, if too wide, increase number of SNPs.

### Thread pool
The thread pool implementation was adapted from Mathias Brossard's and is freely available from:
https://github.com/mbrossard/threadpool
