
#include "config_parser/config.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

using namespace config_parser;

namespace
{
    using Clock = std::chrono::steady_clock;

    constexpr int WARMUP_RUNS = 2;
    constexpr int MEASUREMENT_RUNS = 7;
    constexpr int LOOKUP_OPERATIONS = 100000;

    const std::string CONFIG_FILE = "benchmark_config.ini";
    const std::string CSV_FILE = "benchmark_results.csv";

    struct BenchmarkStats
    {
        double averageMs = 0.0;
        double medianMs = 0.0;
        double minimumMs = 0.0;
        double maximumMs = 0.0;
    };

    struct BenchmarkResult
    {
        int entries = 0;
        double fileGenerationMs = 0.0;
        BenchmarkStats loading;
        BenchmarkStats lookup;
        double throughput = 0.0;
        std::uint64_t checksum = 0;
    };

    double elapsedMilliseconds(
        Clock::time_point start,
        Clock::time_point end)
    {
        return std::chrono::duration<double, std::milli>(
            end - start
        ).count();
    }

    BenchmarkStats calculateStats(std::vector<double> values)
    {
        if (values.empty())
        {
            throw std::runtime_error(
                "Cannot calculate statistics from empty measurements."
            );
        }

        BenchmarkStats stats;

        stats.averageMs =
            std::accumulate(values.begin(), values.end(), 0.0)
            / static_cast<double>(values.size());

        std::sort(values.begin(), values.end());

        stats.minimumMs = values.front();
        stats.maximumMs = values.back();

        const std::size_t middle = values.size() / 2;

        if (values.size() % 2 == 0)
        {
            stats.medianMs =
                (values[middle - 1] + values[middle]) / 2.0;
        }
        else
        {
            stats.medianMs = values[middle];
        }

        return stats;
    }

    double generateConfigurationFile(int numberOfEntries)
    {
        const auto start = Clock::now();

        std::ofstream file(CONFIG_FILE, std::ios::binary);

        if (!file)
        {
            throw std::runtime_error(
                "Unable to create benchmark configuration file."
            );
        }

        file << "[benchmark]\n";

        for (int i = 0; i < numberOfEntries; ++i)
        {
            file << "key" << i
                 << " = value" << i << '\n';
        }

        file.close();

        if (!file)
        {
            throw std::runtime_error(
                "Failed while writing benchmark configuration file."
            );
        }

        const auto end = Clock::now();

        return elapsedMilliseconds(start, end);
    }

    void verifyConfiguration(
        const Config& config,
        int numberOfEntries)
    {
        if (config.getValue("benchmark", "key0") != "value0")
        {
            throw std::runtime_error(
                "First configuration entry verification failed."
            );
        }

        const std::string lastKey =
            "key" + std::to_string(numberOfEntries - 1);

        const std::string expectedValue =
            "value" + std::to_string(numberOfEntries - 1);

        if (config.getValue("benchmark", lastKey) != expectedValue)
        {
            throw std::runtime_error(
                "Last configuration entry verification failed."
            );
        }
    }

    BenchmarkStats benchmarkLoading(int numberOfEntries)
    {
        std::vector<double> measurements;

        measurements.reserve(MEASUREMENT_RUNS);

        // Warm-up runs
        for (int i = 0; i < WARMUP_RUNS; ++i)
        {
            Config config;
            config.loadFromFile(CONFIG_FILE);

            verifyConfiguration(config, numberOfEntries);
        }

        // Actual measurements
        for (int i = 0; i < MEASUREMENT_RUNS; ++i)
        {
            Config config;

            const auto start = Clock::now();

            config.loadFromFile(CONFIG_FILE);

            const auto end = Clock::now();

            measurements.push_back(
                elapsedMilliseconds(start, end)
            );

            // Verification is outside the timed region.
            verifyConfiguration(config, numberOfEntries);
        }

        return calculateStats(std::move(measurements));
    }

    std::vector<std::string> generateLookupKeys(int numberOfEntries)
    {
        std::vector<std::string> keys;

        keys.reserve(LOOKUP_OPERATIONS);

        // Pre-generate keys outside the lookup measurement.
        for (int i = 0; i < LOOKUP_OPERATIONS; ++i)
        {
            const int index = i % numberOfEntries;

            keys.push_back(
                "key" + std::to_string(index)
            );
        }

        return keys;
    }

    BenchmarkStats benchmarkLookup(
        const Config& config,
        const std::vector<std::string>& lookupKeys,
        std::uint64_t& finalChecksum)
    {
        std::vector<double> measurements;

        measurements.reserve(MEASUREMENT_RUNS);

        finalChecksum = 0;

        // Warm-up runs
        for (int run = 0; run < WARMUP_RUNS; ++run)
        {
            std::uint64_t checksum = 0;

            for (const std::string& key : lookupKeys)
            {
                const std::string value =
                    config.getValue("benchmark", key);

                checksum += value.size();
            }

            finalChecksum += checksum;
        }

        // Actual measurements
        for (int run = 0; run < MEASUREMENT_RUNS; ++run)
        {
            std::uint64_t checksum = 0;

            const auto start = Clock::now();

            for (const std::string& key : lookupKeys)
            {
                const std::string value =
                    config.getValue("benchmark", key);

                checksum += value.size();
            }

            const auto end = Clock::now();

            measurements.push_back(
                elapsedMilliseconds(start, end)
            );

            // Keep results observable outside timed region.
            finalChecksum += checksum;
        }

        if (finalChecksum == 0)
        {
            throw std::runtime_error(
                "Lookup benchmark verification failed."
            );
        }

        return calculateStats(std::move(measurements));
    }

