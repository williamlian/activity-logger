package main

import (
	"database/sql"
	"log"
	"net/http"

	_ "github.com/go-sql-driver/mysql"
)

func main() {
	conn, err := sql.Open("mysql", "root:@/activity_logger?parseTime=true")
	if err != nil {
		log.Fatal("failed to connect to db", err)
	}
	db, err := NewDB(conn)
	if err != nil {
		log.Fatal("failed to create db component", err)
	}

	http.Handle(`/start`, startHandler{db: db})
	http.Handle(`/end`, endHandler{db: db})
	http.Handle(`/get`, getHandler{db: db})
	http.HandleFunc(`/app`, func(w http.ResponseWriter, r *http.Request) { http.ServeFile(w, r, "./www/app.html") })
	http.Handle(`/`, http.FileServer(http.Dir("./www")))
	log.Println("Starting Server")
	log.Fatal(http.ListenAndServe(":7231", nil))
}
