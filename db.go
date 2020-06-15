package main

import (
	"database/sql"
	"errors"
	"log"
	"time"
)

// DB ...
type DB interface {
	StartActivity(activity Activity) error
	EndActivity(userID int, typeID int) error
	GetActivityList(userID int, from int64, to int64) ([]*Activity, error)
	GetActivitySummary(userID int, from int64, to int64) ([]*Summary, error)
	GetLastActivity(userID int) (*Activity, error)
}

type db struct {
	conn *sql.DB
}

// NewDB ...
func NewDB(conn *sql.DB) (DB, error) {
	return &db{
		conn: conn,
	}, nil
}

func (c *db) StartActivity(activity Activity) error {
	lastActivity, err := c.GetLastActivity(activity.UserID)
	if err != nil {
		log.Println("failed to read last activity", err)
		return errors.New("failed to start activity. detail=" + err.Error())
	}
	if lastActivity.StartedAt == activity.StartedAt && lastActivity.Type.ID == activity.Type.ID {
		log.Println("same activity within one seconds, ignored")
		return errors.New(ErrAlreadyStarted)
	}
	if lastActivity.EndedAt == nil {
		log.Println("last activity not ended yet, ending it now")
		err = c.EndActivity(lastActivity.UserID, lastActivity.Type.ID)
		if err != nil {
			log.Println("ending last activity failed")
			return errors.New("failed to end activity. detail=" + err.Error())
		}
	}
	_, err = c.conn.Exec(`INSERT INTO activity VALUES(?, ?, ?, ?)`,
		activity.StartedAt,
		nil,
		activity.UserID,
		activity.Type.ID)
	if err != nil {
		log.Println("failed to insert activity", err)
		return errors.New("failed to start activity. detail=" + err.Error())
	}
	return nil
}

func (c *db) EndActivity(userID int, typeID int) error {
	lastActivity, err := c.GetLastActivity(userID)
	if err != nil {
		log.Println("failed to read last activity", err)
		return errors.New("failed to end activity. detail=" + err.Error())
	}
	if lastActivity.EndedAt != nil {
		log.Println("last activity already ended")
		return errors.New(ErrAlreadyEnded)
	}
	if lastActivity.Type.ID != typeID {
		log.Println("end type mismatch")
		return errors.New(ErrEndTypeMismatch)
	}
	_, err = c.conn.Exec(`UPDATE activity SET ended_at = ? WHERE started_at = ? AND user_id = ? AND type_id = ?`,
		time.Now().Unix(), lastActivity.StartedAt, userID, typeID)
	if err != nil {
		log.Println("failed to end activity", err)
		return errors.New("failed to end activity. detail=" + err.Error())
	}
	return nil
}

func (c *db) GetActivityList(userID int, from int64, to int64) ([]*Activity, error) {
	rows, err := c.conn.Query(`SELECT a.started_at, a.ended_at, a.user_id, a.type_id, t.name, t.display_name
		FROM activity a, activity_type t 
		WHERE a.user_id = ? AND a.started_at >= ? AND a.started_at <= ? AND a.type_id = t.id`,
		userID, from, to)
	if err != nil {
		log.Println("failed to get activities", err)
		return nil, errors.New("failed to get activities. detail=" + err.Error())
	}
	activities := []*Activity{}
	for rows.Next() {
		a, err := scanActivity(rows)
		if err != nil {
			log.Println("failed to get activities", err)
			return nil, errors.New("failed to get activities. detail=" + err.Error())
		}
		activities = append(activities, a)
	}
	return activities, nil
}

func (c *db) GetActivitySummary(userID int, from int64, to int64) ([]*Summary, error) {
	rows, err := c.conn.Query(
		`SELECT t.id, t.name, t.display_name, SUM(COALESCE(a.ended_at, UNIX_TIMESTAMP())-a.started_at) AS seconds
		FROM activity a, activity_type t
		WHERE a.user_id = ? AND a.started_at >= ? AND a.started_at <= ? AND a.type_id = t.id
		GROUP BY t.id`,
		userID, from, to,
	)
	if err != nil {
		log.Println("failed to get summary", err)
		return nil, errors.New("failed to get summary. detail=" + err.Error())
	}
	summary := []*Summary{}
	for rows.Next() {
		var (
			typeID            int
			name, displayName string
			totalSeconds      int64
		)
		if err := rows.Scan(&typeID, &name, &displayName, &totalSeconds); err != nil {
			return nil, err
		}
		s := &Summary{
			TypeID:       typeID,
			Name:         name,
			DisplayName:  displayName,
			TotalSeconds: totalSeconds,
		}
		summary = append(summary, s)
	}
	return summary, nil
}

func (c *db) GetLastActivity(userID int) (*Activity, error) {
	var ts sql.NullInt64
	err := c.conn.QueryRow(
		`SELECT MAX(started_at) FROM activity WHERE user_id = ?`, userID).Scan(&ts)
	if err != nil {
		log.Println("failed to read last activity time: " + err.Error())
		return nil, err
	}
	if !ts.Valid {
		// no previous activity, good
		return nil, nil
	}
	// fetch activity
	row := c.conn.QueryRow(
		`SELECT a.started_at, a.ended_at, a.user_id, a.type_id, t.name, t.display_name
		FROM activity a, activity_type t 
		WHERE a.user_id = ? AND a.started_at = ?`,
		userID, ts,
	)
	return scanActivity(row)
}

type scannable interface {
	Scan(...interface{}) error
}

func scanActivity(scanner scannable) (*Activity, error) {
	var (
		startedAt         int64
		endedAt           sql.NullInt64
		userID, typeID    int
		name, displayName string
	)
	if err := scanner.Scan(&startedAt, &endedAt, &userID, &typeID, &name, &displayName); err != nil {
		return nil, err
	}
	act := &Activity{
		StartedAt: startedAt,
		UserID:    userID,
		Type: ActivityType{
			ID:          typeID,
			Name:        name,
			DisplayName: displayName,
		},
	}
	if endedAt.Valid {
		act.EndedAt = &endedAt.Int64
	}
	return act, nil
}
