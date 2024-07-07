void MinimaxStrategy::searchBestMove() {
    // Initialization code
    omp_set_num_threads(48); // Set the number of threads

    #pragma omp parallel
    {
        #pragma omp single
        test = doMinMaxSearch(0, *_board, *_ev, newMList, -15000, 15000, 0, false, SearchStrategy::_maxDepth);
    }
    // Finalization code
}
