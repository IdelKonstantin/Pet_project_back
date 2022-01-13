CREATE TABLE song_list (
	id INTEGER PRIMARY KEY NOT NULL, -- Уникальный идентификатор (важно что тип был INTEGER PRIMARY KEY - иначе не будет работать autoincrement)
	song_name VARCHAR(256) NOT NULL, -- Название трека
	song_UID VARCHAR(32) NOT NULL -- Уникальный трека
);

CREATE TABLE request_logs (

	id INTEGER PRIMARY KEY NOT NULL, -- Уникальный идентификатор (важно что тип был INTEGER PRIMARY KEY - иначе не будет работать autoincrement)
	hostname VARCHAR(256) NOT NULL, -- Хост	
	port VARCHAR(5) NOT NULL, -- Порт
	REST_method VARCHAR(16), -- REST API метод
	URI VARCHAR(256),
	req_time TEXT NOT NULL -- "время" в формате POSTGREsql without time zone (2022-01-01 16:30:46.047637)
);
