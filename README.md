Query
=====

Context Aware Query Suggestion by Mining Click Through and Session Data
-----------------------------------------------------------------------
The Work is divided into two steps:
* Offline model
  Form a bi-partite graph
  Cluster them into concepts (group of similar queries)
  Create concept sequence suffix tree
* Online model
  Map queries into concept
  Look up the suffix tree and suggest queries
  
  How to run
  ----------
  
  g++ main.cpp
  ./a.out input_file //input file should be your data log
