## TODO

### Bugfixes (this branch)
~~- Remove touch-point contiguity~~
~~- Fix failure to use census adjusted data~~

### Documentation Pass (this branch)
- Remove unused code
- Document as many State/DIstrict/Precinct functions as possible
- Split/organize rebalancing algorithms (swap rebalance and give/take rebalance)
- Remove algorithm dependent on dataframe (all handled by runner)

### Improvements before new algos (other branch)
- Command line args overhaul!
    - Multi-state option
    - Benchmark option (draw x maps of one state and average times elapsed)
    - Custom option (gets passed to draw and interpreted by individual algorithms)
    - Verbose (many prints only print with verbose)
    - Adjustable max deviation
- Better README
- More states