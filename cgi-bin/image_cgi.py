#!/usr/bin/env python

# Required for HTTP headers
print("Content-type: text/html\n")

# HTML content
print("<html>")
print("<head>")
print("<title>Image Page</title>")
print("<style>")
print("body {")
print("    background-color: black; /* Set background color to black */")
print("    color: white; /* Set text color to white */")
print("    font-family: Arial, sans-serif; /* Set font-family */")
print("    text-align: center; /* Center-align text */")
print("}")
print("h1 {")
print("    color: gold; /* Set heading color to gold */")
print("    font-size: 24px; /* Set heading font size */")
print("    margin-bottom: 20px; /* Add space below the heading */")
print("}")
print("p {")
print("    font-size: 36px; /* Set paragraph font size to make it bigger */")
print("}")
print(".banner {")
print("    background-color: #333; /* Set banner background color */")
print("    color: white; /* Set banner text color to white */")
print("    padding: 10px; /* Add padding to the banner */")
print("    text-align: center; /* Center-align text in the banner */")
print("}")
print(".button {")
print("    background-color: #007bff; /* Set button background color to blue */")
print("    color: white; /* Set button text color to white */")
print("    font-size: 18px; /* Set button font size */")
print("    padding: 10px 20px; /* Add padding to the button */")
print("    text-decoration: none; /* Remove underline from link */")
print("    margin: 10px; /* Add margin to the button */")
print("    cursor: pointer; /* Change cursor to pointer on hover */")
print("}")
print("</style>")
print("</head>")
print("<body>")
print("<div class='banner'>")
print("<h1>CGI image (Python3)</h1>")

print("</div>")
print("<div class='image-content'>")
print("<br>")  # Add a line break to separate the image and the button

print("<img src='../html/images/image2.jpeg' alt='Image Description'>")
print("</div>")
print("<div class='button-content'>")
print("<br>")  # Add a line break to separate the image and the button
print("<br>")  # Add a line break to separate the image and the button

print("<button onclick='window.location.href=\"../html/index.html\";' type='button' class='button'>Back to Index</button>")
# print("</div>")

# print("<button onclick='window.location.href=../html/index.html' type='button' class='button'>Back to Index</button>")
print("</div>")
print("</body>")
print("</html>")


# #!/usr/bin/env python

# # Required for HTTP headers
# print("Content-type: text/html\n")

# # HTML content
# print("<html>")
# print("<head>")
# print("<title>CGI Image Page</title>")
# print("<style>")
# print("body {")
# print("    background-color: black; /* Set background color to black */")
# print("    color: white; /* Set text color to white */")
# print("    font-family: Arial, sans-serif; /* Set font-family */")
# print("    text-align: center; /* Center-align text */")
# print("}")
# print("h1 {")
# print("    color: gold; /* Set heading color to gold */")
# print("    font-size: 24px; /* Set heading font size */")
# print("    margin-bottom: 20px; /* Add space below the heading */")
# print("}")
# print(".button {")
# print("    background-color: #007bff; /* Set button background color to blue */")
# print("    color: white; /* Set button text color to white */")
# print("    font-size: 18px; /* Set button font size */")
# print("    padding: 10px 20px; /* Add padding to the button */")
# print("    text-decoration: none; /* Remove underline from link */")
# print("    margin: 10px; /* Add margin to the button */")
# print("    cursor: pointer; /* Change cursor to pointer on hover */")
# print("}")
# print("</style>")
# print("</head>")
# print("<body>")
# print("<div class='banner'>")
# print("<h1>CGI Image View Page</h1>")
# print("</div>")
# # print("<h1>Image Preview</h1>")
# print("<img src='../images/image2.jpeg' alt='Image Description'>")
# print("<br>")  # Add a line break to separate the image and the button
# print("<br>")  # Add a line break to separate the image and the button

# print("<a href='../html/index.html' class='button'>Go Back to Index</a>")
# print("</body>")
# print("</html>")
