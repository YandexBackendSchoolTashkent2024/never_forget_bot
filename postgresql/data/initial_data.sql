DROP DATABASE IF EXISTS never_forget_bot_db_1;

CREATE DATABASE IF NOT EXISTS never_forget_bot_db_1;

DROP SCHEMA IF EXISTS never_forget_bot_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS never_forget_bot_schema;

CREATE TABLE IF NOT EXISTS never_forget_bot_schema.users (
    name TEXT PRIMARY KEY,
    count INTEGER DEFAULT(1)
);

INSERT INTO hello_schema.users(name, count)
VALUES ('user-from-initial_data.sql', 42)
ON CONFLICT (name)
DO NOTHING;
