import matplotlib.pyplot as plt
import numpy as np

# --- 1. DATI ---
valori_list = [185.456, 237.777, 241.098, 294.193, 337.181, 350.7315, 509.089, 581.277, 607.319, 724.936, 766.028, 783.23, 792.49, 858.01, 908.427, 931.216, 961.899, 966.044, 1116.961, 1234.434, 1151.93, 1373.598, 1397.47, 1403.934, 1456.574, 1491.796, 1504.806, 1583.595, 1588.094, 1616.05, 1626.02, 1655.902, 1724.688, 1759.478, 1842.080, 2097.33, 2112.40, 2197.715, 2440.49, 2606.944]
errori_list = [1.68, 1.296, 1.46, 1.366, 1.303, 1.442, 2.11, 1.715, 1.577, 2.15, 1.69, 1.37, 2.10, 2.15, 1.761, 2.06, 1.67, 1.745, 1.845, 1.97, 2.24, 1.66, 3.22, 2.01, 2.026, 1.36, 2.78, 2.62, 2.01, 2.86, 3.10, 3.05, 2.20, 2.082, 2.73, 3.48, 2.55, 2.54, 2.30, 2.453]

# INSERISCI QUI LE TUE 40 ENERGIE TABULATE
energie_tabulate_list = [186.21, 238.63, 241.98, 295.21, 338.40, 351.92, 511.0, 583.19, 609.30, 727.17, 768.36, 785.46, 794.70, 860.56, 911.07, 934.10, 964.8, 969.11, 1120.29, 1238.10, 1155.20, 1377.82, 1401.50, 1408.0, 1460.75, 1495.80, 1509.30, 1588.00, 1592.70, 1620.60, 1630.40, 1661.30, 1729.60, 1764.49, 1847.40, 2103.70, 2118.6, 2204.22, 2447.9, 2614.53]

# --- 2. ELABORAZIONE ---
valori = np.array(valori_list)
errori = np.array(errori_list)
energie_tabulate = np.array(energie_tabulate_list)

errori_divisi = errori / 2.0
scarti = energie_tabulate - valori

# --- 3. CREAZIONE GRAFICO ---
# Dimensioni classiche, ottimali per essere inserite in un documento LaTeX o Word
plt.figure(figsize=(10, 6))

# Disegna solo i punti sperimentali con le barre d'errore
plt.errorbar(energie_tabulate, scarti, yerr=errori_divisi, fmt='o', color='black', ecolor='black', capsize=4, markersize=4, linestyle='none')

# Linea di zero per guidare l'occhio
plt.axhline(0, color='red', linestyle='--', linewidth=1, alpha=0.7)

# Etichette degli assi standard
plt.xlabel('Energia Tabulata (keV)', fontsize=12)
plt.ylabel('Scarto (keV)', fontsize=12)
plt.title('Scarto Energie: Misurate vs Tabulate', fontsize=14, fontweight='bold')

# Griglia leggera per facilitare la lettura
plt.grid(axis='both', linestyle='--', alpha=0.5)

plt.tight_layout() 
plt.savefig("../grafici/confronto_incognita.pdf", format='pdf')

print("Grafico pulito salvato con successo!")

# --- CALCOLO FIT E CORRELAZIONE ---

# 1. Coefficiente di correlazione di Pearson (r)
r = np.corrcoef(energie_tabulate, scarti)[0, 1]
print(f"Coefficiente di correlazione lineare (r): {r:.6f}")

# 2. Fit lineare con matrice di covarianza
# parametri contiene [m, q], covarianza è una matrice 2x2
parametri, covarianza = np.polyfit(energie_tabulate, scarti, 1, cov=True)

m = parametri[0]
q = parametri[1]

# Gli errori sono la radice della diagonale della matrice di covarianza
err_m = np.sqrt(covarianza[0, 0])
err_q = np.sqrt(covarianza[1, 1])

# Stampa i risultati con l'errore
print(f"Pendenza (m) = {m:.6e} ± {err_m:.6e}")
print(f"Intercetta (q) = {q:.6f} ± {err_q:.6f} keV")