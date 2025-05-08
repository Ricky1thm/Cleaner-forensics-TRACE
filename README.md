# Vanish — How It Works 🚀

## Introduction 🔰

**Vanish** is a lightweight Windows utility that performs, with a single click, a series of _anti-forensics_ and _privacy-cleanup_ operations. In short, it:

* 🗑️ **Cleans** system logs, caches, temporary files, history entries, and events that could reveal user activity;  
* 🌀 **Noises** the disk and registry by re-injecting random traces to mask deletions;  
* 🕒 **Manipulates timestamps**: it rolls the clock back, performs its operations, then restores the original time so newly created artefacts carry inconsistent dates;  
* Presents everything through a **minimal GUI** with two buttons (*Start* ▶️ & *Exit* ❌) and a 📊 progress bar that shows the overall progress.

> ⚠️  This project is for research and educational purposes **only**.  
> Running Vanish on systems you don’t own **without explicit consent** may violate laws or ethical guidelines.

---

## 1 · Startup & GUI 🖥️

| Phase          | Details                                                                                                                         |
|----------------|---------------------------------------------------------------------------------------------------------------------------------|
| **wWinMain**   | Registers the window class, creates a **frameless, semi-transparent window** (`WS_POPUP` + layered) sized 350 × 450 px, then centers it. |
| **Painting**   | Inside `WM_PAINT` it draws a **vertical grey→black gradient** and two hand-drawn **custom buttons** (`Start`, `Exit`) using **AlphaBlend**. |
| **Hit-test**   | In `WM_NCHITTEST` the code decides whether the cursor is over a button (`HTCLIENT`) or elsewhere (window drag).                 |
| **Hover**      | In `WM_MOUSEMOVE` the `hover` flags of the buttons are updated to change color on rollover.                                     |
| **Actions**    | `Start` ▶️ launches the cleanup sequence; `Exit` ❌ sends `PostQuitMessage(0)`.                                                  |

> **UI note:** the buttons are GDI sprites, not real BUTTON controls; no *.rc resources are required.

---

## 2 · “Start” Sequence (clean & evasion) 🧹

The progress bar (`PBS_SMOOTH`) is created in `ShowProgressBar()` and updated in 10-percent steps. Below is the step plan taken directly from `SS.cpp`:

| 🔢 | % | Functions called | What they do |
|----|----|-----------------|--------------|
| 1️⃣ | **10** | `changeSystemTime` | Saves the current date/time → sets a _random timestamp_ (2015–today). |
| 2️⃣ | **20** | `discordcache`, `cleandns`, `windef` | Kills Discord, purges its cache & local storage; flushes DNS; deletes Windows Defender logs. |
| 3️⃣ | **40** | `cleanevtx`, `cleanregedit`, `cleancron`, `amcache` | Cleans Event Logs, Explorer MRU, browser history; schedules replacement of **Amcache.hve**. |
| 4️⃣ | **60** | `cleanhistory`, `cleanvarious`, `DPSClean`, `RestartSvc`, `CleanMemory` | Removes PowerShell & RecentDocs history, crash dumps, empties %Temp%; restarts telemetry services; wipes 100 MB RAM. |
| 5️⃣ | **70** | `Journal`, `filestemp`, `Shadows`, `DeleteBam`, `Prefetch` | Resets USN Journal, creates/deletes 30 random `.tmp` files, wipes **Shadow Copies** & `Prefetch`, clears **BAM** key. |
| 6️⃣ | **80** | — | (buffer step; only advances the percentage). |
| 7️⃣ | **90** | `rsttime` | Restores the original date/time saved at step 1️⃣. |
| 8️⃣ | **100** | `POPJournal`, `Events`, `security` | Re-populates USN & Temp with noise, overwrites Event Logs with 10 000 entries, spawns 10 000 minimized `taskhostw.exe` as noise. |
| ✔️ | **End** | `HideProgressBar`, `PostQuitMessage(0)` | Hides the bar → application terminates. |

> **Why this order?**  
> • The clock is skewed **before** generating artefacts → timestamps become inconsistent.  
> • Data is first wiped, then overwritten with random “noise”.  
> • The clock is restored **after** cleanup to avoid obvious traces.

---

## 3 · Key Files 📂

| File         | Role | Note |
|--------------|------|------|
| `SS.cpp`     | Entry point + GUI + orchestration of all steps. | |
| `funzioni.h` | Toolbox with ~40 cleanup, spoofing & noise routines. | |
| `classi.h`   | `menu` struct and `WriteCallBack` (currently unused). | |

---

## 4 · Requirements ⚙️

| Item            | Why? |
|-----------------|------|
| **PowerShell 7**| Provides multi-threading and scripted tasks. |
| **Elevated privileges** 🔒 | Needed to edit registry keys under `HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\bam\\State\\UserSettings\\…`. |

---

## 5 · Quick Build 🛠️

```bash
vcpkg install --triplet x64-windows
cl /std:c++17 /EHsc SS.cpp user32.lib gdi32.lib comctl32.lib msimg32.lib dwmapi.lib
```
# Vanish — Come funziona 🚀

