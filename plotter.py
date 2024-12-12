import re
import matplotlib.pyplot as plt

def parse_results(file_path):
    """
    Parsea los resultados de un archivo de texto y devuelve un diccionario con los datos.

    Parameters
    ----------
    file_path : str
        La ruta al archivo de texto con los resultados.
    
    Returns
    -------
    dict
        Un diccionario con los resultados parseados
    """

    results = {}
    with open(file_path, 'r') as file:
        lines = file.readlines()
        current_N = None
        for line in lines:
            line = line.strip()
            if line.startswith("(|N|"):
                match = re.match(r"\(\|N\|\: (\d+), p: ([0-1](?:\.\d+)?)\)", line)
                if match:
                    current_N = int(match.group(1))
                    p = float(match.group(2))
                    if current_N not in results:
                        results[current_N] = {'p': [], 'seq_times': [], 'bloom_times': [], 'false_positive_rates': []}
                    results[current_N]['p'].append(p)
            elif line.startswith("Tiempo promedio busqueda secuencial para:"):
                seq_time = float(line.split(":")[1].strip())
                results[current_N]['seq_times'].append(seq_time)
            elif line.startswith("Tiempo promedio filtro de Bloom:"):
                bloom_time = float(line.split(":")[1].strip())
                results[current_N]['bloom_times'].append(bloom_time)
            elif line.startswith("Porcentaje falsos positivos:"):
                false_positive_rate = float(line.split(":")[1].strip())
                results[current_N]['false_positive_rates'].append(false_positive_rate)
    return results


def plot_results(results):
    """
    Grafica los resultados obtenidos de la función parse_results.

    Parameters
    ----------
    results : dict
        Un diccionario con los resultados parseados
    
    Returns
    -------
    None
    """

    # Obtener los diferentes valores de N y p
    Ns = sorted(results.keys())
    ps = sorted(set(p for data in results.values() for p in data['p']))

    # Guaramos los tiempos de búsqueda secuencial y de Bloom por p
    seq_times_by_p = {p: [] for p in ps}
    bloom_times_by_p = {p: [] for p in ps}

    for N in Ns:
        data = results[N]
        for i, p in enumerate(data['p']):
            seq_times_by_p[p].append(data['seq_times'][i])
            bloom_times_by_p[p].append(data['bloom_times'][i])
    
    # Graficar los tiempos de búsqueda secuencial
    plt.figure(figsize=(14, 7))
    for p in ps:
        plt.plot(Ns, seq_times_by_p[p], label=f'Secuencial, p={p}', marker='o')
    plt.title('Tiempos de Búsqueda Secuencial vs Tamaño del Arreglo (N)')
    plt.xlabel('Tamaño del Arreglo (N)')
    plt.ylabel('Tiempo promedio de búsqueda secuencial (s)')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Graficar los tiempos del filtro de Bloom
    plt.figure(figsize=(14, 7))
    for p in ps:
        plt.plot(Ns, bloom_times_by_p[p], label=f'Filtro de Bloom, p={p}', marker='o')
    plt.title('Tiempos del Filtro de Bloom vs Tamaño del Arreglo (N)')
    plt.xlabel('Tamaño del Arreglo (N)')
    plt.ylabel('Tiempo promedio del filtro de Bloom (s)')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Graficar los tiempos de búsqueda secuencial y de Bloom por p
    fig, axs = plt.subplots(2, 2, figsize=(14, 10))
    axs = axs.flatten()

    for ax, (N, data) in zip(axs, results.items()):
        p_values = data['p']
        seq_times = data['seq_times']
        bloom_times = data['bloom_times']

        ax.plot(p_values, seq_times, label='Búsqueda Secuencial', marker='o')
        ax.plot(p_values, bloom_times, label='Filtro de Bloom', marker='x')

        ax.set_title(f'Tiempos de Búsqueda vs p para |N| = {N}')
        ax.set_xlabel('p')
        ax.set_ylabel('Tiempo promedio (s)')
        ax.legend()
        ax.grid(True)

    plt.tight_layout()
    plt.show()

    # Graficar los porcentajes de falsos positivos
    fig, axs = plt.subplots(2, 2, figsize=(14, 10))
    axs = axs.flatten() 

    for ax, (N, data) in zip(axs, results.items()):
        p_values = data['p']
        false_positive_rates = data['false_positive_rates']

        ax.plot(p_values, false_positive_rates, label='Porcentaje de Falsos Positivos', marker='o')

        ax.set_title(f'Falsos Positivos vs p para |N| = {N}')
        ax.set_xlabel('p')
        ax.set_ylabel('Porcentaje de Falsos Positivos')
        # Agregamos una linea horizontal de Probabilidad esperada teorica: 0.00819372 
        ax.axhline(y=0.00819372, color='r', linestyle='--', label='Probabilidad esperada teorica: 0.00819')
        ax.legend()
        ax.grid(True)

    plt.tight_layout()  # Ajustar el espaciado entre gráficos
    plt.show()


# Main
if __name__ == '__main__':
    # Probabilidad esperada teorica: 0.00819372 
    # Obtenemos los resultados
    results = parse_results('resultados.txt')
    # Graficamos los resultados
    plot_results(results)
    # Anunciamos el fin del programa
    print("Fin del programa")