    BenchmarkResult runBenchmark(int numberOfEntries)
    {
        BenchmarkResult result;

        result.entries = numberOfEntries;

        std::cout << "\nPreparing "
                  << numberOfEntries
                  << " configuration entries...\n";

        result.fileGenerationMs =
            generateConfigurationFile(numberOfEntries);

        std::cout << "Running loading benchmark...\n";

        result.loading = benchmarkLoading(numberOfEntries);

        Config config;
        config.loadFromFile(CONFIG_FILE);

        verifyConfiguration(config, numberOfEntries);

        std::cout << "Preparing lookup keys...\n";

        const std::vector<std::string> lookupKeys =
            generateLookupKeys(numberOfEntries);

        std::cout << "Running lookup benchmark...\n";

        result.lookup = benchmarkLookup(
            config,
            lookupKeys,
            result.checksum
        );

        result.throughput =
            static_cast<double>(LOOKUP_OPERATIONS)
            / (result.lookup.averageMs / 1000.0);

        return result;
    }

    void printStats(
        const std::string& label,
        const BenchmarkStats& stats)
    {
        std::cout << "\n" << label << '\n';

        std::cout << "  Average : "
                  << stats.averageMs << " ms\n";

        std::cout << "  Median  : "
                  << stats.medianMs << " ms\n";

        std::cout << "  Minimum : "
                  << stats.minimumMs << " ms\n";

        std::cout << "  Maximum : "
                  << stats.maximumMs << " ms\n";
    }

    void printResult(const BenchmarkResult& result)
    {
        std::cout << "\n========================================\n";

        std::cout << "Configuration entries : "
                  << result.entries << '\n';

        std::cout << "File generation time  : "
                  << result.fileGenerationMs << " ms\n";

        printStats("Loading performance", result.loading);

        printStats("Lookup performance", result.lookup);

        std::cout << "\nLookup operations     : "
                  << LOOKUP_OPERATIONS << '\n';

        std::cout << "Lookup throughput     : "
                  << result.throughput
                  << " operations/sec\n";

        std::cout << "Verification checksum: "
                  << result.checksum << '\n';

        std::cout << "========================================\n";
    }

    void writeCsv(
        const std::vector<BenchmarkResult>& results)
    {
        std::ofstream file(CSV_FILE);

        if (!file)
        {
            throw std::runtime_error(
                "Unable to create benchmark CSV file."
            );
        }

        file << "entries,file_generation_ms,"
             << "loading_average_ms,loading_median_ms,"
             << "loading_minimum_ms,loading_maximum_ms,"
             << "lookup_average_ms,lookup_median_ms,"
             << "lookup_minimum_ms,lookup_maximum_ms,"
             << "lookup_operations,lookup_ops_per_second,"
             << "checksum\n";

        file << std::fixed << std::setprecision(6);

        for (const BenchmarkResult& result : results)
        {
            file << result.entries << ','
                 << result.fileGenerationMs << ','
                 << result.loading.averageMs << ','
                 << result.loading.medianMs << ','
                 << result.loading.minimumMs << ','
                 << result.loading.maximumMs << ','
                 << result.lookup.averageMs << ','
                 << result.lookup.medianMs << ','
                 << result.lookup.minimumMs << ','
                 << result.lookup.maximumMs << ','
                 << LOOKUP_OPERATIONS << ','
                 << result.throughput << ','
                 << result.checksum << '\n';
        }

        file.close();

        if (!file)
        {
            throw std::runtime_error(
                "Failed while writing benchmark CSV file."
            );
        }
    }
}

int main()
{
    try
    {
        std::cout << "========================================\n";
        std::cout << " CONFIGURATION PARSER - REFINED BENCHMARK\n";
        std::cout << "========================================\n";

        std::cout << "Warm-up runs          : "
                  << WARMUP_RUNS << '\n';

        std::cout << "Measurement runs      : "
                  << MEASUREMENT_RUNS << '\n';

        std::cout << "Lookup operations/run : "
                  << LOOKUP_OPERATIONS << '\n';

        const std::vector<int> testSizes = {
            100,
            1000,
            10000,
            100000
        };

        std::vector<BenchmarkResult> results;

        results.reserve(testSizes.size());

        for (int numberOfEntries : testSizes)
        {
            BenchmarkResult result =
                runBenchmark(numberOfEntries);

            printResult(result);

            results.push_back(std::move(result));
        }

        writeCsv(results);

        std::remove(CONFIG_FILE.c_str());

        std::cout << "\nBenchmark completed successfully.\n";
        std::cout << "CSV report saved to: "
                  << CSV_FILE << '\n';

        return 0;
    }
    catch (const std::exception& error)
    {
        std::remove(CONFIG_FILE.c_str());

        std::cerr << "\nBenchmark failed: "
                  << error.what() << '\n';

        return 1;
    }
}