## Introduzione 🔰

**Vanish** è una piccola utility Windows che, con un solo clic, esegue una serie di operazioni di _anti-forensics_ e _privacy cleanup_. In pratica:

* 🗑️ **Pulisce** log di sistema, cache, file temporanei, cronologia ed eventi che potrebbero rivelare l’attività dell’utente;  
* 🌀 **Sporca** il disco e il registro reinserendo tracce casuali per mascherare le cancellazioni;  
* 🕒 **Manipola i timestamp**: porta indietro l’orologio, esegue le operazioni, poi ripristina l’ora originale così che i nuovi artefatti abbiano date incoerenti;  
* Il tutto dentro una **GUI minimale** con due pulsanti (*Start* ▶️ & *Exit* ❌) e una progress-bar 📊 che mostra l’avanzamento.

> ⚠️  Progetto a scopo didattico / di ricerca.  
> Usarlo su macchine altrui **senza consenso** può violare la legge.

---

## 1 · Avvio & GUI 🖥️

| Fase          | Dettagli                                                                                                             |
|---------------|----------------------------------------------------------------------------------------------------------------------|
| **wWinMain**  | Registra la classe finestra, crea una **finestra frameless & semi-trasparente** (`WS_POPUP` + layered) di 350 × 450 px e la centra. |
| **Disegno**   | In `WM_PAINT` disegna un **gradiente verticale grigio→nero** e due **bottoni custom** (`Start`, `Exit`) con **AlphaBlend**. |
| **Hit-test**  | In `WM_NCHITTEST` capisce se il cursore è su un bottone (`HTCLIENT`) o altrove (trascina finestra). |
| **Hover**     | In `WM_MOUSEMOVE` aggiorna il flag `hover` per cambiare colore al passaggio del mouse. |
| **Azioni**    | `Start` ▶️ avvia la sequenza di pulizia; `Exit` ❌ chiude con `PostQuitMessage(0)`. |

---

## 2 · Sequenza “Start” (clean & evasion) 🧹

La progress-bar (`PBS_SMOOTH`) nasce in `ShowProgressBar()` e avanza a scatti del 10 %. Step estratti da `SS.cpp`:

| # | % | Funzioni | Cosa fanno |
|---|---|----------|-----------|
| 1️⃣ | **10** | `changeSystemTime` | Salva data/ora correnti → imposta data casuale (2015–oggi). |
| 2️⃣ | **20** | `discordcache`, `cleandns`, `windef` | Chiude Discord, svuota la cache, flush DNS, cancella log Defender. |
| 3️⃣ | **40** | `cleanevtx`, `cleanregedit`, `cleancron`, `amcache` | Pulisce Event Log, MRU, cronologia browser, prepara nuova **Amcache.hve**. |
| 4️⃣ | **60** | `cleanhistory`, `cleanvarious`, `DPSClean`, `RestartSvc`, `CleanMemory` | Rimuove history PowerShell, dump, `%Temp%`, riavvia servizi telemetria, wipe 100 MB RAM. |
| 5️⃣ | **70** | `Journal`, `filestemp`, `Shadows`, `DeleteBam`, `Prefetch` | Resetta USN, crea/elimina `.tmp`, cancella Shadow Copy & Prefetch, azzera **BAM**. |
| 6️⃣ | **80** | — | Step di buffer (solo avanzamento). |
| 7️⃣ | **90** | `rsttime` | Ripristina la data/ora originale (step 1️⃣). |
| 8️⃣ | **100** | `POPJournal`, `Events`, `security` | Riempie di “rumore” USN, Temp e Event Log; spawna 10 000 `taskhostw.exe`. |
| ✔️ | **Fine** | `HideProgressBar`, `PostQuitMessage(0)` | Nasconde la barra → l’app termina. |

> **Perché questo ordine?**  
> • Si altera l’orologio **prima** di produrre artefatti → i timestamp diventano incoerenti.  
> • Si pulisce, poi si riscrive con rumore casuale.  
> • Si ripristina l’orario **dopo** per non lasciare tracce evidenti.

---

## 3 · File principali 📂

| File         | Ruolo                                                |
|--------------|------------------------------------------------------|
| `SS.cpp`     | Entry point + GUI + orchestrazione.                  |
| `funzioni.h` | ~40 funzioni di cleanup, spoofing e noise.           |
| `classi.h`   | Struttura `menu`, callback `WriteCallBack` (non usato). |

---

## 4 · Requisiti ⚙️

| Cosa                 | Perché? |
|----------------------|---------|
| **PowerShell 7**     | Per sfruttare multi-threading e task scriptati. |
| **Privilegi elevati** 🔒 | Necessari per modificare chiavi in `…\\bam\\State\\UserSettings\\…`. |

---

## 5 · Compilazione rapida 🛠️

```bash
vcpkg install --triplet x64-windows
cl /std:c++17 /EHsc SS.cpp user32.lib gdi32.lib comctl32.lib msimg32.lib dwmapi.lib
```
