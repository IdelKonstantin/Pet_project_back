CREATE TABLE song_list (
	id bigserial PRIMARY KEY NOT NULL, -- Уникальный идентификатор
	song_name VARCHAR(256) NOT NULL, -- Название трека
	song_UID VARCHAR(32) NOT NULL -- Уникальный трека
);

CREATE TABLE request_logs (

	id bigserial PRIMARY KEY NOT NULL, -- Уникальный идентификатор
	hostname VARCHAR(256) NOT NULL, -- Хост	
	port VARCHAR(5) NOT NULL, -- Порт
	REST_method VARCHAR(16), -- REST API метод
	URI VARCHAR(256),
	req_time timestamp without time zone NOT NULL
);
