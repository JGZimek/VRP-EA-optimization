# VRP-EA-optimization
Repository dedicated to solving the Vehicle Routing Problem (VRP) using an Evolutionary Algorithm (EA). Includes heuristic-based optimization approaches and implementation details for efficient route planning.

## Output Files

Running the program now saves results to the `output` directory:

* `output/results.csv` &ndash; contains a CSV table of run number and best cost, followed by the average and best cost across all runs.
* `output/run_<n>_routes.txt` &ndash; for each run `n`, lists the routes of all vehicles.

These files can be parsed by external scripts (for example, in Python) to generate graphs or further analyses.

## Plotting Results

The repository provides a helper script in `scripts/plot_results.py` which reads
CSV results from the `output` directory (and its subdirectories) and generates
summary tables along with plots showing how different parameters influence the
solution quality. Usage example:

```bash
python3 scripts/plot_results.py output --output plots
```

The script automatically saves summary CSV files and PNG charts inside the
specified `plots` directory. It also generates a text file with a brief
comment highlighting which value of each parameter achieved the best average
cost. These files can be directly included in the report sections analysing
the effect of generations, population size, selection method, and mutation
or crossover probabilities.
