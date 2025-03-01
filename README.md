# Guida a Git

## Cos'è Git?

Git è un software di controllo versione distribuito creato da Linus Torvalds nel 2005. Permette di gestire la cronologia delle modifiche di un progetto e di collaborare con altri sviluppatori tramite repository locali e remoti.

Un repository Git ha quattro stati principali:

1. **Working directory**: Contiene i file attuali del progetto.
2. **Index (staging area)**: Uno spazio intermedio in cui si preparano i file per il commit (`git add`).
3. **HEAD**: Punta all'ultimo commit registrato (`git commit`).
4. **Remote repository**: Il repository online su un server remoto (`git push`).

Git supporta la gestione dei **branch**, che permettono di sviluppare funzionalità in parallelo senza influenzare il codice principale. Una volta completato lo sviluppo, il branch può essere unito al `main` o `master` tramite `git merge`.

Git permette anche di creare **tag**, che sono versioni specifiche di un progetto, solitamente utilizzate per identificare rilasci importanti.

---

## Configurazioni di base di Git

Prima di iniziare a usare Git, è necessario configurare le proprie credenziali:

```sh
git config --global user.name "IlTuoNome"
git config --global user.email "email@example.com"
```

### Creazione di un repository

Ci sono due modi per inizializzare un repository Git:

1. Creare un nuovo repository:

   ```sh
   git init
   ```

2. Clonare un repository esistente:

   ```sh
   git clone <serverURL>.git
   ```

   **Esempio:**

   ```sh
   git clone https://github.com/username/repository.git
   ```

---

## Configurazione del server remoto

Visualizzare i server remoti configurati:

```sh
git remote -v
```

Aggiungere un nuovo server remoto:

```sh
git remote add origin https://github.com/username/repository.git
```

Aggiornare l'URL del server remoto:

```sh
git remote set-url origin https://<token>@github.com/<username>/<repository>
```

Rinominare un server remoto:

```sh
git remote rename origin nuovo_nome
```

Rimuovere un server remoto:

```sh
git remote rm nome_server
```

---

## Lavorare con Git

### Aggiungere e registrare modifiche

Aggiungere file all'**index**:

```sh
git add <nome_file>
```

Aggiungere tutti i file modificati:

```sh
git add .
```

Escludere file specifici creando un `.gitignore`:

```
*.log
/node_modules/
config.json
```

Registrare i file nello **staging area** e creare un commit:

```sh
git commit -m "Messaggio del commit"
```

Annullare l'ultimo commit (senza perdere le modifiche):

```sh
git reset --soft HEAD~1
```

Annullare l'ultimo commit eliminando le modifiche:

```sh
git reset --hard HEAD~1
```

---

## Sincronizzazione con il server remoto

Aggiornare il repository locale con le modifiche dal server remoto:

```sh
git pull origin <nome_branch>
```

Inviare i commit al repository remoto:

```sh
git push origin <nome_branch>
```

Forzare un push (usare con attenzione!):

```sh
git push --force
```

---

## Controllare lo stato del progetto

Verificare lo stato dei file:

```sh
git status
```

Vedere le differenze tra le versioni dei file:

```sh
git diff
```

Visualizzare la cronologia dei commit:

```sh
git log --oneline --graph --decorate --all
```

Ripristinare un file allo stato dell'ultimo commit:

```sh
git checkout -- <nome_file>
```

---

## Gestione dei Tag

Elenco di tutti i tag disponibili:

```sh
git tag
```

Creare un nuovo tag:

```sh
git tag -a v1.0.0 -m "Versione stabile 1.0.0"
```

Vedere le informazioni di un tag:

```sh
git show v1.0.0
```

Pubblicare un tag sul repository remoto:

```sh
git push origin v1.0.0
```

Eliminare un tag:

```sh
git tag -d v1.0.0
git push origin --delete v1.0.0
```

---

## Gestione dei Branch

Elenco dei branch disponibili:

```sh
git branch
```

Creare un nuovo branch:

```sh
git branch <nome_branch>
```

Spostarsi su un branch:

```sh
git checkout <nome_branch>
```

Creare e spostarsi su un nuovo branch:

```sh
git checkout -b <nome_branch>
```

Unire un branch nel `master`:

```sh
git checkout master
git merge <nome_branch>
```

Eliminare un branch:

```sh
git branch -d <nome_branch>
```

Forzare l'eliminazione di un branch non fuso:

```sh
git branch -D <nome_branch>
```

Pubblicare un nuovo branch:

```sh
git push origin <nome_branch>
```

Eliminare un branch remoto:

```sh
git push origin --delete <nome_branch>
```

---

## Comandi principali di Git

### Inizializzazione

- `git init`: Inizializza un repository Git.
- `git clone <URL>`: Clona un repository remoto.

### Lavorare con i file

- `git add <file>`: Aggiunge un file all'index.
- `git commit -m "messaggio"`: Registra le modifiche.
- `git rm <file>`: Rimuove un file dal repository.

### Controllo dello stato

- `git status`: Mostra lo stato dei file.
- `git log`: Mostra la cronologia dei commit.
- `git diff`: Mostra le differenze tra commit.

### Sincronizzazione

- `git pull origin <branch>`: Scarica le modifiche remote.
- `git push origin <branch>`: Carica le modifiche sul server.

### Gestione dei Branch

- `git branch <nome>`: Crea un nuovo branch.
- `git checkout <nome>`: Passa a un branch.
- `git merge <branch>`: Unisce un branch al corrente.

### Gestione dei Tag

- `git tag -a <versione> -m "messaggio"`: Crea un tag.
- `git push origin <tag>`: Pubblica un tag.

Questa guida fornisce una panoramica completa su Git. Per approfondire, consulta la documentazione ufficiale: [git-scm.com](https://git-scm.com/doc).
