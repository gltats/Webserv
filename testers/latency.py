import subprocess

url = "http://localhost:8080"  # Replace with your server URL
duration = "10s"  # Test duration

cmd = f"wrk -t2 -c10 -d{duration} {url}"
result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

print(result.stdout)
