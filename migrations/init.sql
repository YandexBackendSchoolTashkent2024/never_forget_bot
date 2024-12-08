CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS "user" (
    "id" UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    "telegram_id" INTEGER NOT NULL UNIQUE,
    "username" VARCHAR(255) UNIQUE,
    "name" VARCHAR(255),
    "timezone" VARCHAR(50) NOT NULL DEFAULT 'Asia/Tashkent',  
    "created_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    "updated_at" TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TYPE "token_type" AS ENUM ('YANDEX_CALENDAR', 'YANDEX_GPT');

CREATE TABLE IF NOT EXISTS "token" (
    "id" UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    "type" "token_type" NOT NULL,
    "token" VARCHAR(255) NOT NULL,
    "user_id" UUID NOT NULL,
    "created_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    "updated_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    CONSTRAINT fk_token_user
        FOREIGN KEY("user_id") 
            REFERENCES "user"("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
);

CREATE TYPE "event_type" AS ENUM ('WHILE_NOT_DONE', 'ONE_TIME');
CREATE TYPE "event_status" AS ENUM ('PENDING', 'COMPLETED', 'NOT_COMPLETED');

CREATE TABLE IF NOT EXISTS "event" (
    "id" UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    "user_id" UUID NOT NULL,
    "name" VARCHAR(255) NOT NULL,
    "time" TIMESTAMPTZ NOT NULL,
    "type" "event_type" NOT NULL,
    "status" "event_status" NOT NULL,
    "created_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    "updated_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    CONSTRAINT fk_event_user
        FOREIGN KEY("user_id") 
            REFERENCES "user"("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
);

CREATE TABLE IF NOT EXISTS "notification" (
    "id" UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    "event_id" UUID NOT NULL,
    "time" TIMESTAMPTZ NOT NULL,
    "sent_time" TIMESTAMPTZ NULL,
    "created_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    "updated_at" TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    CONSTRAINT fk_notification_event
        FOREIGN KEY("event_id") 
            REFERENCES "event"("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
);
