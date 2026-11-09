Configuration Parser --- Benchmark Results Report
Project: C++ Configuration Parser
Report type: Recorded benchmark results
Build configurations: Default and Release
Workload: 100, 1,000, 10,000, and 100,000 configuration entries
Lookup workload: 100,000 operations per measurement run
Measurement repetitions: 7
Warm-up runs: 2

1. Purpose
This document records the measured performance results of the
Configuration Parser benchmark.

It is intended as a results report and evidence summary. The benchmark
methodology, metric definitions, interpretation, limitations, and
learning notes are documented separately in BENCHMARK_NOTES.md.

The original CSV reports should be preserved alongside this document:
results/
├── benchmark_default.csv
└── benchmark_release.csv
2. Default Build Results
The following values were recorded from the refined Default benchmark
run.
  Configuration   Loading average    Lookup average Lookup throughput
        entries              (ms)              (ms)         (ops/sec)

            100          0.233671           38.2755         2,612,640

          1,000           1.67691           37.8466         2,642,250

         10,000           16.0342           40.7188         2,455,870

        100,000           187.715           57.2508         1,746,700

The available Default benchmark transcript records loading and lookup
measurements. File-generation times are not included in this table.
3. Release Build Results
The following values were recorded from the Release benchmark run.
Configuration           File        Loading Lookup average         Lookup
      entries     generation   average (ms)           (ms)     throughput
                        (ms)                                    (ops/sec)

          100         0.5338         0.0815         2.1735     46,008,700

        1,000         0.9455       0.366643         3.0157     33,159,800

       10,000         1.8672        2.73677        3.72656     26,834,400

      100,000        15.8432        36.3182        10.4238      9,593,430

4. Default vs Release --- Average Time Comparison
 Entries    Default    Release    Loading    Default    Release     Lookup
            loading    loading    speedup     lookup     lookup    speedup
               (ms)       (ms)                  (ms)       (ms) 

     100   0.233671     0.0815      2.87×    38.2755     2.1735     17.60×

   1,000    1.67691   0.366643      4.57×    37.8466     3.0157     12.55×

  10,000    16.0342    2.73677      5.86×    40.7188    3.72656     10.93×

 100,000    187.715    36.3182      5.17×    57.2508    10.4238      5.49×

Speedup is calculated as:
Default average time / Release average time
A speedup greater than 1 means the Release measurement took less time in
this run.
5. Release Benchmark Verification Output
The Release benchmark printed the following verification checksums:
Entries   Verification checksum

    100               6,210,000
  1,000               7,101,000
 10,000               8,000,100
100,000               8,900,010
The benchmark ended with:
Benchmark completed successfully.
CSV report saved to: benchmark_results.csv
These checksums are recorded as the benchmark program's output. They are
not independent correctness proofs for every parser behavior; the
automated test suite provides separate functional verification.
6. Summary
- The Default benchmark completed successfully.
- The Release benchmark completed successfully.
- The Release build recorded lower average loading and lookup times
  for all four tested input sizes.
- The benchmark tested configuration sizes from 100 to 100,000
  entries.
- Each lookup measurement used 100,000 operations.
- The Release benchmark produced verification checksums and a CSV
  report.
- The project's automated test run separately recorded 58 passing
  tests and 0 failures.
Benchmark measurements depend on the machine, compiler, build
configuration, and system load. These values should be presented as
results from the recorded environment, not as universal performance
guarantees.
7. Evidence Files
Keep the generated CSV files unchanged:
docs/
└── benchmark/
    ├── BENCHMARK_NOTES.md
    └── results/
        ├── BENCHMARK_RESULTS.md
        ├── benchmark_default.csv
        └── benchmark_release.csv
The CSV files are the raw tabular reports. This Markdown file is a
readable summary of the recorded results.

Evidence rule: If a value in this report does not match the
corresponding CSV, the original CSV is the source to inspect. Correct
this report from the actual CSV rather than guessing or manually
adjusting measurements.