CREATE TABLE `activity_logger`.`activity_type` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(255) NOT NULL,
  `display_name` VARCHAR(255) NOT NULL,
  PRIMARY KEY (`id`));

CREATE TABLE `activity` (
  `started_at` bigint(20) NOT NULL,
  `ended_at` bigint(20) DEFAULT NULL,
  `user_id` int(11) NOT NULL,
  `type_id` int(11) NOT NULL,
  PRIMARY KEY (`started_at`,`user_id`),
  KEY `id_idx` (`type_id`),
  CONSTRAINT `id` FOREIGN KEY (`type_id`) REFERENCES `activity_type` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT)

INSERT INTO activity_type(`name`, `display_name`) VALUES
	("activity_1", "Activity A"),
  ("activity_2", "Activity B"),
  ("activity_3", "Activity C"),
  ("activity_4", "Activity D"),
  ("activity_5", "Activity E");

-- TESTING ACTIVITIES
-- [-2, ..] Activity 5
-- [-4, -3] Activity 4
-- [-6, -4] Activity 3
-- [-7, -6] Activity 2
-- [-8, -7] Activity 1
DELETE FROM activity WHERE user_id = 0;
INSERT INTO activity VALUES
  (UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -2 HOUR)), NULL, 0, 5),
  (UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -4 HOUR)), UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -3 HOUR)), 0, 4),
  (UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -6 HOUR)), UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -4 HOUR)), 0, 3),
  (UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -7 HOUR)), UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -6 HOUR)), 0, 2),
  (UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -8 HOUR)), UNIX_TIMESTAMP(DATE_ADD(CURRENT_TIMESTAMP(),INTERVAL -7 HOUR)), 0, 1);
