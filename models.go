package main

// common errors
const (
	ErrAlreadyStarted   = "already_started"
	ErrAlreadyEnded     = "already_ended"
	ErrEndTypeMismatch  = "end_activity_type_not_match"
	ErrPreviousNotEnded = "last_activity_not_ended"
)

// ActivityType ...
type ActivityType struct {
	ID          int
	Name        string
	DisplayName string
}

// Activity ...
type Activity struct {
	StartedAt int64
	EndedAt   *int64
	UserID    int
	Type      ActivityType
}

// Summary ...
type Summary struct {
	TypeID       int
	Name         string
	DisplayName  string
	TotalSeconds int64
}
