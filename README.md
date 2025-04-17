In another terminal: telnet 127.0.0.1 8888
To end the session: Ctrl + ], then quit
To terminate the existing server process: kill -9 <pid>
To confirm it's gone: lsof -i :8888 # Should return nothing