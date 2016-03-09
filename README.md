# Spark <-> SMS 2-Way Gateway (using Tropo)<br>
<br>
Usage:<br>
1) Put PHP/INC files on a publicly accessible webserver with PHP support (and MySQL/MariaDB)<br>
2) Create new database (use the definitions found in the sql_import.sql file)<br>
3) Create 1 Tropo WebAPI Application.<br>
	a) Point it's Text Script to your webserver/tropospark.php<br>
	b) Assign a phone number to the application<br>
	c) Save the messaging API key<br>
4) Edit the config.inc file to set your SQL username and password, as well as your Tropo messaging API key from (3c)<br>

