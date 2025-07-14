#!/usr/bin/php-cgi
<?php
// header("Content-Type: text/html; charset=utf-8\n\r\n\r\n");
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Define the cookie name
$cookie_name = "visit_count";

// Handle the reset action
if (isset($_POST['reset'])) {
    // Reset the visit count and expire the cookie
    setcookie($cookie_name, "", time() - 3600, "/"); // Delete the cookie
    $visit_count = 1;
} else {
    // Check if the cookie is set
    if (isset($_COOKIE[$cookie_name])) {
        // Get the current visit count from the cookie
        $visit_count = (int)$_COOKIE[$cookie_name] + 1;
    } else {
        // If the cookie is not set, initialize the visit count to 1
        $visit_count = 1;
    }

    // Set the cookie with the updated visit count (expires in 30 days)
    setcookie($cookie_name, $visit_count, time() + (60 * 60 * 24 * 30), "/");
}

// Determine the message based on the visit count
$message = ($visit_count === 1) ? "Welcome! This is your first visit." : "You have visited this page $visit_count times.";
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Visit Counter</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            color: #333;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            text-align: center;
            background-color: #fff;
            padding: 2rem;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        .button {
            margin-top: 1rem;
            padding: 0.5rem 1rem;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        .button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>Visit Counter</h2>
        <p><?php echo $message; ?></p>
        <form action="" method="post">
            <button type="submit" name="reset" class="button">Reset Visit Count</button>
        </form>
    </div>
</body>
</html>
