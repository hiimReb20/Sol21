
1. Introduzione
Lo studente deve realizzare un file storage server in cui la memorizzazione dei file avviene in memoria
principale. La capacità dello storage è fissata all’avvio e non varia dinamicamente durante l’esecuzione del server.
Per poter leggere, scrivere o eliminare file all’interno del file storage, il client deve connettersi al server ed utilizzare
una API che dovrà essere sviluppata dallo studente (come descritto nel seguito). Il server esegue tutte le operazioni
richieste dai client operando sempre e solo in memoria principale e mai sul disco.
La capacità del file storage, unitamente ad altri parametri di configurazione, è definita al momento dell’avvio del
server tramite un file di configurazione testuale.
Il file storage server è implementato come un singolo processo multi-threaded in grado di accettare connessioni
da multipli client. Il processo server dovrà essere in grado di gestire adeguatamente alcune decine di connessioni
contemporanee da parte di più client.
Ogni client mantiene una sola connessione verso il server sulla quale invia una o più richieste relative ai file
memorizzati nel server, ed ottiene le risposte in accordo al protocollo di comunicazione “richiesta-risposta”. Un file è
identificato univocamente dal suo path assoluto.

2. Il server
Il file storage server deve essere implementato come un singolo processo multi-threaded secondo lo schema
“master-worker” in cui il numero di threads worker è fissato all’avvio del server sulla base delle informazioni di
configurazione, ed ogni worker può gestire richieste di client diversi. Eventuali altri thread di “supporto” (oltre al
thread main) possono essere presenti nel sistema a discrezione dello studente.
All’avvio del server, il thread main legge il file di configurazione chiamato “config.txt” che può risiedere in una
qualsiasi directory del sistema (NOTA: tale file non è un header file quindi non va incluso ma va parsato
opportunamente!). Il formato di tale file non viene specificato e deve essere deciso e descritto nella relazione dallo
studente. Il file conterrà tutti i parametri che definiscono la configurazione del server, come ad esempio il numero di
thread workers, la dimensione dello spazio di memorizzazione, il nome del socket file, ed altri parametri che lo
studente ritiene utili.
Il processo server accetta connessioni di tipo AF_UNIX da parte dei client. Il nome del file associato al socket è
pubblico e comunque conosciuto ai client (ad esempio “-f /tmp/LSOfiletorage.sk”). Tale processo rimane sempre
attivo in attesa di nuove connessioni da parte di nuovi client oppure in attesa di richieste di operazioni da eseguire
inviate attraverso le connessioni attive.
Il server termina alla ricezione dei segnali SIGINT, SIGQUIT e SIGHUP. Nel caso di ricezione di uno dei due
segnali SIGINT o SIGQUIT, il server termina il prima possibile, ossia non accetta più nuove richieste da parte dei
client connessi chiudendo tutte le connessioni attive (dovrà comunque generare il sunto delle statistiche, descritto nel
seguito). Nel caso di ricezione del segnale SIGHUP, non vengono più accettate nuove richieste da parte di nuovi
client, ma vengono servite tutte le richieste dei client connessi al momento della ricezione del segnale, quindi il
server terminerà solo quando tutti i client connessi chiuderanno la connessione.
Lo spazio di memorizzazione del server ha una capacità limitata, sia nel numero massimo di file memorizzabili
che come spazio effettivo di memorizzazione, ad esempio, il server potrebbe memorizzare al più 100 file e fino al
raggiungimento di 100 Mbytes di spazio (tali informazioni sono passate al server attraverso il file di config).
Si chiede allo studente di gestire tale spazio di memorizzazione come una cache di file. Se il server non ha
capacità sufficiente ad accettare nuovi file, dovrà espellere file per guadagnare capacità. I file espulsi devono essere
inviati al client la cui richiesta ne ha causato l’espulsione (il quale li gestisce come descritto più avanti), e non sono
più accessibili in richieste successive.
Sono possibili diverse politiche di rimpiazzamento dei file nella cache del server a seguito di “capacity misses”,
lo studente dovrà implementare almeno la politica di rimpiazzamento FIFO, mentre politiche più sofisticate (come ad
esempio LRU, LFU, etc..) possono essere implementate opzionalmente.
Durante l’esecuzione, il processo server storage effettua il logging, su un file di log specificato nel file di
configurazione, di tutte le operazioni che vengono richieste dai client o di gestione interna del server (ad esempio,
l’arrivo di una nuova connessione, il nome del file letto e l’ammontare dei byte restituiti al client, la quantità di dati
scritti, se è stata richiesta una operazione di lock su un file, se è partito l’algoritmo di rimpiazzamento dei file della
cache e quale vittima è stata selezionata, etc.). La scelta del formato del file di log è lasciata allo studente.

