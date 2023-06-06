import sys
import numpy as np


def generate_text_file(file_name, y, z):
    average = 32768
    standard_deviation = y

    # Generates random numbers in normal distribuition
    numbers = np.random.normal(average, standard_deviation, z)
    numbers = np.clip(numbers, 0, 65536)

    # Rounds the generated numbers
    numbers = np.round(numbers).astype(int)

    # Writes the numbers in the text file
    with open(file_name, 'w') as arquivo:
        for num in numbers:
            arquivo.write(str(num) + '\n')


# Parameters
z = 1000000

for i in range(1, len(sys.argv)):
    file_name = f'input{i}.txt'
    y = int(sys.argv[i])

    # Generates the text file
    generate_text_file(file_name, y, z)
