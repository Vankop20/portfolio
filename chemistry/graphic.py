import matplotlib.pyplot as plt

# Открываем файл и считываем данные
with open('name_file.csv', 'r') as file:
    data = [line.strip().split(';') for line in file]

# Преобразуем данные в числа
times = [float(row[0]) for row in data]
concentrations_a = [float(row[1]) for row in data]
concentrations_b = [float(row[2]) for row in data]
concentrations_c = [float(row[3]) for row in data]
concentrations_d = [float(row[4]) for row in data]
concentrations_e = [float(row[5]) for row in data]
concentrations_f = [float(row[6]) for row in data]
concentrations_q = [float(row[7]) for row in data]
concentrations_p = [float(row[8]) for row in data]

# Строим график
plt.plot(times, concentrations_a, label='A')
plt.plot(times, concentrations_b, label='B')
plt.plot(times, concentrations_c, label='C')
plt.plot(times, concentrations_d, label='D')
plt.plot(times, concentrations_e, label='E')
plt.plot(times, concentrations_f, label='F')
plt.plot(times, concentrations_q, label='Q')
plt.plot(times, concentrations_p, label='P')
plt.xlabel('Время')
plt.ylabel('Концентрация')
plt.legend()
plt.show()
