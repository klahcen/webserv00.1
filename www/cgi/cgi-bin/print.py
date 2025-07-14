#!/usr/bin/env python3
import random
while True:
    print("FD")
# Ensure the correct content-type header is printed for HTML
print("Status: 200 OK")
print("Content-Type: text/html")
print("\r\n")

# Generate random colors and sizes for variation
petal_colors = ["#FF6347", "#FF1493", "#8A2BE2", "#32CD32", "#FFD700"]
center_colors = ["#FFD700", "#FF4500", "#DA70D6", "#1E90FF", "#ADFF2F"]
flower_size = random.randint(80, 120)
petal_color = random.choice(petal_colors)
center_color = random.choice(center_colors)

html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dynamic Floral Design</title>
    <style>
        body {{
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f5f5f5;
            font-family: Arial, sans-serif;
        }}
        .flower {{
            position: relative;
            width: {flower_size}px;
            height: {flower_size}px;
            background: {center_color};
            border-radius: 100%;
            animation: sway 2s ease-in-out infinite alternate;
        }}
        .petal {{
            position: absolute;
            width: {flower_size * 0.6}px;
            height: {flower_size * 0.2}px;
            background: {petal_color};
            border-radius: 50px;
        }}
        .petal:nth-child(1) {{ transform: rotate(0deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(2) {{ transform: rotate(45deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(3) {{ transform: rotate(90deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(4) {{ transform: rotate(135deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(5) {{ transform: rotate(180deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(6) {{ transform: rotate(225deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(7) {{ transform: rotate(270deg) translate({flower_size * 0.4}px); }}
        .petal:nth-child(8) {{ transform: rotate(315deg) translate({flower_size * 0.4}px); }}

        @keyframes sway {{
            0% {{ transform: rotate(-10deg); }}
            100% {{ transform: rotate(10deg); }}
        }}
    </style>
    <script>
        function changeFlower() {{
            location.reload();
        }}
    </script>
</head>
<body onclick="changeFlower()">
    <div class="flower">
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
        <div class="petal"></div>
    </div>
</body>
</html>
"""

# Print the HTML content
print(html_content)
