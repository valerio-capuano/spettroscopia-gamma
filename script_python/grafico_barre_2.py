import matplotlib.pyplot as plt

# --- 1. DATI ---
# Le tre etichette testuali per l'asse X
etichette = ['picco 1 (soglia)', 'picco 1', 'picco 2'] 

valori = [216.842, 212.77, 142.01] 

# (Opzionale) Se hai anche l'incertezza per questi valori, inseriscila qui.
# Se non hai errori da mostrare, imposta tutto a 0: errori = [0, 0, 0]
errori = [27.111/2, 20.79/2, 23.62/2] 

# --- 2. CREAZIONE GRAFICO ---
plt.figure(figsize=(7, 5))

# Usiamo errorbar! 
# fmt='o' mette il pallino centrale. Se preferisci avere SOLO la barra verticale
# senza nessun punto in mezzo, ti basta cambiare in fmt='none'.
plt.errorbar(etichette, valori, yerr=errori, fmt='none', color='black', ecolor='black', capsize=8, linewidth=1.5)

# Personalizzazione etichette assi e titolo
plt.ylabel('Centroide (canali)', fontsize=12)
plt.title('Confronto per coincidenza', fontsize=14, fontweight='bold')

# Griglia solo orizzontale per guidare l'occhio
plt.grid(axis='y', linestyle='--', alpha=0.5)

# Aggiunge un po' di margine sopra e sotto per non far toccare i bordi
plt.margins(y=0.2)

# Impagina correttamente e salva
plt.tight_layout()
plt.savefig("../grafici/confronto_coincidenza.pdf", format='pdf')

print("Grafico pulito salvato con successo!")