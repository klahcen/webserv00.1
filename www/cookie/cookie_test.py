#!/usr/bin/env python3
import cgi
import cgitb
import os
import datetime

cgitb.enable()  # Enable debugging

# Set the content type to HTML
print("Content-Type: text/html\r\n", end="")

# Parse the form data
form = cgi.FieldStorage()

# Check if the request method is POST and if the "message" field is set
if os.environ.get('REQUEST_METHOD') == 'POST' and 'message' in form:
    message_value = form.getvalue('message')

    # Ensure message_value is a string (handle list case)
    if isinstance(message_value, list):  
        message_value = message_value[0]  # Take the first value

    if not message_value:
        print("\r\nError: Message value cannot be empty.")
    else:
        # Calculate the expiration date (1 hour from now)
        expires = (datetime.datetime.utcnow() + datetime.timedelta(hours=1)).strftime("%a, %d-%b-%Y %H:%M:%S GMT")

        # Correctly format the Set-Cookie header
        print(f"Set-Cookie: user_message={message_value}; Expires={expires}; Path=/\r\n", end="")

        # End headers with a blank line
        print("\r\n\r\n")

        # HTML body starts here
        print("<h1>Message Received Successfully</h1>")
        print(f"<p>Your message: <strong>{message_value}</strong></p>")
        print("<p>A cookie has been set with your message.</p>")
else:
    print("\r\n")
    print('''
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Message Form</title>
        </head>
        <body>
            <h1>Message Form</h1>
            <form method="POST" action="">
                <label for="message">Enter Your Message:</label>
                <input type="text" id="message" name="message" placeholder="e.g., Hello World" required>
                <button type="submit">Submit Message</button>
            </form>
        </body>
        </html>
    ''')
