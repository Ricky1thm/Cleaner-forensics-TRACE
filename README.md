🇬🇧 # Vanish — How It Works

## Introduction

**Vanish** is a lightweight Windows utility that performs, with a single click, a series of _anti-forensics_ and _privacy-cleanup_ operations. In short, it:

* **Cleans** system logs, caches, temporary files, history entries, and events that could reveal user activity;  
* **Noises** the disk and registry by re-injecting random traces to mask deletions;  
* **Manipulates timestamps**: it rolls the clock back, performs its operations, then restores the original time so newly created artefacts carry inconsistent dates;  
* Presents everything through a **minimal GUI** with two buttons (*Start* and *Exit*) and a progress bar that shows the overall progress.

> ⚠️  This project is for research and educational purposes **only**.  
> Running Vanish on systems you don’t own **without explicit consent** may violate laws or ethical guidelines.

---

## 1 · Startup & GUI

| Phase          | Details                                                                                                                         |
|----------------|---------------------------------------------------------------------------------------------------------------------------------|
| **wWinMain**   | Registers the window class, creates a **frameless, semi-transparent window** (`WS_POPUP` + layered) sized 350 × 450 px, then centers it. |
| **Painting**   | Inside `WM_PAINT` it draws a **vertical grey→black gradient** and two hand-drawn **custom buttons** (`Start`, `Exit`) using **AlphaBlend**. |
| **Hit-test**   | In `WM_NCHITTEST` the code decides whether the cursor is over a button (`HTCLIENT`) or elsewhere (window drag).                 |
| **Hover**      | In `WM_MOUSEMOVE` the `hover` flags of the buttons are updated to change color on rollover.                                     |
| **Actions**    | `Start` → launches the cleanup sequence; `Exit` → `PostQuitMessage(0)`.                                                         |

> **UI note:** the buttons are GDI sprites, not real BUTTON controls; no *.rc resources are required.

---

## 2 · “Start” Sequence (clean & evasion)

The progress bar (`PBS_SMOOTH`) is created in `ShowProgressBar()` and updated in 10-percent steps. Below is the step plan taken directly from `SS.cpp`:

| Step | Percent | Functions called                                                     | What they do                                                                                                                                                                          |
|------|---------|-----------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1️⃣  | **10 %** | `changeSystemTime`                                                   | Saves the current date/time → sets a _random timestamp_ (2015–today).                                                                                                                 |
| 2️⃣  | **20 %** | `discordcache`, `cleandns`, `windef`                                 | Kills Discord, purges its cache & local storage; flushes DNS; deletes Windows Defender logs.                                                                                           |
| 3️⃣  | **40 %** | `cleanevtx`, `cleanregedit`, `cleancron`, `amcache`                  | Cleans Event Logs (System/Application/Security), Explorer MRU, browser history; schedules replacement of **Amcache.hve**.                                                             |
| 4️⃣  | **60 %** | `cleanhistory`, `cleanvarious`, `DPSClean`, `RestartSvc`, `CleanMemory` | Removes PowerShell & RecentDocs history, crash dumps, empties %Temp%; stops telemetry services (DPS, DiagTrack) then restarts them; wipes 100 MB of RAM.                              |
| 5️⃣  | **70 %** | `Journal`, `filestemp`, `Shadows`, `DeleteBam`, `Prefetch`           | Resets the USN Journal, creates/deletes 30 random `.tmp` files, deletes all **Shadow Copies** and the contents of `C:\\Windows\\Prefetch`, clears the **BAM** registry key.           |
| 6️⃣  | **80 %** | —                                                                    | (buffer — no extra functions; just advances the percentage).                                                                                                                          |
| 7️⃣  | **90 %** | `rsttime`                                                            | Restores the original date/time saved at step 1️⃣.                                                                                                                                    |
| 8️⃣  | **100 %**| `POPJournal`, `Events`, `security`                                   | Re-populates the USN Journal and Temp with random files, **overwrites** the Event Logs with 10 000 “plausible” entries, spawns 10 000 minimized `taskhostw.exe` processes as noise.   |
| —    | **End** | `HideProgressBar`, `PostQuitMessage(0)`                               | Hides the bar → application terminates.                                                                                                                                               |

> **Why this order?**  
> • The clock is skewed **before** generating artefacts → timestamps become inconsistent.  
> • Data is first wiped, then overwritten with random “noise”.  
> • The clock is restored **after** cleanup to avoid obvious traces.

---

## 3 · Key Files

| File         | Role                                               | Note |
|--------------|----------------------------------------------------|------|
| `SS.cpp`     | Entry point + GUI + orchestration of all steps.    | |
| `funzioni.h` | Toolbox with ~40 cleanup, spoofing, and noise routines for logs/registry/disk. | |
| `classi.h`   | `menu` struct, `WriteCallBack` for potential HTTP requests (currently unused in `SS.cpp`). | |

---

## 4 · Requirements

| Item            | Why?                                                                                                                   |
|-----------------|------------------------------------------------------------------------------------------------------------------------|
| **PowerShell 7**| Used for multi-threading and scripted tasks.                                                                           |
| **Privileges**  | The cleaner modifies registry keys under `HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings\\…`; run elevated. |

---

## 5 · Quick Build

```bash
vcpkg install --triplet x64-windows
cl /std:c++17 /EHsc SS.cpp user32.lib gdi32.lib comctl32.lib msimg32.lib dwmapi.lib
```

🇮🇹 # Vanish — Come funziona

## Introduzione

**Vanish** è una piccola utility Windows pensata per eseguire, con un solo click, una serie di operazioni di _anti-forensics_ e _privacy cleanup_. In pratica:

