#!/usr/bin/env python3

import cgi
import cgitb

cgitb.enable()  # Enable debugging for CGI scripts

# En-têtes HTTP requis
print("Content-Type: text/html\r\n\r\n")

# Récupérer les données du formulaire
form = cgi.FieldStorage()

# Récupérer la première valeur seulement, en évitant les doublons
name = form.getfirst("name", "Unknown").strip()
age = form.getfirst("age", "Unknown").strip()

# Générer le contenu HTML
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Informations de l'Utilisateur</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter&display=swap');
        body {{
            font-family: 'Inter', sans-serif;
            background-color: #f0f0f0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }}
        .container {{
            text-align: center;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Bienvenue, {name}!</h1>
        <p>Votre âge est {age} ans.</p>
    </div>
</body>
</html>
"""

print(html_content)
