<?php
/**
 * using mysqli_connect for database connection
 */
 
$databaseHost = 'localhost:3306';
$databaseName = 'flooders';
$databaseUsername = 'root';
$databasePassword = '';
$system_email = "initubesiot@gmail.com";
$system_password = "qlcbwwefthbezrku";
$connection = mysqli_connect($databaseHost, $databaseUsername, $databasePassword, $databaseName) or die(mysql_error());


?>