3. Il client
Il programma client è un programma separato dal server con una propria funzione main. E’ costruito per inviare
richieste per creare/rimuovere/aprire/scrivere/… file nel/dal file storage server esclusivamente attraverso l’API
descritta nella sezione successiva. Il programma client invia richieste al server sulla base degli argomenti specificati
sulla riga di comando. In generale, il protocollo tra client e server è di tipo “richiesta-risposta”. Particolarmente
degno di nota è il caso di richiesta di scrittura di un file che provoca un “capacity miss” e l’espulsione dallo storage
di un file che era stato precedentemente modificato (ad esempio dall’operazione ‘appendToFile’ -- vedere la sezione
successiva). In questo caso, il server risponde alla richiesta di operazione di scrittura inviando il file espulso dalla
cache. Sulla base degli argomenti a linea di comando specificati per il client, tale file verrà buttato via oppure salvato
in una opportuna directory (vedere l’opzione ‘-D’).

4. Interfaccia per interagire con il file server (API)
- int openConnection(const char* sockname, int msec, const struct timespec abstime)
Viene aperta una connessione AF_UNIX al socket file sockname. Se il server non accetta immediatamente la
richiesta di connessione, la connessione da parte del client viene ripetuta dopo ‘msec’ millisecondi e fino allo
scadere del tempo assoluto ‘abstime’ specificato come terzo argomento. Ritorna 0 in caso di successo, -1 in caso
di fallimento, errno viene settato opportunamente.
- int closeConnection(const char* sockname)
Chiude la connessione AF_UNIX associata al socket file sockname. Ritorna 0 in caso di successo, -1 in caso di
fallimento, errno viene settato opportunamente.
- int openFile(const char* pathname, int flags)
Richiesta di apertura o di creazione di un file. La semantica della openFile dipende dai flags passati come secondo
argomento che possono essere O_CREATE ed O_LOCK. Se viene passato il flag O_CREATE ed il file esiste già
memorizzato nel server, oppure il file non esiste ed il flag O_CREATE non è stato specificato, viene ritornato un
errore. In caso di successo, il file viene sempre aperto in lettura e scrittura, ed in particolare le scritture possono
avvenire solo in append. Se viene passato il flag O_LOCK (eventualmente in OR con O_CREATE) il file viene
aperto e/o creato in modalità locked, che vuol dire che l’unico che può leggere o scrivere il file ‘pathname’ è il
processo che lo ha aperto. Il flag O_LOCK può essere esplicitamente resettato utilizzando la chiamata unlockFile,
descritta di seguito.
Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
- int readFile(const char* pathname, void** buf, size_t* size)
Legge tutto il contenuto del file dal server (se esiste) ritornando un puntatore ad un'area allocata sullo heap nel
parametro ‘buf’, mentre ‘size’ conterrà la dimensione del buffer dati (ossia la dimensione in bytes del file letto). In
caso di errore, ‘buf‘e ‘size’ non sono validi. Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene
settato opportunamente.
- int readNFiles(int N, const char* dirname)
Richiede al server la lettura di ‘N’ files qualsiasi da memorizzare nella directory ‘dirname’ lato client. Se il server
ha meno di ‘N’ file disponibili, li invia tutti. Se N<=0 la richiesta al server è quella di leggere tutti i file
memorizzati al suo interno. Ritorna un valore maggiore o uguale a 0 in caso di successo (cioè ritorna il n. di file
effettivamente letti), -1 in caso di fallimento, errno viene settato opportunamente.
- int writeFile(const char* pathname, const char* dirname)
Scrive tutto il file puntato da pathname nel file server. Ritorna successo solo se la precedente operazione,
terminata con successo, è stata openFile(pathname, O_CREATE| O_LOCK). Se ‘dirname’ è diverso da NULL, il
file eventualmente spedito dal server perchè espulso dalla cache per far posto al file ‘pathname’ dovrà essere
scritto in ‘dirname’; Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
- int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname)
Richiesta di scrivere in append al file ‘pathname‘ i ‘size‘ bytes contenuti nel buffer ‘buf’. L’operazione di append
nel file è garantita essere atomica dal file server. Se ‘dirname’ è diverso da NULL, il file eventualmente spedito
dal server perchè espulso dalla cache per far posto ai nuovi dati di ‘pathname’ dovrà essere scritto in ‘dirname’;
Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
- int lockFile(const char* pathname)
In caso di successo setta il flag O_LOCK al file. Se il file era stato aperto/creato con il flag O_LOCK e la
richiesta proviene dallo stesso processo, oppure se il file non ha il flag O_LOCK settato, l’operazione termina
immediatamente con successo, altrimenti l’operazione non viene completata fino a quando il flag O_LOCK non
viene resettato dal detentore della lock. L’ordine di acquisizione della lock sul file non è specificato. Ritorna 0 in
caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
- int unlockFile(const char* pathname)
Resetta il flag O_LOCK sul file ‘pathname’. L’operazione ha successo solo se l’owner della lock è il processo che
ha richiesto l’operazione, altrimenti l’operazione termina con errore. Ritorna 0 in caso di successo, -1 in caso di
fallimento, errno viene settato opportunamente.
- int closeFile(const char* pathname)
Richiesta di chiusura del file puntato da ‘pathname’. Eventuali operazioni sul file dopo la closeFile falliscono.
Ritorna 0 in caso di successo, -1 in caso di fallimento, errno viene settato opportunamente.
- int removeFile(const char* pathname)
Rimuove il file cancellandolo dal file storage server. L’operazione fallisce se il file non è in stato locked, o è in
stato locked da parte di un processo client diverso da chi effettua la removeFile.
