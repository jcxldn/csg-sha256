#!/bin/bash

set -e
# Any subsequent(*) commands which fail will cause the shell script to exit immediately

build_folder=$1
output_folder=$2

echo "Using build folder '$1'"
echo "Using output folder '$2'"

mkdir -p $output_folder

targets=(
    benchmark_runner_debug
    benchmark_runner_o2
    benchmark_runner_os
    benchmark_runner_release
    benchmark_runner_release_native
    benchmark_runner_release_optimized
)

for i in "${targets[@]}"; do
    echo "Running '$i'"

    $build_folder/benchmark/$i --benchmark_out=$output_folder/results-$i.json --benchmark_out_format=json
done