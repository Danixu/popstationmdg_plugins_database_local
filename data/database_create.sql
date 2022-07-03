CREATE TABLE games
               (serial TEXT NOT NULL PRIMARY KEY, title TEXT NOT NULL, region TEXT, languages TEXT);
CREATE INDEX games_serial ON games(serial);

CREATE TABLE md5
               (serial TEXT NOT NULL, version TEXT, track INT NOT NULL, size INT NOT NULL, md5 TEXT NOT NULL);
CREATE INDEX md5_serial ON md5(serial);
CREATE INDEX md5_hash ON md5(md5);

## Test data
# INSERT INTO games VALUES("SCES03884", "Firebugs (Europe) (En,Fr,De,Es,It,Nl,Pt,Sv,No,Da,Fi)", "Europe", "Danish|Dutch|English|Finnish|French|German|Italian|Norwegian|Portuguese|Spanish|Swedish");