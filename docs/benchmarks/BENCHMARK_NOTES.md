Configuration Parser --- Benchmark Notes
Project: C++ Configuration Parser
Benchmark target: config_parser_benchmark
Builds compared: Default and Release
Status: Both benchmark runs completed successfully

1. Why Benchmark This Project?
A benchmark measures how a program performs under a defined workload.

For this Configuration Parser, we want to understand:
- How long it takes to load configuration files of different sizes.
- How quickly the parser retrieves values by section and key.
- How performance changes as the number of entries increases.
- How the Default build compares with the optimized Release build.
A benchmark is useful only when its workload and measurement method are
documented. Results should be interpreted in the context of the machine,
compiler, build settings, and benchmark implementation.
2. What This Benchmark Measures
The benchmark measures two main operations.
2.1 Configuration loading
The benchmark generates a configuration file and measures the time taken
by the parser to load it.

The input sizes are:
- 100 entries
- 1,000 entries
- 10,000 entries
- 100,000 entries
Important: File generation is measured separately. The reported
loading time is the parser's loading measurement, not the time required
to generate the input file.
2.2 Key lookup
The benchmark performs 100,000 lookup operations per measurement run.

Lookup keys are prepared before the timed lookup measurement. This
avoids including per-lookup key construction in the timed section.

The measured getValue() call returns a std::string by value, so the
lookup measurement includes the cost associated with producing that
returned string.
3. Benchmark Methodology
The refined benchmark uses:

  Setting                                                           Value
  Warm-up runs                                                          2
Measurement runs                                                      7
Lookup operations per measurement                               100,000
Entry counts                                100, 1,000, 10,000, 100,000
Reported statistics                   Average, median, minimum, maximum
3.1 Warm-up runs
Warm-up runs execute the workload before the measured repetitions.

Their purpose is to reduce the influence of first-run effects, such as
initial runtime or system state effects. Warm-up does not eliminate all
measurement noise.
3.2 Measurement repetitions
The benchmark repeats each measured workload seven times.

Repeated measurements help show variation rather than relying on one
timing sample.
3.3 Average
The arithmetic mean of the seven measurements:
\[
`\frac{\text{sum of measurements}}{\text{number of measurements}}`{=tex}
\]
The average gives an overall summary but can be affected by unusually
high or low samples.
3.4 Median
The middle value after sorting the measurements.

The median is less affected by an unusually slow or fast sample than the
average.
3.5 Minimum and maximum
- Minimum: Fastest observed measurement.
- Maximum: Slowest observed measurement.
These values show the observed spread, not guaranteed performance
limits.
3.6 Lookup throughput
Throughput expresses how many lookup operations are completed per
second.

The benchmark reports throughput based on the lookup workload. A useful
approximation is:
\[
`\frac{\text{lookup operations}}{\text{lookup time in seconds}}`{=tex}
\]
Higher throughput means more lookup operations completed per second
under this benchmark's conditions.
4. Default Build Results
These are the recorded results from the refined Default benchmark run.
        Entries   Loading average    Lookup average Lookup throughput
                             (ms)              (ms)           (ops/s)

            100          0.233671           38.2755         2,612,640

          1,000           1.67691           37.8466         2,642,250

         10,000           16.0342           40.7188         2,455,870

        100,000           187.715           57.2508         1,746,700

The Default benchmark completed successfully.
5. Release Build Results
These are the recorded results from the refined Release benchmark run.
        Entries   Loading average    Lookup average Lookup throughput
                             (ms)              (ms)           (ops/s)

            100            0.0815            2.1735        46,008,700

          1,000          0.366643            3.0157        33,159,800

         10,000           2.73677           3.72656        26,834,400

        100,000           36.3182           10.4238         9,593,430

The Release benchmark completed successfully. The benchmark also printed
verification checksums for each workload size.
6. Default vs Release Comparison
The following table compares the recorded average loading and lookup
times.
Entries   Loading speedup (approx.)   Lookup speedup (approx.)

    100                       2.87×                     17.60×
  1,000                       4.57×                     12.55×
 10,000                       5.86×                     10.93×
100,000                       5.17×                      5.49×
Speedup is calculated as:
\[
`\frac{\text{Default average time}}{\text{Release average time}}`{=tex}
\]
A value greater than 1 means the Release measurement took less time.
6.1 Interpretation
- The Release build was faster than the Default build for both
  measured operations at all tested input sizes.
- The measured loading time increased as the number of entries
  increased.
- Lookup time also increased for the largest tested configuration.
- The speedup varied with workload size; it should not be summarized
  as one universal multiplier.
- These results describe this benchmark run and environment, not a
  guarantee for every machine or workload.
7. Understanding the CSV Reports
The benchmark writes its report to:
benchmark_results.csv
After preserving the two runs, the project keeps separate files:
docs/
└── benchmark/
    ├── BENCHMARK_NOTES.md
    └── results/
        ├── benchmark_default.csv
        └── benchmark_release.csv
