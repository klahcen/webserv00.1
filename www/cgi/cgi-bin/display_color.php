#!/usr/bin/env php
<?php

// Définir l'en-tête Content-Type
// header("Content-Type: text/html; charset=UTF-8\r\n\r\n");

// Récupérer la couleur sélectionnée par l'utilisateur
$color = isset($_POST['color']) ? $_POST['color'] : (isset($_GET['color']) ? $_GET['color'] : 'white');

// Assurez-vous que la couleur est une des options autorisées pour éviter tout code malveillant
$valid_colors = ['red', 'blue', 'green', 'yellow', 'purple'];
if (!in_array($color, $valid_colors)) {
    $color = 'white'; // couleur par défaut si non valide
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Color Page</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,wght@0,100..900;1,100..900&display=swap');

        body {
            font-family: 'Inter', sans-serif;
            background-color: <?= htmlspecialchars($color) ?>;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }
        .container {
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Your Selected Color is: <?= htmlspecialchars(ucfirst($color)) ?></h1>
        <p>The background color of this page is now <?= htmlspecialchars($color) ?>.</p>
    </div>
</body>
</html>
