Project 3
Αγγελική Φελιμέγκα
1115201300192

Το Project 3 αποτελείται από τα εξής: MirrorInitiator.cpp MirrorServer.cpp ContentServer.cpp Workers_MirrorManagers.cpp LinkdedList.cpp
το header file:Workers_Managers.h LinkdedList.h


MirrorInitiator.cpp: Στο πρόγραμμα αυτό δημιουργείται το απαραίτητο socket και γίνεται η μεταφορά των μηνυμάτων στον MirrorServer.

MirrorServer.cpp: Το πρόγραμμα αυτό επικοινωνεί με τον MirrorInitiator και μέσω socket διαβάζει τα ContentServerAddress:ContentServerPort:dirorfile:delay, τα αποθηκεύει σε ενάν πίνακα από Structs και στην συνέχεια δημιουργεί τα workers και MirrorManager threads και τα καλεί έτσι ώστε αυτά να αναλάβουν την διεκπεραίωση των απαιτούμενων ενεργειών.

Workers_MirrorManagers.cpp - Workers_MirrorManagers.h: Τα αρχεία αυτά αποτελούνται απο τις συναρτήσεις: 
	void place ( pool_t * pool , string data , string ContentServerAddress, string ContentServerPort);
	char * obtain ( pool_t * pool, string &Address, int &port, char * file );
	void * MirrorManagerJob ( void * ptr );
	void * workerJob ( void * ptr );
H place τοποθετεί ένα ένα τα μηνυματα με τα αρχεία και τις διευθύννσεις τους που έχουν ληφθεί απο τον ContentServer με την εντολή List  στην LinkdedList.
Η obtain ψάχνει αν τα ζητούμενα αρχεία που έχουν ληφθεί από τον MirrorInitiator βρίσκονται στην LinkdedList τα ανασύρει και διαγράφει τον κόμβο αφού πια δεν χρειάζεται.
Η MirrorManagerJob αναλαμβάνει να στείλει αίτημα List στον ContentServer μέσω socket και να λάβει τα αρχεία που διαθέτει ο ContentServer και στην συνέχεια να καλέσει την place ώστε να αποθηκευτούν.
Η workerJob αναλαμβάνει να πάρει ένα ένα τα ζητούμενα αρχεία που ζητά ο MirrorInitiator και καλεί την obtain για να ελεγξει αν υπάρχει το αρχείο και στην συνέχεια μέσω socket στένει αίτημα FETCH στον ContentServer και αντιγράφονται τα περιεχόμενα του φακέλου ή του αρχείου στον MirrorServer.

ContentServer.cpp: Το πρόγραμμα αυτό επικοινωνεί με τον MirrorServer.Πιο συγκεκριμένα, με την συνάρτηση MirrorManagerJob δημιουργείται socket για τα αιτήματα LIST όπου μεταφέρονται όλα τα πιθανά αρχεία και κατάλογοι που μπορεί να έχει ζητήσει ο MirrorInitiator και με την συνάρτηση workerJob δημιουργείται socket για τα αιτήματα FETCH ώστε να μεταφερθούν τα ζητούμενα περιεχόμενα στον MirrorServer.

LinkdedList.cpp-LinkdedList.h: Στα αρχεία αυτά υπάρχει η κλάση μιας συνδεδεμένης λίστας με τα απαραιτητα μέλη για την διεξαγωγή της αποθήκευσης των αρχείων-διευθύνσεων και την εξαγωγή τουςγια τις ανάγκες τις άσκησης.

Περιέχεται επίσης makefile.

Εντολές εκτέλεσης:

$make all
$./MirrorInitiator -n MirrorServerAddress -p 59579 -s ContentServerAddress1:ContentServerPort1:dirorfile1:delay1,ContentServerAddress2:ContentServerPort2:dirorfile2:delay2
$./MirrorServer -p <port> -m <dirname> -w <threadnum>
$./ContentServer -p <port> -d dirname
