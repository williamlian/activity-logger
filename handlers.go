package main

import (
	"encoding/json"
	"errors"
	"io/ioutil"
	"log"
	"net/http"
	"strconv"
	"time"
)

type startHandler struct {
	db DB
}

type postRequest struct {
	UserID int
	TypeID int
}

func (h startHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	log.Println("start request")
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		serverError(w, err)
		return
	}
	var start postRequest
	err = json.Unmarshal(body, &start)
	if err != nil {
		serverError(w, err)
		return
	}
	err = h.db.StartActivity(Activity{
		StartedAt: time.Now().Unix(),
		UserID:    start.UserID,
		Type: ActivityType{
			ID: start.TypeID,
		},
	})
	if err != nil {
		serverError(w, err)
		return
	}
	w.WriteHeader(200)
	w.Write([]byte("OK"))
}

type endHandler struct {
	db DB
}

func (h endHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	log.Println("end request")
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		serverError(w, err)
		return
	}
	var end postRequest // reuse for same field
	err = json.Unmarshal(body, &end)
	if err != nil {
		serverError(w, err)
		return
	}
	err = h.db.EndActivity(end.UserID, end.TypeID)
	if err != nil {
		serverError(w, err)
		return
	}
	w.WriteHeader(200)
	w.Write([]byte("OK"))
}

type getHandler struct {
	db DB
}

func (h getHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	var from, to int64
	log.Println("get request")

	userID, err := strconv.Atoi(r.FormValue("userID"))
	if err != nil {
		serverError(w, errors.New("unknown user ID"))
		return
	}

	getType := r.FormValue("type")
	if getType == "" {
		getType = "summary" // by defaul get returns summary, unless passed type = raw
	}

	fromStr := r.FormValue("from")
	from, err = strconv.ParseInt(fromStr, 10, 64)
	if fromStr == "" || err != nil {
		// default to get all the way to the beginning
		log.Printf("not parsing from: %v (error: %s)\n", fromStr, err)
		from = -1
	}

	toStr := r.FormValue("to")
	to, err = strconv.ParseInt(toStr, 10, 64)
	if toStr == "" || err != nil {
		// default to get all the way to now
		log.Printf("not parsing to: %v (error: %s)\n", toStr, err)
		to = time.Now().Unix()
	}

	var activities interface{}

	if getType == "raw" {
		activities, err = h.db.GetActivityList(userID, from, to)
	} else if getType == "last" {
		lastActivity, lastErr := h.db.GetLastActivity(userID)
		activities = lastActivity.Type.ID
		err = lastErr
	} else {
		activities, err = h.db.GetActivitySummary(userID, from, to)
	}
	if err != nil {
		serverError(w, err)
		return
	}
	jsonByte, _ := json.Marshal(activities)
	w.WriteHeader(200)
	w.Write(jsonByte)
}

func serverError(w http.ResponseWriter, err error) {
	w.WriteHeader(500)
	w.Write([]byte(err.Error()))
	log.Println("request error", err.Error())
}
