#!/bin/bash

psql -h 127.0.0.1 -p 5432 -U postgres -f postgre_db.sql
psql -h 127.0.0.1 -p 5432 -U postgres -d petproject_db -f postgre.sql
