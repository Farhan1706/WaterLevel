<?php
/**
 * using mysqli_connect for database connection
 */
 
$databaseHost = 'localhost:3306';
$databaseName = 'flooders';
$databaseUsername = 'root';
$databasePassword = '';
$system_email = "email";
$system_password = "password-aplikasi google";
$connection = mysqli_connect($databaseHost, $databaseUsername, $databasePassword, $databaseName) or die(mysql_error());


?>