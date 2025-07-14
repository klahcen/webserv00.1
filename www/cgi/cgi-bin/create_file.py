#!/usr/bin/env python3

import cgi
import os
from pathlib import Path

# Create an instance of FieldStorage
form = cgi.FieldStorage()

# Get the filename and content as single values
filename = form.getfirst("filename")  # FIX: Ensure it's a string, not a list
content = form.getfirst("content")    # FIX: Ensure it's a string, not a list

# Check if filename is valid
if not filename:
    print("Content-Type: text/html; charset=utf-8\r\n\r\n")
    print("<html><body><h1>Error: No filename provided</h1></body></html>")
    exit()

# Define directory and filepath
directory = "./www/cgi/cgi-bin/uploads/"
filepath = os.path.join(directory, filename)

# Ensure the directory exists
os.makedirs(directory, exist_ok=True)

print("Content-Type: text/html; charset=utf-8\r\n\r\n")  # HTTP header

# Check if the file already exists
if os.path.exists(filepath):
    value = f"Le fichier '{filename}' existe déjà."
else:
    try:
        with open(filepath, "w") as file:
            file.write(content if content else "")  # Avoid NoneType errors
        value = f"Le fichier '{filename}' a été créé avec succès."
    except Exception as e:
        value = f"Échec de la création du fichier '{filename}': {str(e)}"

# Generate HTML output
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Informations de l'Utilisateur</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');
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
        {value}
    </div>
</body>
</html>
"""

print(html_content)
