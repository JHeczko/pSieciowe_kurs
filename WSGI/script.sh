#!/bin/sh

# Skrypt uruchamiający i demonstrujący działanie aplikacji z pliku
# rest_webapp.py. Skrypt przetestowano pod Debianem 7, czyli dystrybucją
# zainstalowaną w pracowniach studenckich.
#
# Przy ręcznym testowaniu  webaplikacji możesz chcieć użyć "curl -v" aby
# zobaczyć nagłówki zapytań i odpowiedzi HTTP.


# Zainicjuj bazę z danymi osób.

rm -f osoby.sqlite

sqlite3 osoby.sqlite "
CREATE TABLE osoby (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    nazwisko TEXT,
    telefon TEXT,
    adres TEXT,

    UNIQUE(imie,nazwisko,telefon,adres)
);
CREATE TABLE psy (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    rasa TEXT,
    w_id INTEGER,

    CONSTRAINT fk_osoby_delete
        FOREIGN KEY (w_id)
        REFERENCES osoby (id)
        ON DELETE CASCADE,
    CONSTRAINT fk_osoby_update
        FOREIGN KEY (w_id)
        REFERENCES osoby (id)
        ON UPDATE CASCADE
);

PRAGMA foreign_keys = ON;

INSERT INTO osoby VALUES (1, 'Anna', 'Nowak', '+48124569988',
    'Rynek Główny 2, 30-001 Kraków');
INSERT INTO osoby VALUES (2, 'Jan', 'Kowalski', '+48127770022',
    'ul. Podzamcze 1, 31-001 Kraków');
INSERT INTO osoby VALUES (3, 'Konrad', 'Kowalski', '+48127770022',
    'ul. Podzamcze 1, 31-001 Kraków');
INSERT INTO osoby VALUES (4, 'Milosz', 'Kowalski', '+48127770022',
    'ul. Podzamcze 1, 31-001 Kraków');
INSERT INTO psy VALUES (1, 'Pimpek', 'Kundel', 2);
INSERT INTO psy VALUES (2, 'Kijna', 'Kogiel Mogiel', 1);
INSERT INTO psy VALUES (3, 'Pies1', 'Kundel', 1);
"

# Dla pewności wypisz na ekran jej zawartość.

echo "Zawartość bazy:"
sqlite3 --header osoby.sqlite "SELECT * FROM osoby"
sqlite3 --header osoby.sqlite "SELECT * FROM psy"
