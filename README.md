# RELIC_Silu

## The whole workflow is in file "compare_inferred_graph.ipynb"
1. data generation
2. lineage inference 
    * profiling
    * pre-clustering
    * preserving-ops
        * pairwise (*cell, col, relational metric*)
        * intra-cluster mst + inter-cluster mst
3. compare the inferred graph with real lineage graph
    * number of common edges
    * TODO: parital order?
