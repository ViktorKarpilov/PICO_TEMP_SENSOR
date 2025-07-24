from http.server import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)

        print("="*50)
        print("RAW HTTP REQUEST:")
        print(f"{self.command} {self.path} {self.request_version}")
        for header, value in self.headers.items():
            print(f"{header}: {value}")
        print()
        print(post_data.decode('utf-8'))
        print("="*50)

        self.send_response(200)
        self.end_headers()
        self.wfile.write(b"Got it!")

server = HTTPServer(('0.0.0.0', 8080), RequestHandler)
print("Listening on port 8080...")
server.serve_forever()