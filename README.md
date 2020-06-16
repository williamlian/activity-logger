# Activity Tracker

# Structure

- www: stsatic app web UI
- create.sql: table schema
- arduino: arduino C code
- db.go: DAO
- models.go: models
- handlers.go: HTTP handlers

# Database

Table Activity
- Epoch startedAt
- Epoch endedAt
- Integer   userID
- Integer   typeID (FK)
PK = startedAt,userID

Table ActivityType
- Integer id
- String name
- String displayName

# Endpoint

- POST /start body: userID, typeID
- POST /end body:userID, typeID
- GET /get?userID=x,from=a,to=b,type=raw|summary

# Test

START
curl -i localhost:7231/start -X POST -H 'Content-Type: application/json' -H 'Connection: close' -d '{"userID":0, "typeID":10001}'

END
curl -i localhost:7231/end -X POST -H 'Content-Type: application/json' -d '{"userID":0, "typeID":10001}'

GET SUMMARY
curl localhost:7231/get?userID=0

GET RAW
curl 'localhost:7231/get?userID=0&type=raw'

GET LAST
curl 'localhost:7231/get?userID=0&type=last'

Restart Service
sudo supervisorctl restart activity-logger

# Test cases

- start activity while previous one is not ended
    - shall auto end previous one
- end activity with wrong type
- end already ended activity

# Run

- dep ensure
- go build
