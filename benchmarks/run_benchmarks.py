import argparse
import csv
import statistics
import subprocess
import time
from pathlib import Path


benchmark_folder = Path(__file__).resolve().parent
project_folder = benchmark_folder.parent
input_folder = benchmark_folder / "inputs"
default_compiler = project_folder / "compiler-benchmark"
default_result_file = benchmark_folder / "results" / "benchmark_results.csv"


def read_arguments():
    parser = argparse.ArgumentParser(
        description="Run repeatable benchmarks for the C compiler"
    )
    parser.add_argument("--runs", type=int, default=30)
    parser.add_argument("--warmups", type=int, default=3)
    parser.add_argument("--compiler", type=Path, default=default_compiler)
    parser.add_argument("--output", type=Path, default=default_result_file)
    return parser.parse_args()


def remove_generated_files(input_file):
    input_file.with_suffix(".i").unlink(missing_ok=True)
    input_file.with_suffix(".s").unlink(missing_ok=True)


def run_compiler(compiler_path, input_file):
    start_time = time.perf_counter_ns()
    # run compiler in benchmark mode
    result = subprocess.run(
        [str(compiler_path), "--benchmark", str(input_file)],
        capture_output=True,
        text=True
    )
    end_time = time.perf_counter_ns()

    if result.returncode != 0:
        raise RuntimeError(
            "Compiler failed for " + str(input_file) + "\n" + result.stderr
        )

    # check for benchmarking data returned by the compiler (stdout)
    benchmark_line = ""
    for line in result.stdout.splitlines():
        if line.startswith("BENCHMARK,"):
            benchmark_line = line

    if benchmark_line == "":
        raise RuntimeError(
            "Compiler did not return benchmark data for " + str(input_file)
        )

    values = benchmark_line.split(",")
    if len(values) != 8:
        raise RuntimeError("Unexpected benchmark output: " + benchmark_line)

    remove_generated_files(input_file)

    return {
        "wall_time_us": (end_time - start_time) // 1000,
        "preprocessing_us": int(values[1]),
        "lexing_us": int(values[2]),
        "parsing_us": int(values[3]),
        "tacky_us": int(values[4]),
        "assembly_generation_us": int(values[5]),
        "code_emission_us": int(values[6]),
        "compiler_total_us": int(values[7])
    }


def expression_count(input_file):
    # returns the expression files number
    return int(input_file.stem.split("_")[-1])


def main():
    arguments = read_arguments()
    # .resolve() from pathlib converts filepaths
    compiler_path = arguments.compiler.resolve()

    if not compiler_path.exists():
        raise FileNotFoundError(
            "Benchmark compiler does not exist: " + str(compiler_path)
        )

    input_files = sorted(
        input_folder.glob("expressions_*.c"),
        # function above
        key=expression_count 
    )
    if not input_files:
        raise FileNotFoundError(
            "No benchmark inputs found. Run generate_inputs.py first."
        )

    arguments.output.parent.mkdir(parents=True, exist_ok=True)
    # create a list of dicts for each runs data 
    rows = []

    for input_file in input_files:
        print("Benchmarking " + input_file.name)

        for unused_warmup in range(arguments.warmups):
            run_compiler(compiler_path, input_file)

        file_rows = []
        for run_number in range(1, arguments.runs + 1):
            result = run_compiler(compiler_path, input_file)
            result["input"] = input_file.name
            result["expression_count"] = expression_count(input_file)
            result["run"] = run_number
            rows.append(result)
            file_rows.append(result)

        # compute stats
        median_total = statistics.median(
            row["compiler_total_us"] for row in file_rows
        )
        median_wall = statistics.median(
            row["wall_time_us"] for row in file_rows
        )
        print(
            "  median compiler time: " + str(median_total) + " us"
        )
        print(
            "  median process time:  " + str(median_wall) + " us"
        )

    field_names = [
        "input",
        "expression_count",
        "run",
        "wall_time_us",
        "preprocessing_us",
        "lexing_us",
        "parsing_us",
        "tacky_us",
        "assembly_generation_us",
        "code_emission_us",
        "compiler_total_us"
    ]

    with arguments.output.open("w", newline="", encoding="utf-8") as output_file:
        writer = csv.DictWriter(output_file, fieldnames=field_names)
        writer.writeheader()
        writer.writerows(rows)

    print("Saved results to " + str(arguments.output))


if __name__ == "__main__":
    main()
