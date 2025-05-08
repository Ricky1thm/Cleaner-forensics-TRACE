# Vanish — Come funziona

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
