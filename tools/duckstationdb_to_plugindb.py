#!/usr/bin/env python3

'''

    Created by Daniel Carrasco from ElectroSoftCloud.com
    
    To get the gamedata database go to https://github.com/stenzek/duckstation/tree/master/data/database

'''

import json
import sqlite3
from datetime import datetime


now = datetime.now()
database_data = []
region_by_code = {
    # USA Codes
    "SCUS": "USA",
    "SLUS": "USA",
    "SPUS": "USA",
    "PUPX": "USA",
    "LSP": "USA",

    # Europe Codes
    "SCES": "Europe",
    "SLES": "Europe",
    "SCED": "Europe",
    "SLED": "Europe",
    "PEPX": "Europe",

    # Japan Codes
    "SCPS": "Japan",
    "SCPM": "Japan",
    "SLPS": "Japan",
    "SLPM": "Japan",
    "SIPS": "Japan",
    "ESPM": "Japan",
    "SLKA": "Japan",
    "PAPX": "Japan",
    "PCPX": "Japan",
    "PCPD": "Japan",
    "PTPX": "Japan",
    "PBPX": "Japan",
    "HPS":  "Japan",
    "CPCS": "Japan",
    "SCAJ": "Japan",
    "SCZS": "Japan"
}

# Open the duckstation database in JSON
with open('gamedb.json') as json_file:
    database_data = json.load(json_file)

# Open the SQLITE database
sqliteCon = sqlite3.connect(F'games--{now.strftime("%y%m%d-%H%M")}.db')
sqliteCur = sqliteCon.cursor()

# Create tables
sqliteCur.execute('''CREATE TABLE games
               (serial TEXT NOT NULL PRIMARY KEY, title TEXT NOT NULL, region TEXT, languages TEXT);''')
sqliteCur.execute('''CREATE TABLE md5
               (serial TEXT NOT NULL, version TEXT, track INT NOT NULL, size INT NOT NULL, md5 TEXT NOT NULL);''')

# Create indexes
sqliteCur.execute('''CREATE INDEX games_serial ON games(serial);''')
sqliteCur.execute('''CREATE INDEX md5_serial ON md5(serial);''')
sqliteCur.execute('''CREATE INDEX md5_hash ON md5(md5);''')

for item in database_data:
    # Get the region by the code
    regionCode = item["serial"].split("-")[0]
    region = region_by_code.get(regionCode.upper(), "")

    # Insert it into the DB
    sqliteCur.execute(F'''INSERT OR IGNORE INTO games VALUES
               ("{item["serial"].replace("-", "")}", "{item["name"]}", "{region}", "{"|".join(item.get("languages", []))}");''')

    # Get all the md5 hashes and insert it into the database
    for trdata in item.get("track_data", []):
        version = trdata.get("version", "")
        trackNumber = 0
        for track in trdata.get("tracks", []):
            trackNumber += 1
            # Insert it into the DB
            sqliteCur.execute(F'''INSERT INTO md5 VALUES
                        ("{item["serial"].replace("-", "")}", "{version}", {trackNumber}, {track["size"]}, "{track["md5"]}");''')

sqliteCon.commit()

sqliteCon.close()