7.1 Why keep CSV files?
CSV stands for Comma-Separated Values. It stores tabular data as
text, with values separated by commas.

The CSV files are the raw benchmark reports. They can be opened in
spreadsheet software or processed by scripts.

Keep the generated CSV files unchanged as evidence of the recorded runs.
The analysis and explanation belong in this Markdown document.
8. Benchmark Limitations
The benchmark is useful for comparing these two builds under the same
project workload, but it has limitations.
- Results depend on the CPU, memory, operating system, compiler,
  compiler version, and system load.
- Seven repetitions provide a small sample; they do not characterize
  every possible system condition.
- Warm-up runs reduce some first-run effects but do not remove all
  noise.
- Lookup keys are prepared before the timed lookup loop, so
  key-construction cost is not part of the lookup measurement.
- getValue() returns a string by value, so the measured lookup
  includes the return-value behavior of the current API.
- The benchmark measures this parser's workload; it is not a
  comparison against other parser libraries.
- File generation time is reported separately and should not be
  confused with parser loading time.
- No statistical confidence interval is calculated.
For a stronger future benchmark, consider more repetitions, controlled
system conditions, additional workload patterns, and comparison with a
known baseline implementation.
9. Important C++ and Performance Concepts
  Concept                             Meaning in this project
  Benchmark                           Measures execution time for a
defined workload

  Warm-up                             Runs workload before collecting
measurements

  Repetition                          Repeats measurements to observe
variation

  Average                             Arithmetic mean of measured times

  Median                              Middle measurement after sorting

  Minimum / Maximum                   Fastest and slowest observed
samples

  Throughput                          Number of operations completed per
second

  Release build                       Build configured with Release
optimization settings

  Lookup complexity                   Expected average-case hash-map
lookup is O(1), with O(n) worst
case
Measurement noise                   Timing variation caused by system
and runtime conditions
9.1 Big-O versus benchmark timing
Big-O complexity describes how an algorithm's work grows as input size
grows. A benchmark measures actual elapsed time on a particular machine
and workload.

They answer different questions:
- Big-O: How does the operation scale in theory?
- Benchmark: How long did this implementation take under the
  measured conditions?
Do not use one benchmark result to prove an algorithm's Big-O
complexity.
10. Interview Revision Questions
Q1. What is benchmarking?
Benchmarking is measuring the performance of a program or operation
under a defined workload.
Q2. Why do we use warm-up runs?
Warm-up runs execute the workload before measurement to reduce some
first-run effects. They do not remove all measurement noise.
Q3. What is the difference between average and median?
Average is the arithmetic mean. Median is the middle value after
sorting. Median is less sensitive to unusually high or low samples.
Q4. What is throughput?
Throughput is the number of operations completed per unit of time. In
this benchmark, it is reported as lookup operations per second.
Q5. Why compare Default and Release builds?
Release builds commonly enable compiler optimizations. Comparing the
same workload across builds helps observe the effect of the build
configuration in this environment.
Q6. Does a faster benchmark prove better algorithmic complexity?
No. Benchmark timing measures actual elapsed time for a particular
workload. Big-O describes how work scales as input size grows.
Q7. Why prepare lookup keys before timing?
It keeps key construction outside the timed lookup section, so the
measurement focuses more directly on the parser's lookup operation.
Q8. Why does getValue() returning by value matter?
Returning by value means the result string is produced for the caller.
The benchmark therefore measures the current API's lookup and return
behavior, not only the internal hash-table search.
Q9. Why preserve CSV files?
They provide the recorded tabular results for later inspection,
spreadsheet analysis, and comparison.
Q10. Can these benchmark numbers be expected on every computer?
No. Results depend on hardware, software, compiler, system load, and
measurement conditions.
11. Reproduction Commands
Run commands from the project root.
Default build benchmark
cmake -S . -B build -G "MinGW Makefiles" -DCONFIG_PARSER_BUILD_BENCHMARKS=ON
cmake --build build
.\build\config_parser_benchmark.exe
Release build benchmark
cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCONFIG_PARSER_BUILD_BENCHMARKS=ON -DCONFIG_PARSER_BUILD_TESTS=OFF -DCONFIG_PARSER_BUILD_EXAMPLES=OFF
cmake --build build-release
.\build-release\config_parser_benchmark.exe
The benchmark writes benchmark_results.csv in the current working
directory. Preserve or rename the output after each run so that the
Default and Release reports are not overwritten.
12. Documentation Maintenance
When benchmark code or methodology changes:
1. Update the methodology section.
2. Re-run both build configurations under documented conditions.
3. Preserve the generated CSV reports separately.
4. Replace result tables only with measurements from actual runs.
5. Explain any changes to workload size, timing, or lookup behavior.
6. Keep limitations visible.
Benchmark rule: Record measured facts separately from
interpretation. Never invent or manually adjust performance
measurements.