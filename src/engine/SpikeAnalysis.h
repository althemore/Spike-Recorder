#ifndef BACKYARDBRAINS_SPIKEANALYSIS_H
#define BACKYARDBRAINS_SPIKEANALYSIS_H

#include <vector>
#include <utility>
#include <stdint.h>

namespace BackyardBrains {

class SpikeAnalysis {
public:
	// computes histogram for values in [-maxtime-binsize/2, maxtime+binsize/2]
	//
	// maxsamples: maximal signal timeshift in samples
	// binsize: binsize of histogram
	static void crosscorrelation(std::vector<int> &histogram,
			const std::vector<std::pair<int64_t, int16_t> > &train1,
			const std::vector<std::pair<int64_t, int16_t> > &train2,
			int maxsamples,
			int binsize);

	// like crosscorrelation except only [-binsize/2, maxtime+binsize/2]
	static void autocorrelation(std::vector<int> &histogram,
			const std::vector<std::pair<int64_t, int16_t> > &train,
			int maxsamples,
			int binsize);

	// generates logarithmic bin partition for Interspike Interval Analysis.
	// always between 10^-3 and 10^1 s
	static void isipartition(std::vector<double> binxs, int nbin);

	// Inter Spike Interval Analysis
	// 
	// hist: output
	// train: train to analyse
	// binxs: bin partition to use (use isipartition() to generate)
	// nbin: number of bins

	static void isi(std::vector<int> histogram,
			const std::vector<std::pair<int64_t, int16_t> > &train,
			const std::vector<double> &binxs,
			int nbin, int samplerate);
};

}

#endif
