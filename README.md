# Spettroscopia gamma

Relazione e codice di analisi dati per l'esperimento di **laboratorio di fisica nucleare e particellare**, svolto presso l'Università degli Studi di Catania.

## Obiettivo dell'esperimento
L’emissione di due fotoni in coincidenza da una sorgente di cobalto può essere studiata mediante due rivelatori. In particolare, disponendo di un rivelatore a semiconduttore e di uno a scintillazione, è possibile metterne in evidenza le caratteristiche costruttive con l’utilizzo di altre sorgenti ausiliarie come l’europio, così da avere dei risultati consistenti di incertezza sperimentale. Inoltre, una volta calibrato l’apparato sperimentale, si può studiare anche una sorgente incognita.

## Struttura del repository
* `dati/`
  * `input/` — Dataset grezzi acquisiti in laboratorio.
  * `output/` — File elaborati e risultati intermedi generati dagli script.
* `macro/` — Macro C++ per **ROOT** utilizzate per l'analisi statistica, i fit e la generazione dei grafici.
* `script_python/` — Script Python per la visualizzazione dei confronti finali con barre d'errore (`.pdf`).
* `grafici/` — Grafici finali in formato vettoriale (`.pdf`) inclusi nella relazione.
* `main.tex` — Codice sorgente LaTeX della relazione (con immagine di riferimento `.jpg`).

## Requisiti
Per rieseguire le analisi in locale sono necessari:
* **ROOT** (CERN)
* **Python 3** (con le librerie utilizzate nello script, es. `numpy`, `matplotlib`, `pandas`)
* Una distribuzione **LaTeX** (opzionale, solo per ricompilare il file `.tex`)

## Come eseguire il codice

### 1. Macro ROOT
```bash
root -l macro/spettro.cpp.C
```
*(Prima di eseguire qualsiasi macro, verificare nell'intestazione del codice di aver selezionato i parametri richiesti: nome del file di input, numero di bin, ecc.).*

### 2. Script Python
Installare le librerie principali (se non già presenti):
```bash
pip install numpy matplotlib pandas
```

Eseguire lo script Python:
```bash
python3 script_python/grafico_barre.py
```