* **Pulisce** log di sistema, cache, file temporanei, cronologia ed eventi che potrebbero rivelare l’attività dell’utente;  
* **Sporca** (“noise”) il disco e i registri reinserendo tracce casuali per mascherare le cancellazioni;  
* **Manipola i timestamp**: porta indietro l’orologio, compie le operazioni, poi ripristina l’ora originale così che i nuovi artefatti abbiano date incoerenti;  
* Il tutto è presentato in una **GUI minimale** con due pulsanti (*Start* e *Exit*) e una progress-bar che indica l’avanzamento.

> ⚠️  L’obiettivo è puramente didattico/di ricerca.  
> Usare Vanish su macchine di terzi **senza consenso** può costituire violazione di legge.

---

## 1 · Avvio & GUI

| Fase           | Dettagli                                                                                                             |
|----------------|----------------------------------------------------------------------------------------------------------------------|
| **wWinMain**   | Registra la classe finestra, crea una **finestra frameless & semi-trasparente** (`WS_POPUP` + layered) di 350×450 px, poi la centra sul monitor. |
| **Disegno**    | In `WM_PAINT` viene ridisegnato un **gradiente verticale grigio→nero** e due _custom buttons_ disegnati a mano con **AlphaBlend** (`Start`, `Exit`). |
| **Hit-test**   | In `WM_NCHITTEST` il codice decide se il cursore è sui pulsanti (allora restituisce `HTCLIENT`) o altrove (trascinamento finestra). |
| **Hover**      | In `WM_MOUSEMOVE` i flag `hover` dei bottoni vengono aggiornati per cambiare colore.                                 |
| **Azioni**     | `Start` → avvia la sequenza di pulizia; `Exit` → `PostQuitMessage(0)`                                                |

> **Nota UI**: i bottoni non sono veri controlli BUTTON; sono sprite GDI. Nessuna dipendenza da resource *.rc.

---

## 2 · Sequenza “Start” (clean & evasion)

La progress-bar (`PBS_SMOOTH`) nasce in `ShowProgressBar()` e viene aggiornata in blocchi di 10 punti. Sotto, lo _step plan_ preso direttamente da `SS.cpp`:

| Ordine | Percentuale | Funzioni chiamate                                                     | Cosa fanno                                                                                                                                                                                      |
|--------|-------------|-----------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1️⃣      | **10 %**    | `changeSystemTime`                                                   | Salva data/ora correnti → imposta _timestamp casuale_ (2015–oggi).                                                                                                                              |
| 2️⃣      | **20 %**    | `discordcache`, `cleandns`, `windef`                                 | Kill Discord, svuota cache & Local Storage; flush DNS; cancella log di Windows Defender.                                                                                                        |
| 3️⃣      | **40 %**    | `cleanevtx`, `cleanregedit`, `cleancron`, `amcache`                 | Pulisce Event Log (System/Application/Security), MRU di Explorer, cronologia browser, programma la sostituzione di **Amcache.hve**.                                                             |
| 4️⃣      | **60 %**    | `cleanhistory`, `cleanvarious`, `DPSClean`, `RestartSvc`, `CleanMemory` | Rimuove history di PowerShell & RecentDocs, crash dumps, svuota %Temp%, stoppa servizi telemetria (DPS, DiagTrack) poi li riavvia; _wipe_ di 100 MB RAM.                                        |
| 5️⃣      | **70 %**    | `Journal`, `filestemp`, `Shadows`, `DeleteBam`, `Prefetch`           | Resetta USN Journal, crea/elimina 30 file .tmp random, cancella tutti i **Shadow Copy** e il contenuto di `C:\\Windows\\Prefetch`, azzera la chiave **BAM**.                                     |
| 6️⃣      | **80 %**    | —                                                                    | (buffer — nessuna funzione extra; percentuale solo progressiva).                                                                                                                                |
| 7️⃣      | **90 %**    | `rsttime`                                                            | Ripristina la data/ora originali salvate al passo 1️⃣.                                                                                                                                            |
| 8️⃣      | **100 %**   | `POPJournal`, `Events`, `security`                                   | Popola USN e cartella Temp con file random, _sovrascrive_ i registri EventLog con 10 000 voci “plausibili”, avvia 10 000 processi `taskhostw.exe` minimizzati per rumorosità.                    |
| —      | **Fine**    | `HideProgressBar`, `PostQuitMessage(0)`                              | Barra sparisce → l’app termina.                                                                                                                                                                 |

> **Perché questo ordine?**  
> • Si altera l’ora **prima** di produrre artefatti → tutti i timestamp diventano incoerenti.  
> • Si pulisce, poi si riscrive con eventi casuali (“noise”).  
> • Si ripristina l’orario **dopo** la pulizia per non lasciare tracce evidenti.

---

## 3 · File principali

| File          | Ruolo                                                         | Nota |
|---------------|--------------------------------------------------------------|------|
| `SS.cpp`      | _Entry point_ + GUI + orchestrazione di tutti gli step.       | |
| `funzioni.h`  | Toolbox con ~40 routine di pulizia, spoofing e noise su log/reg/hdd. | |
| `classi.h`    | Struct `menu`, callback `WriteCallBack` per eventuali richieste HTTP (non ancora usato in `SS.cpp`). | |

---

## 4 · Requisiti

| Cosa          | Perchè?                                                      |
|---------------|--------------------------------------------------------------|
| `Powershell 7`| per un avere l'utilizzo del multi-threading. |
| `Privilegi`   | il Cleaner cambia delle regedit riguardanti la bam impostate i privilegi su HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\bam\State\UserSettings\S-1-5-21-4006182184-4220401772-2549539988-1001 |

---

## 5 · Compilazione rapida

```bash
vcpkg install --triplet x64-windows
cl /std:c++17 /EHsc SS.cpp user32.lib gdi32.lib comctl32.lib msimg32.lib dwmapi.lib
```
