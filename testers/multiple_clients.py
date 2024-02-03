import requests #run : pip install requests
import concurrent.futures

# URL to request
url = "http://localhost:4432/html/index.html"

# Number of concurrent clients
num_clients = 10

# Function to make an HTTP request
def make_request(client_id):
    try:
        response = requests.get(url)
        response_text = response.text
        print(f"Client {client_id}: Response received (length: {len(response_text)})")
    except Exception as e:
        print(f"Client {client_id}: Error - {e}")

# Create a thread pool for concurrent requests
with concurrent.futures.ThreadPoolExecutor(max_workers=num_clients) as executor:
    # Submit tasks for each client
    futures = [executor.submit(make_request, i) for i in range(num_clients)]

    # Wait for all tasks to complete
    concurrent.futures.wait(futures)

print("All clients have completed their requests.")
