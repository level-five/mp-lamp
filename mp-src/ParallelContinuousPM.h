/*
 * ParallelSearch.h
 *
 *  Created on: Apr 17, 2017
 *      Author: yuu
 */

#ifndef MP_SRC_PARALLELCONTINUOUSPM_H_
#define MP_SRC_PARALLELCONTINUOUSPM_H_

#include "MPI_Data.h"

#include "ParallelDFS.h"

namespace lamp_search {

/**
 * TODO: This class is intended to be a parent class for pattern mining classes.
 *       For now it takes responsibility to run
 *       - GetMinimalSupport
 *       - GetTestablePatterns
 *       - GetSignificantPatterns
 *       These functions should be factored out as subclasses.
 */
    class ParallelContinuousPM : public ParallelDFS {

        typedef double Feature;
        typedef int Class;

    public:
        ParallelContinuousPM(ContinuousPatternMiningData *bpm_data,
                             MPI_Data &mpi_data, TreeSearchData *treesearch_data,
                             double alpha, Log *log, Timer *timer, std::ostream &ofs);

        virtual ~ParallelContinuousPM();

//	virtual void Search();
        void GetMinimalSupport();

        void GetDiscretizedMinimalSupport(double freqRatio);

//	void PreProcessRootNode(GetMinSupData* getminsup_data);
        void GetTestablePatterns(GetTestableData *gettestable_data);

        void GetSignificantPatterns(
                GetContSignificantData *getsignificant_data,
                int topk = 0);

        void GetTopKPvalue(int k, double freqRatio);

        void SearchSignificantPatterns(double pvalue);

        double GetThreFreq() const {
            return thre_freq_;
        }

        double GetThrePmin() const {
            return thre_pmin_;
        }

        void AddFrequence(double frequence) {
            frequencies_.push_back(frequence);
        }

        int NumberOfTestablePatterns() const;

        void CallbackForProbe();

    private:
        /*
         * Data structure
         */
        MPI_Data &mpi_data;
        GetMinSupData *getminsup_data;
        GetTestableData *gettestable_data;
        GetContSignificantData *getsignificant_data;

        // TODO: How can we hide the dependency on those low level structures?
        //       Let's try to understand the semantics of how these methods are used, and factor out.
        /*
         * Domain graph
         */
        ContDatabase *d_;

        // TODO: sup_buf_ is only used in ProcessNode and PreProcessRootNode!
        std::vector<Feature> sup_buf_;
        std::vector<Feature> child_sup_buf_;

        // TODO: TOP-K data members. Pack them if necessary.
        int topk; // top-k

//	Log* log_;
//	Timer* timer_;

        /**
         * Methods used for ALL searches: Maybe they should be overrided by other methods.
         *
         */
        virtual void ProbeExecute(MPI_Status *probe_status, int probe_src, int probe_tag);

        void ProcAfterProbe(); // DOMAINDEPENDENT
        void Check(); // DOMAINDEPENDENT
        std::vector<int> GetChildren(std::vector<int> items); // DOMAINDEPENDENT

        /**
         * Methods for Maintaining threshold value
         */
        // 0: count, 1: time warp flag, 2: empty flag, 3--: data
        void SendMinPValueRequest();

        void RecvMinPValueRequest(int src);

        // 0: count, 1: time warp flag, 2: empty flag, 3--: data
        void SendMinPValueReply();

        void RecvMinPValueReply(int src, MPI_Status *probe_status);

        void CalculateThreshold();

        void SendNewSigLevel(double sig_level);

        void RecvNewSigLevel(int src);

        std::vector<double> frequencies_;
        std::vector<double> topKFrequencies;

        double alpha_;
        double thre_freq_; // Threshold for itemset-set C.
        double thre_pmin_; // Threshold for itemset-set T (testable pattern)

//	double thre_pvalue_; // Threshold of current Top-K: the largest pvalue to be in Top-K.

//	std::vector<std::pair<double, double>> freq_pmin; // only for rank-0.
        bool freq_received;
//	int prev_freq_pmin_size_; // only for rank-0.
        // TODO: These functions should be factored in Get
        /**
         * Methods For GetSignificant
         */
        void SendResultRequest();

        void RecvResultRequest(int src);

        void SendResultReply();

        void RecvResultReply(int src, MPI_Status status);

//	bool AccumCountReady(MPI_Data& mpi_data) const;
        void ExtractSignificantSet();

        void PrintItemset(int *itembuf, std::vector<Feature> freqs);

// insert pointer into significant_map_ (do not sort the stack itself)
//	void SortSignificantSets();

        /**
         * Linear Space Continuous Pattern Mining
         *
         */
        std::vector<std::pair<double, double>> InitializeThresholdTable(double ratio, int size, double alpha);

        std::vector<std::pair<double, double>> InitializePvalueTable(double ratio, int size, double alpha);

        // 0: count, 1: time warp flag, 2: empty flag, 3--: data
        void SendDTDAccumRequest();

        void RecvDTDAccumRequest(int src);

        // 0: count, 1: time warp flag, 2: empty flag, 3--: data
        void SendDTDAccumReply();

        void RecvDTDAccumReply(int src);


        void CheckCSThreshold();

        bool ExceedCsThr() const;

        int NextLambdaThr() const;


        void SendLambda(int lambda);

        void RecvLambda(int src);

    public:
        void IncCsAccum(int sup_num);
        void UpdateGetTestableData(int* ppc_ext_buf, double freq);
        bool UpdateSupBuf(const std::vector<int>& buf);
        void UpdateChildSupBuf(int new_item);
        Feature GetFreqFromDatabase();
        int GetDiscretizedFrequency(double freq) const;
        const std::vector<std::pair<double, double>>& GetThresholds()  {
            return thresholds;
        };



            std::vector<std::pair<double, double>> thresholds;
//	std::vector<int> count;

        /**
         * Utils
         *
         */
        void CheckInit();

        void CheckInitTestable();

        /**
         * Statistics
         */
        int expand_num_;
        int closed_set_num_;
    };

} /* namespace lamp_search */

#endif /* MP_SRC_PARALLELPATTERNMINING_H_ */
