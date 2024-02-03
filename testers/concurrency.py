from concurrent.futures import ThreadPoolExecutor

def make_request(url):
    # Make HTTP requests here

urls = ["http://localhost:8080/page1", "http://localhost:8080/page2"]
concurrency = 5

with ThreadPoolExecutor(max_workers=concurrency) as executor:
    results = list(executor.map(make_request, urls))
