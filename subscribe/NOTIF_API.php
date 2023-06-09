<?php

use PHPMailer\PHPMailer\PHPMailer;
use PHPMailer\PHPMailer\SMTP;
use PHPMailer\PHPMailer\Exception;

// Load Composer's autoloader
require 'vendor/autoload.php';
require 'flooders-config.php';

// Create an instance; passing `true` enables exceptions
$mail = new PHPMailer(true);

$status = $_GET['status'];
$ketinggian = $_GET['ketinggian'];
$lokasi = $_GET['lokasi'];

try {
    $subscribers = mysqli_query($connection, "SELECT email FROM subscribers");
    $recipients = []; // Array to store recipient email addresses

    while ($subscriber = mysqli_fetch_array($subscribers)) {
        $recipients[] = $subscriber['email']; // Add email address to the array
    }

    // Server settings
    $mail->SMTPDebug = SMTP::DEBUG_SERVER; // Enable verbose debug output
    $mail->isSMTP(); // Send using SMTP
    $mail->Host = 'smtp.gmail.com'; // Set the SMTP server to send through
    $mail->SMTPAuth = true; // Enable SMTP authentication
    $mail->Username = $system_email; // SMTP username
    $mail->Password = $system_password; // SMTP password
    $mail->SMTPSecure = PHPMailer::ENCRYPTION_SMTPS; // Enable implicit TLS encryption
    $mail->Port = 465; // TCP port to connect to; use 587 if you have set `SMTPSecure = PHPMailer::ENCRYPTION_STARTTLS`

    // Recipients
    $mail->setFrom($system_email, 'Sensor Ketinggian Air');
    $mail->addReplyTo('tidakushreply@yahoo.co.id', 'Information');

    // Content
    $mail->isHTML(true); // Set email format to HTML
    $mail->Subject = 'Status Ketinggian Air';
    $mail->Body = "<h2>Sensor Ketinggian Air</h2><p>Pemberitahuan ! <br> Di daerah " . $lokasi . " terjadi perubahan ketinggian air. Dengan data sebagai berikut : <br><ul><li>Status : " . $status . "</li><li>Jarak : " . $ketinggian . " Cm</li></ul><br><br>Waspada kepada warga yang tinggal di daerah " . $lokasi . " dan pengendara yang hendak melintas. <br><br> <marquee><h1 style='color:red;'>HARAP BERHATI-HATI !!!</h1></marquee></p>";

    // Set email recipients as the collected addresses
    foreach ($recipients as $recipient) {
        $mail->addBcc($recipient);
    }

    $mail->send(); // Send the email

    echo 'Pesan Terkirim';
} catch (Exception $e) {
    echo "Message could not be sent. Mailer Error: {$mail->ErrorInfo}";
}
