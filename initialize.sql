/* CS 174
// Sahil Bissessur and Vincent Chang
*/

DROP DATABASE IF EXISTS project;
CREATE DATABASE project;
USE project;

CREATE TABLE Employees (
	id INT,
	age INT,
	salary VARCHAR(512) NOT NULL,
	PRIMARY KEY (id)
);


