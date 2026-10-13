from pathlib import Path


benchmark_folder = Path(__file__).resolve().parent
input_folder = benchmark_folder / "inputs"
expression_sizes = [10, 100, 500, 1000]


def create_expression(number_of_values):
    expression = "1"

    for current_value in range(2, number_of_values + 1):
        # keep the values in range (1-9)
        value = (current_value % 9) + 1
        expression += " + " + str(value)

    return expression


def create_program(number_of_values):
    expression = create_expression(number_of_values)

    return (
        "int main(void)\n"
        "{\n"
        "    return " + expression + ";\n"
        "}\n"
    )


def main():
    input_folder.mkdir(parents=True, exist_ok=True)

    for expression_size in expression_sizes:
        output_path = input_folder / (
            "expressions_" + str(expression_size) + ".c"
        )
        output_path.write_text(
            create_program(expression_size),
            encoding="utf-8"
        )
        print("Created " + str(output_path))


if __name__ == "__main__":
    main()
